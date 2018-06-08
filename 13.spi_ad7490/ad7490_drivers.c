#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/wait.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio.h>
#include <mach/gpio-exynos4.h>
#include <linux/spi/spi.h>
#include <linux/spi/spidev.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kk");

struct spi_device *my_spi;

static int write_test(unsigned char *buffer, int len)
{
	int status;
	struct spi_transfer	t = {
		.tx_buf		= buffer,
		.len		= len,
	};
	struct spi_message	m;
	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	DECLARE_COMPLETION_ONSTACK(done);
	m.complete = complete;
	m.context = &done;
	
	printk("spi_async send begin!\n");
	status = spi_async(my_spi,&m);
	if(status == 0){
		wait_for_completion(&done);
		status = m.status;
		if (status == 0)
			status = m.actual_length;
	}
	return status;
}

static int read_test(unsigned char *buffer, int len)
{
	int status;
	struct spi_transfer	t = {
		.rx_buf		= buffer,
		.len		= len,
	};
	struct spi_message	m;
	spi_message_init(&m);
	spi_message_add_tail(&t, &m);
	DECLARE_COMPLETION_ONSTACK(done);
	m.complete = complete;
	m.context = &done;
	
	printk("spi_async read begin!\n");
	status = spi_async(my_spi,&m);
	if(status == 0){
		wait_for_completion(&done);
		status = m.status;
		if (status == 0)
			status = m.actual_length;
	}
	return status;
}

void ad7490_test(){
	int i = 0;
	for(i=0;i<100000;i++){
		short cmd = 0x833;
		write_test(&cmd,2);
		short ans = 0;
		read_test(&ans,2);
		printk("ad value is %d\n",ans);
	}	
}

static int __devinit ad7490_probe(struct spi_device *spi)
{
	
	printk("ad7490_probe!\n");
	
	my_spi = spi;
	ad7490_test();
	
	return 0;
}

static int __devexit ad7490_remove(struct spi_device *spi)
{
	printk("ad7490_remove!\n");
	return 0;
}
static struct spi_driver ad7490_spi_driver = {
	.driver = {
		.name  = "ad7490",
		.owner = THIS_MODULE,
	},
	.probe =	ad7490_probe,
	.remove = __devexit_p(ad7490_remove),

};
static int ad7490_init(){

	spi_register_driver(&ad7490_spi_driver);
	return 0;
}
static void ad7490_exit(){
	spi_unregister_driver(&ad7490_spi_driver);
}
module_init(ad7490_init);
module_exit(ad7490_exit);

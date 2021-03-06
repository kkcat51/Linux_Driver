#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio.h>
#include <mach/gpio-exynos4.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("kk");

#define MAJOR 210

struct cdev *my_cdev;

static int leds_open(struct inode* inode,struct file* file){
	printk("led open\n");
	return 0;
}
static int leds_release(struct inode* inode,struct file* file){
	printk("leds release\n");
	return 0;
}
static long leds_ioctl(struct file* file,unsigned int cmd,unsigned long arg){
	printk("cmd is %d,arg is %d\n",cmd,arg);
	if(cmd>1 || arg>1){
		printk("input error\n");
		return -1;
	}
	gpio_set_value(EXYNOS4_GPL2(0),cmd);
	return 0;
}

static struct file_operations leds_ops = {
	.owner = THIS_MODULE,
	.open = leds_open,
	.release = leds_release,
	.unlocked_ioctl = leds_ioctl,
};
static int leds_init(){

	dev_t devId = MKDEV(MAJOR,0);//参数1为主设备号,参数2为次设备号
	my_cdev = cdev_alloc();
	cdev_init(my_cdev,&leds_ops);
	int err = cdev_add(my_cdev,devId,1);//从设备号devId开始连续添加1个设备
	if(err<0){
		printk("cdev_add failed\n");
		return -1;
	}
	int ret = gpio_request(EXYNOS4_GPL2(0),"LED");
	if(ret<0){
		printk("gpio request failed!\n");
		return -1;
	}
	s3c_gpio_cfgpin(EXYNOS4_GPL2(0),S3C_GPIO_OUTPUT);
	gpio_set_value(EXYNOS4_GPL2(0),0);
	return 0;
}
static void leds_exit(){
	cdev_del(my_cdev);
	gpio_free(EXYNOS4_GPL2(0));
	printk("cdev_del \n");
}
module_init(leds_init);
module_exit(leds_exit);

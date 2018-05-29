#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/gpio.h>
#include <linux/platform_device.h>

#include <linux/regulator/consumer.h>
#include <mach/gpio.h>
#include <mach/irqs.h>
#include <plat/gpio-cfg.h>
#include <plat/iic.h>
#include <plat/ft5x0x_touch.h>

static struct i2c_board_info i2c_devs3  = {
	I2C_BOARD_INFO("i2c_test", 0x70>>1),
	.irq = IRQ_EINT(4),	
};

static struct i2c_client *i2c_client;  

static int i2c_tes_read_reg(struct i2c_client *client,u8 addr, u8 *pdata) {
	u8 buf1[4] = { 0 };
	u8 buf2[4] = { 0 };
	struct i2c_msg msgs[] = {
		{
			.addr	= client->addr,	//0x38
			.flags	= 0,	//写
			.len	= 1,	//要写的数据的长度
			.buf	= buf1,
		},
		{
			.addr	= client->addr,
			.flags	= I2C_M_RD,
			.len	= 1,
			.buf	= buf2,
		},
	};
	int ret;
	buf1[0] = addr;
	ret = i2c_transfer(client->adapter, msgs, 2);
	printk("i2c address 0x%02x \n",client->addr);
	if (ret < 0) {
		pr_err("read reg (0x%02x) error, %d\n", addr, ret);
	} else {
		*pdata = buf2[0];
	}
	return ret;
}
static int i2c_tes_read_fw_reg(struct i2c_client *client,unsigned char *val)
{
	int ret;
	*val = 0xff;
	ret = i2c_tes_read_reg(client,0xa6, val);
	printk("ts reg 0xa6 val is %d\n",*val);
	return ret;
}


static int i2c_test_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	unsigned char val;
	printk("==%s:\n", __FUNCTION__);
	
	i2c_tes_read_fw_reg(client,&val);
	
	return 0;
}

static int __devexit i2c_test_remove(struct i2c_client *client)
{
	i2c_set_clientdata(client, NULL);
	printk("==%s:\n", __FUNCTION__);
	return 0;
}

static const struct i2c_device_id i2c_test_id[] = {
	{ "i2c_test", 0 },
	{ }
};

static struct i2c_driver i2c_test_driver = {
	.probe		= i2c_test_probe,
	.remove		= __devexit_p(i2c_test_remove),
	.id_table	= i2c_test_id,
	.driver	= {
		.name	= "i2c_test",
		.owner	= THIS_MODULE,
	},
};

//i2c设备上电初始化
static void i2c_io_init(void)
{
	int ret;
	ret = gpio_request(EXYNOS4_GPL0(2), "TP1_EN");
	if (ret<0) {
		printk(KERN_ERR "failed to request TP1_EN for "
				"I2C control\n");
		//return err;
	}
	gpio_direction_output(EXYNOS4_GPL0(2), 1);
	s3c_gpio_cfgpin(EXYNOS4_GPL0(2), S3C_GPIO_OUTPUT);
	gpio_free(EXYNOS4_GPL0(2));
	mdelay(5);
	
	ret = gpio_request(EXYNOS4_GPX0(3), "GPX0_3");
	if (ret<0) {
		gpio_free(EXYNOS4_GPX0(3));
		ret = gpio_request(EXYNOS4_GPX0(3), "GPX0_3");
		if(ret<0)
		{
			printk("ft5xox: Failed to request GPX0_3 \n");
		}
	}
	gpio_direction_output(EXYNOS4_GPX0(3), 0);
	mdelay(200);
	gpio_direction_output(EXYNOS4_GPX0(3), 1);
	s3c_gpio_cfgpin(EXYNOS4_GPX0(3), S3C_GPIO_OUTPUT);
	gpio_free(EXYNOS4_GPX0(3));
	msleep(300);	
}

static int __init i2c_test_init(void)
{
	printk("==%s:\n", __FUNCTION__);

	i2c_io_init();

	struct i2c_adapter *i2c_adap;  
  
    i2c_adap = i2c_get_adapter(3);
    i2c_client = i2c_new_device(i2c_adap, &i2c_devs3);  
    i2c_put_adapter(i2c_adap);  


	printk("==%s:\n", __FUNCTION__);
	return i2c_add_driver(&i2c_test_driver);
}
static void __exit i2c_test_exit(void)
{
	printk("==%s:\n", __FUNCTION__);
	i2c_unregister_device(i2c_client);  
	i2c_del_driver(&i2c_test_driver);
	printk("==%s:\n", __FUNCTION__);
}


module_init(i2c_test_init);
module_exit(i2c_test_exit);


MODULE_AUTHOR("KK");
MODULE_DESCRIPTION("i2c_test");
MODULE_LICENSE("GPL");

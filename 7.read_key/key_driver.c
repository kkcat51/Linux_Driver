#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <plat/gpio-cfg.h>
#include <mach/gpio.h>
#include <mach/gpio-exynos4.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("kk");

#define DEVICE_NAME "keys"

static int key_gpios[] = {
	EXYNOS4_GPX1(1),
	EXYNOS4_GPX1(2),
};

static int keys_open(struct inode* inode,struct file* file){
	printk("key open\n");
	return 0;
}
static int keys_release(struct inode* inode,struct file* file){
	printk("keys release\n");
	return 0;
}
static ssize_t keys_read(struct file *filp, char __user *buff, size_t size, loff_t *ppos)
{
	unsigned char key_value[2];
	
	if(size != sizeof(key_value)){
		return -1;
	}
	
	key_value[0] = gpio_get_value(key_gpios[0]);
	key_value[1] = gpio_get_value(key_gpios[1]);
	
	copy_to_user(buff,key_value,sizeof(key_value));
	return 0;
}

static struct file_operations keys_ops = {
	.owner = THIS_MODULE,
	.open = keys_open,
	.release = keys_release,
	.read = keys_read,
};
static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &keys_ops,
};
static int keys_init(){

	int ret,i;
	misc_register(&misc);
	for(i=0;i<2;i++){
		ret = gpio_request(key_gpios[i],"key_gpio");
		if(ret<0){
			printk("gpio request failed!\n");
			return -1;
		}
		s3c_gpio_cfgpin(key_gpios[i],S3C_GPIO_INPUT); 
		s3c_gpio_setpull(key_gpios[i],S3C_GPIO_PULL_NONE);
	}
	return 0;
}
static void keys_exit(){
	
	int i;
	misc_deregister(&misc);
	for(i=0;i<2;i++){
		gpio_free(key_gpios[i]);
	}
	
	printk("cdev_del \n");
}
module_init(keys_init);
module_exit(keys_exit);

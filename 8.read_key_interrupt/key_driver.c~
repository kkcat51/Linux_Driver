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
//中断头文件
#include <linux/irq.h>
#include <linux/interrupt.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("kk");

#define DEVICE_NAME "keys"
char* device_name = "keys";

static wait_queue_head_t wq;

static char buffer[2];

static bool have_data = false;

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
	wait_event_interruptible(wq, have_data==true);
	if(copy_to_user(buff,buffer,2))
		return -EFAULT;
	have_data = false; 
	return size;
}

static irqreturn_t eint9_interrupt(int irq,void *dev_id)
{
	buffer[0] = 9;
	buffer[1] = 1;
	have_data = true;
	wake_up_interruptible(&wq);
	return IRQ_HANDLED;
}

static irqreturn_t eint10_interrupt(int irq,void *dev_id)
{
	buffer[0] = 9;
	buffer[1] = 1;
	have_data = true;
	wake_up_interruptible(&wq);
	return IRQ_HANDLED;
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

	misc_register(&misc);
	int ret1,ret2;	
	
	//申请中断,无需配置GPIO
	ret1 = request_irq(IRQ_EINT(9),eint9_interrupt,IRQ_TYPE_EDGE_FALLING,"my_eint9",device_name);
	ret2 = request_irq(IRQ_EINT(10),eint10_interrupt,IRQ_TYPE_EDGE_FALLING,"my_eint10",device_name);
	if(ret1<0 || ret2<0){
		printk("irq request failed");
		return -EFAULT;
	}
	init_waitqueue_head(&wq); 
	return 0;
}
static void keys_exit(){
	
	free_irq(IRQ_EINT(9),device_name);
	free_irq(IRQ_EINT(10),device_name);
	misc_deregister(&misc);
}
module_init(keys_init);
module_exit(keys_exit);

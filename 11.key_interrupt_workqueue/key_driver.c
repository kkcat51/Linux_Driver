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

char* device_name = "keys";

static char buffer[2];

/* 定义工作队列 */
void key_wq_func(unsigned long data);
struct work_struct wq;


//工作队列处理函数底半部
void key_wq_func(unsigned long data){
	printk("%d key is pressed\n",buffer[0]);
}


//中断处理顶半部
static irqreturn_t eint9_interrupt(int irq,void *dev_id)
{
	printk("eint9_interrupt \n");
	buffer[0] = 9;
	buffer[1] = 1;
	schedule_work(&wq);
	return IRQ_HANDLED;
}

//中断处理顶半部
static irqreturn_t eint10_interrupt(int irq,void *dev_id)
{
	printk("eint10_interrupt \n");
	buffer[0] = 10;
	buffer[1] = 1;
	schedule_work(&wq);
	return IRQ_HANDLED;
}


static int keys_init(){

	//初始化工作队列
	INIT_WORK(&wq,key_wq_func);
	int ret1,ret2;	
	
	//申请中断,无需配置GPIO
	ret1 = request_irq(IRQ_EINT(9),eint9_interrupt,IRQ_TYPE_EDGE_FALLING,"my_eint9",device_name);
	ret2 = request_irq(IRQ_EINT(10),eint10_interrupt,IRQ_TYPE_EDGE_FALLING,"my_eint10",device_name);
	if(ret1<0 || ret2<0){
		printk("irq request failed");
		return -EFAULT;
	}
	return 0;
}
static void keys_exit(){
	
	free_irq(IRQ_EINT(9),device_name);
	free_irq(IRQ_EINT(10),device_name);
}
module_init(keys_init);
module_exit(keys_exit);

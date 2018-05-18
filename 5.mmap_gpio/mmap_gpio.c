#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>

int main(){
	int fd=0;
	void *start = NULL,*reg = NULL;
	fd = open("/dev/mem",O_RDWR);
	if(fd<0){
		printf("open mem failed\n");
		return -1;	
	}
	//参数1:指定映射到的内核虚拟地址,NULL表示由内核决定
	//参数2:映射大小,最小为1页.必为页大小的整数倍
	//参数3:映射区的权限
	//参数4:对映射区的修改改变映射区的内容
	//参数5:映射的文件描述符
	//参数6:一个页的起始物理地址
	start = mmap(NULL,1024*4,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0x11000000);
	if(start==NULL){
		printf("mmap  error\n");
		return -1;
	}

	reg = start+0x0100;//GPL2CON寄存器地址
	*(unsigned int*)reg &= 0;
	*(unsigned int*)reg |= 0x00000001;//设置GPL2_0为输出

	reg = start+0x0104;//GPL2DAT寄存器地址
	
	while(1){
		*(unsigned int*)reg |= 0x01;
		sleep(1);
		*(unsigned int*)reg &= 0x00;
		sleep(1);
	}


}

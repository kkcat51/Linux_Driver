#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main(){
	int fd = open("leds",O_RDWR|O_NDELAY);
	if(fd<0)
		printf("leds open fail\n");
	else{
		printf("leds open success\n");
		ioctl(fd,1,1);
		sleep(2);
		ioctl(fd,0,1);
		sleep(2);
		ioctl(fd,1,1);
	}
	close(fd);
}

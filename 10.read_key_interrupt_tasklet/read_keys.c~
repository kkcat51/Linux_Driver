#include <stdio.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

int main()
{
	int fd;
	unsigned char buffer[2];
	char *read_key = "/dev/keys";

/*O_RDWR只读打开,阻塞方式*/	
	if((fd = open(read_key,O_RDWR))<0){
		printf("APP open %s failed\n",read_key);
		return -1;
	}
	printf("APP open %s success!\n",read_key);
	while(1){
		read(fd,buffer,sizeof(buffer));
		printf("button %d is clicked",buffer[0]);
	}
	
	close(fd);
}

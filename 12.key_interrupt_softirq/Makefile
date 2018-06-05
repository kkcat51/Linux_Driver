#!/bin/bash

obj-m += key_driver.o

KDIR := /home/topeet/iTop4412_Kernel_3.0
PWD ?= $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm *.o *.symvers *.order key_driver.ko

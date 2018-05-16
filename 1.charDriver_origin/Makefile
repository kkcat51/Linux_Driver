#!/bin/bash

obj-m += leds_drivers.o

KDIR := /home/topeet/iTop4412_Kernel_3.0
PWD ?= $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules
clean:
	rm *.o *.symvers *.order leds_driver.ko

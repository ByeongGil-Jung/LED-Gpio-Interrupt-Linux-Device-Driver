KDIR := /home/bkbuntu/Desktop/xslab/source/V-Raptor_BMC_Won/kernel
PWD := $(shell pwd)

COMPILER = arm-linux-gnueabihf-
ARCH = arm

obj-m = my_device_interrupt.o

defualt:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) CROSS_COMPILE=$(COMPILER) ARCH=$(ARCH) modules

clean:
	rm -rf *.ko
	rm -rf *.mod.*
	rm -rf .*.cmd
	rm -rf *.o

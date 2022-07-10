# Makefile para el módulo holamundo.ko
obj-m = holamundo.o
KERNELDIR = /lib/modules/$(shell uname -r)/build
PWD = $(shell pwd)
all:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
clean:
	rm -rf *.ko *.mod.c *.o *.order *.symvers .tmp* .holamundo*

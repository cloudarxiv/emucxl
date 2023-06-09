# If called directly from the command line, invoke the kernel build system.
ifeq ($(KERNELRELEASE),)

	KERNEL_SOURCE := /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default: module emucxl_app

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

emucxl_app: emucxl_app.c emucxl_lib.c emucxl_lib.h
	gcc -o emucxl_app emucxl_app.c emucxl_lib.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	${RM} emucxl_lib emucxl_app

# Otherwise KERNELRELEASE is defined; we've been invoked from the
# kernel build system and can use its language.
else

	obj-m := emucxl_mmap.o

endif

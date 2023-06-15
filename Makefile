# If called directly from the command line, invoke the kernel build system.
ifeq ($(KERNELRELEASE),)

	KERNEL_SOURCE := /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default: module memcxlib_app

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

memcxlib_app: memcxlib_app.c memcxlib_lib.c memcxlib_lib.h
	gcc -o memcxlib_app memcxlib_app.c memcxlib_lib.c

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	${RM} memcxlib_lib memcxlib_app

# Otherwise KERNELRELEASE is defined; we've been invoked from the
# kernel build system and can use its language.
else

	obj-m := memcxlib_mmap.o

endif

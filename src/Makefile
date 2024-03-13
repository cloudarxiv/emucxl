# If called directly from the command line, invoke the kernel build system.
ifeq ($(KERNELRELEASE),)

	KERNEL_SOURCE := /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default: module direct_queue kvs_appl kvs_testcase

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

direct_queue: direct_queue.c emucxl_lib.c emucxl_lib.h  
	gcc -o direct_queue direct_queue.c emucxl_lib.c  
  
kvs_testcase: kvs_testcase.c emucxl_lib.c kvs.c emucxl_lib.h kvs.h  
	gcc -o kvs_testcase kvs_testcase.c emucxl_lib.c kvs.c  
  
kvs_appl: kvs_appl.c emucxl_lib.c kvs.c emucxl_lib.h kvs.h  
	gcc -o kvs_appl kvs_appl.c emucxl_lib.c kvs.c 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	${RM} emucxl_lib direct_queue kvs_appl emucxl_app emucxl_direct kvs_testcase

# Otherwise KERNELRELEASE is defined; we've been invoked from the
# kernel build system and can use its language.
else

	obj-m := emucxl_kernel.o

endif

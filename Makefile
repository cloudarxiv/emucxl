# If called directly from the command line, invoke the kernel build system.
ifeq ($(KERNELRELEASE),)

	KERNEL_SOURCE := /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default: module direct_queue kvs_appl kvs_testcase

module:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd)/src modules

direct_queue: experiments/direct_queue.c src/emucxl_lib.c src/emucxl_lib.h  
	gcc -o direct_queue experiments/direct_queue.c src/emucxl_lib.c  
  
kvs_testcase: experiments/kvs_testcase.c src/emucxl_lib.c src/kvs.c src/emucxl_lib.h src/kvs.h  
	gcc -o kvs_testcase experiments/kvs_testcase.c src/emucxl_lib.c src/kvs.c  
  
kvs_appl: experiments/kvs_appl.c src/emucxl_lib.c src/kvs.c src/emucxl_lib.h src/kvs.h  
	gcc -o kvs_appl experiments/kvs_appl.c src/emucxl_lib.c src/kvs.c 

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
	${RM} emucxl_lib direct_queue kvs_appl emucxl_app emucxl_direct kvs_testcase

# Otherwise KERNELRELEASE is defined; we've been invoked from the
# kernel build system and can use its language.
else

	obj-m := emucxl_kernel.o

endif

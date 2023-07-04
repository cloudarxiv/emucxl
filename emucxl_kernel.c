#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>


/* START: For NUMA NODE*/
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/module.h>
#include <linux/gfp.h>
#include <linux/types.h>

#include <linux/sched.h>
#include <linux/mm.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/io.h>
#include <asm-generic/errno.h>
#include <asm-generic/errno-base.h>
/* END: For NUMA NODE*/


#include "emucxl_kernel.h"

static dev_t dev;
static struct cdev c_dev;
static struct class *cl;

static unsigned char *buffer;
static unsigned char array[10]={0,1,2,3,4,5,6,7,8,9};

/* Open*/
static int my_open(struct inode *i, struct file *f)
{
	return 0;
}

/* Close */
static int my_close(struct inode *i, struct file *f)
{
	return 0;
}

/* Custom MMAP*/
static int my_mmap(struct file *filp, struct vm_area_struct *vma)
{
	phys_addr_t phys_addr;
	pid_t pid;
    unsigned long page;
    unsigned char i;
    unsigned long start = (unsigned long)vma->vm_start;
    unsigned long size = (unsigned long)(vma->vm_end - vma->vm_start);
	unsigned long node = vma->vm_pgoff << PAGE_SHIFT >> PAGE_SHIFT;
	printk(KERN_INFO "node= %lu and size = %lu\n", node, size);
	pid = current->pid;
	buffer = (unsigned char *)kmalloc_node(size, GFP_USER | __GFP_ZERO, node);  // alloc some mem.
	phys_addr = virt_to_phys((void *)buffer);
	if (!phys_addr) {
		pr_alert("Error: Virtual Address 0x%p of PID: %d does not exist... \n", (void*)buffer, pid);
		return -1;
	}
	pr_info("NUMA_NODE: %ld \t Virtual Address: 0x%px \t Physical Address: 0x%llx \n", node, (void*)buffer, phys_addr);
	printk(KERN_INFO "address = 0x%p\n", (void*)buffer);
	SetPageReserved(virt_to_page(buffer));  // set this page as reserved.


    page = virt_to_phys(buffer); // get the physical address from virtual.
    if(remap_pfn_range(vma, start, page>>PAGE_SHIFT, size, PAGE_SHARED))
        return -1;

    for(i=0; i<10; i++)  // write some data
        buffer[i] = array[i] * i;

    return 0;
}

int copy_to_user_fun(unsigned long arg, int ret)
{
	emucxl_arg_t rets;
	rets.return_value = ret;
	if (copy_to_user((emucxl_arg_t *)arg, &rets, sizeof(emucxl_arg_t)))
	{
		return -EACCES;
	}
	return 0;
}
static struct file_operations query_fops;

/* IOCTL*/

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int my_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
	#ifdef DEBUG
		phys_addr_t phys_addr;
		emucxl_lib_t nod;
		int numa_node;
	#endif
	pid_t pid;

	switch (cmd)
	{
		case EMUCXL_INIT:
			pid = task_pid_nr(current);
			pr_info("INIT PID: %d\n", pid);
			if(copy_to_user_fun(arg, 0) != 0)
				return -EACCES;
			break;
		case EMUCXL_EXIT:
			pid = task_pid_nr(current);
			pr_info("EXIT PID: %d\n", pid);
			break;

		case EMUCXL_ALLOC:
			#ifdef DEBUG
				pid = task_pid_nr(current);
				if (copy_from_user(&nod, (emucxl_lib_t *)arg, sizeof(emucxl_lib_t)))
				{
					return -EACCES;
				}
				numa_node = nod.numa_node;
				buffer = (unsigned char *)kmalloc_node(nod.size, GFP_USER | __GFP_ZERO, numa_node);  // alloc some mem.
				phys_addr = virt_to_phys((void *)buffer);
				if (!phys_addr) {
					pr_alert("Error: Virtual Address 0x%p of PID: %d does not exist... \n", (void*)buffer, pid);
					copy_to_user_fun(arg, -1);
					return -1;
				}
				pr_info("NUMA_NODE: %d \t Virtual Address: 0x%px \t Physical Address: 0x%llx \n", numa_node, (void*)buffer, phys_addr);
				printk(KERN_INFO "address = 0x%p\n", (void*)buffer);
				SetPageReserved(virt_to_page(buffer));  // set this page as reserved.
			#endif
			break;

		case EMUCXL_FREE:
				#ifdef DEBUG
					ClearPageReserved(virt_to_page(buffer)); // reset reserved
					kfree(buffer);  // free the memory
				#endif
			break;
		default:
			return -EINVAL;
	}

	return 0;
}


static struct file_operations query_fops =
{
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_close,
    .mmap = my_mmap,
	#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
	.ioctl = my_ioctl
	#else
		.unlocked_ioctl = my_ioctl
	#endif
};

static int __init emucxl_mmap_init(void)
{
	int ret;
	struct device *dev_ret;

	if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "emucxl_ioctl")) < 0)
	{
		return ret;
	}

	cdev_init(&c_dev, &query_fops);

	if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
	{
		return ret;
	}

	if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
	{
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(cl);
	}
	if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "emucxl")))
	{
		class_destroy(cl);
		cdev_del(&c_dev);
		unregister_chrdev_region(dev, MINOR_CNT);
		return PTR_ERR(dev_ret);
	}
	printk(KERN_INFO "emucxl_mmap driver registered");
	return 0;
}

static void __exit emucxl_mmap_exit(void)
{
	device_destroy(cl, dev);
	class_destroy(cl);
	cdev_del(&c_dev);
	unregister_chrdev_region(dev, MINOR_CNT);
	printk(KERN_INFO "emucxl_mmap driver unregistered");
}

module_init(emucxl_mmap_init);
module_exit(emucxl_mmap_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Raja Gond <rajagond@cse.iitb.ac.in>");
MODULE_DESCRIPTION("Basic EMUCXL Library");
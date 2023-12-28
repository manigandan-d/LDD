#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define mem_size 1024

dev_t dev = 0;

static struct class *dev_class;
static struct cdev md_cdev;

uint8_t *kernel_buffer;

static int __init demo_init(void);
static void __exit demo_exit(void);
static int md_open(struct inode *inode, struct file *file);
static int md_release(struct inode *inode, struct file *file);
static ssize_t md_read(struct file *fp, char __user *buf, size_t len, loff_t *off);
static ssize_t md_write(struct file *fp, const char __user *buf, size_t len, loff_t *off);

static struct file_operations fops = {
	.owner = THIS_MODULE, 
	.read = md_read, 
	.write = md_write, 
	.open = md_open, 
	.release = md_release
};

static int md_open(struct inode *inode, struct file *file) {
	pr_info("Driver open function called\n");
	
	return 0;
}

static int md_release(struct inode *inode, struct file *file) {
	pr_info("Driver release function called\n");
	
	return 0;
}

static ssize_t md_read(struct file *fp, char __user *buf, size_t len, loff_t *off) {
	// pr_info("Driver read function called\n");
	
	if(copy_to_user(buf, kernel_buffer, mem_size)) {
		pr_err("Data read error\n");
	}
	
	pr_info("Data read done\n");
	
	return mem_size;
}

static ssize_t md_write(struct file *fp, const char __user *buf, size_t len, loff_t *off) {
	// pr_info("Driver write function called\n");
	
	if(copy_from_user(kernel_buffer, buf, len)) {
		pr_err("Data write error\n");
	}
	
	pr_info("Data write done\n");
	
	return len;
}
 
static int __init demo_init(void)
{
	if(alloc_chrdev_region(&dev, 0, 1, "md_dev") < 0) {
		pr_err("Cannot allocate major and minor number for the device\n");
		
		return -1;
	}
	
    pr_info("Major Number: %d\n", MAJOR(dev));
    pr_info("Minor Number: %d\n", MINOR(dev));
    
    cdev_init(&md_cdev, &fops);
    
    if(cdev_add(&md_cdev, dev, 1) < 0) {
    	pr_err("Cannot add device to the system\n");
    	
    	goto r_class;
    }
    
    dev_class = class_create(THIS_MODULE, "md_class");
    
    if(IS_ERR(dev_class)) {
    	pr_err("Cannot create struct class for device\n");
    	
    	goto r_class;
    }
    
    if(IS_ERR(device_create(dev_class, NULL, dev, NULL, "md_device"))) {
    	pr_err("Cannot create the device\n");
    	
    	goto r_device;
    }
    
    kernel_buffer = kmalloc(mem_size, GFP_KERNEL);
    
    if(kernel_buffer == 0) {
    	pr_info("Cannot allocate memory in kernel\n");
    	
    	goto r_device;
    }
    
    strcpy(kernel_buffer, "Hello Mani");
    
    pr_info("Module Inserted Successfully\n");
    
    return 0;
    
r_device: 
	class_destroy(dev_class);

r_class:
	unregister_chrdev_region(dev, 1);
	return -1;
	
}

static void __exit demo_exit(void)
{
	kfree(kernel_buffer);
	device_destroy(dev_class, dev);
	class_destroy(dev_class);
	cdev_del(&md_cdev);
	unregister_chrdev_region(dev, 1);
	
    pr_info("Module Removed Successfully\n");
}
 
module_init(demo_init);
module_exit(demo_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MD");
MODULE_DESCRIPTION("Demo");
MODULE_VERSION("1.0");

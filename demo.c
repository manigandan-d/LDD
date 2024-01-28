#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
 
static int __init demo_init(void) {
    pr_info("Module Inserted Successfully\n");
    
    return 0;	
}

static void __exit demo_exit(void) {
    pr_info("Module Removed Successfully\n");
}
 
module_init(demo_init);
module_exit(demo_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("MD");
MODULE_DESCRIPTION("Demo");
MODULE_VERSION("1.0");

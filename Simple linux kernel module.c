#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/timekeeping.h>
MODULE_LICENSE("Dual BSD/GPL");

u64 module_start_time, module_end_time;
ktime_t k_start_time, k_end_time;
static int hello_init(void)
{
	printk(KERN_ALERT "Hello, world\n");

	long tick = (1000000 + HZ/2) / HZ;
	printk(KERN_INFO "tick time: %ld milliseconds\n", tick/1000);
	module_start_time = jiffies;
	k_start_time = ktime_get_boottime();
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "Goodbye, cruel world\n"); 
	
	//method a:
	module_end_time = jiffies;
	printk(KERN_INFO "a: Module Time: %llu seconds\n", (module_end_time - module_start_time)/HZ);

	//method b:
	k_end_time = ktime_get_boottime();
	printk(KERN_INFO "b: Module Time: %llu seconds\n", (k_end_time - k_start_time)/1000000000);
}

module_init(hello_init);
module_exit(hello_exit);

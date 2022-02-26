#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/rtc.h>
#include <linux/timekeeping.h>
#include <linux/time64.h>
#include <linux/uaccess.h>

//open function
static int my_misc_open(struct inode *inode, struct file *file)
{
	pr_info("My misc device is open\n");
	return 0;
}

//close function
static int my_misc_close(struct inode *inodep, struct file *filp)
{
	pr_info("My misc device is closed\n");
	return 0;
}

//read function
static ssize_t my_misc_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	pr_info("My misc device is read\n");

	struct rtc_time tt;
	ktime_t kt;
	kt = ktime_get_real();
	tt = rtc_ktime_to_tm(kt);

	char readbuf[80];
	int day;
	int hour;
	if (tt.tm_hour < 5){
		day = tt.tm_mday - 1;
		hour = 24 - (5 - tt.tm_hour);
	}
	else {
		day = tt.tm_mday;
		hour = tt.tm_hour - 5;
	}
	sprintf(readbuf, "Hello world, the current date is %d, %d %d. The current time is %d:%d:%d\n\0", tt.tm_mon+1, day, tt.tm_year+1900, hour, tt.tm_min, tt.tm_sec);

	return simple_read_from_buffer(buf, count, f_pos, readbuf, 80);
}

//file operation structure
static const struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = my_misc_open,
	.read = my_misc_read,
	.release = my_misc_close,
	.llseek = no_llseek,
};

//Misc device structure
struct miscdevice my_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "lab3",
	.fops = &fops,
};

//Misc init function
static int __init misc_init(void)
{
	int error;
	error = misc_register(&my_misc_device);
	if 	(error) {
		pr_err("misc_register failed!\n");
		return error;
	}

	pr_info("misc_register init done!\n");
	return 0;
}

//Misc exti function	
static void __exit misc_exit(void)
{
	misc_deregister(&my_misc_device);
	pr_info("misc_register exit done!\n");
}

module_init(misc_init)
module_exit(misc_exit)

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple misc device driver that returns current time.");
MODULE_AUTHOR("Melody Qu");

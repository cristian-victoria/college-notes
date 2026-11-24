#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/ktime.h>
#include <linux/time.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A kernel module that tracks time using ktime_get_real_ts64()");
MODULE_VERSION("1.0");

#define PROC_NAME "timer"

// Global variables to store time information
static struct timespec64 curr_time;
static struct timespec64 prev_time;
static int first_call = 1;  // Flag to track if this is the first call

// proc_read - Called when /proc/timer is read
// This function is called each time the /proc/timer file is read.
// It prints the current time and elapsed time since the last call.

static int proc_show(struct seq_file *m, void *v)
{
    long elapsed_sec = 0;
    long elapsed_nsec = 0;
    
    // Get the current time
    ktime_get_real_ts64(&curr_time);
    
    // Print current time (seconds.nanoseconds since Unix Epoch)
    seq_printf(m, "current time: %lld.%09ld", 
               (long long)curr_time.tv_sec, 
               curr_time.tv_nsec);
    
    // Calculate and print elapsed time if this is not the first call
    if (!first_call) {
        elapsed_sec = curr_time.tv_sec - prev_time.tv_sec;
        elapsed_nsec = curr_time.tv_nsec - prev_time.tv_nsec;
        
        // Handle nanosecond underflow
        if (elapsed_nsec < 0) {
            elapsed_sec--;
            elapsed_nsec += 1000000000L;
        }
        
        seq_printf(m, " elapsed time: %ld.%09ld", elapsed_sec, elapsed_nsec);
    }
    
    seq_printf(m, "\n");
    
    // Update prev_time for next call
    prev_time = curr_time;
    first_call = 0;
    
    return 0;
}

// Called when /proc/timer is opened
static int proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, proc_show, NULL);
}

// File operations structure for /proc/timer
static const struct proc_ops timer_proc_ops = {
    .proc_open = proc_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

// Module initialization function
// Called when the module is loaded with insmod
// Creates the /proc/timer entry
static int __init my_timer_init(void)
{
    // Create /proc/timer entry
    struct proc_dir_entry *proc_entry;
    
    proc_entry = proc_create(PROC_NAME, 0666, NULL, &timer_proc_ops);
    
    if (proc_entry == NULL) {
        printk(KERN_ERR "my_timer: Error creating /proc/%s entry\n", PROC_NAME);
        return -ENOMEM;
    }
    
    printk(KERN_INFO "my_timer: /proc/%s created\n", PROC_NAME);
    
    // Initialize first_call flag
    first_call = 1;
    
    return 0;
}

// Module cleanup function
// Called when the module is unloaded with rmmod
// Removes the /proc/timer entry

static void __exit my_timer_exit(void)
{
    // Remove /proc/timer entry
    remove_proc_entry(PROC_NAME, NULL);
    
    printk(KERN_INFO "my_timer: /proc/%s removed\n", PROC_NAME);
}

// Register module entry and exit points
module_init(my_timer_init);
module_exit(my_timer_exit);
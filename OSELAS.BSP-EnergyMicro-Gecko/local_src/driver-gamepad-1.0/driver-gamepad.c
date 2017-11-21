/*
 * This is a demo Linux kernel module.
 */


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/ioport.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/interrupt.h>
#include <asm/siginfo.h>
#include <linux/signal.h>

#include "efm32gg.h"

#define GPIO_EVEN_IRQ_LINE 17
#define GPIO_ODD_IRQ_LINE 18
#define DRIVER_NAME "driver-gamepad"

/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static int driver_fasync(int, struct file*, int);
static ssize_t driver_read(struct file*, char* __user, size_t, loff_t*);
static int driver_open(struct inode*, struct file*);
static int driver_release(struct inode*, struct file*);

struct file_operations my_fops = {
.owner = THIS_MODULE,
.read = driver_read,
.open = driver_open,
.release = driver_release,
.fasync = driver_fasync,
};


static dev_t dev;
struct cdev my_cdev;
struct class *my_class;
struct fasync_struct* async_queue;

static int Button_pressed;

static int driver_fasync(int fd, struct file* filp, int mode) {
    return fasync_helper(fd, filp, mode, &async_queue);
}

static ssize_t driver_read(struct file *filp, char __user *buff, size_t count, loff_t *offp){
//    printk(KERN_INFO "read driver\n");
    copy_to_user(buff, &Button_pressed, 1);
    return 1;
};

static int driver_open(struct inode* inode, struct file* filp) {
    printk(KERN_INFO "Open driver\n");
    return 0;
}


static int driver_release(struct inode* inode, struct file* filp) {
    printk(KERN_INFO "Release driver\n");
    return 0;
}

//Interupt handler
irqreturn_t irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
    //printk(KERN_ALERT "Handling GPIO interrupt\n");
    Button_pressed = *GPIO_IF;
    iowrite32(ioread32(GPIO_IF), GPIO_IFC);

    if(async_queue){
	kill_fasync(&async_queue, SIGIO, POLL_IN);
    }
    return IRQ_HANDLED;
}


//Startup/Initalize function
static int __init template_init(void)
{

    if(0>alloc_chrdev_region(&dev, 0, 1, DRIVER_NAME)){
	printk("Error making major and minor numbers\n");
    };

    if(0>cdev_add(&my_cdev, dev, 1)){
	printk("Error adding device to kernel\n");
    }

    cdev_init(&my_cdev, &my_fops);
    my_class = class_create(THIS_MODULE, DRIVER_NAME);

    if(device_create(my_class, NULL, dev, NULL, DRIVER_NAME) == NULL ){
        printk(KERN_ALERT "Error creating device\n");
        return -1;
    }

    //Request access to I/O ports
    if (request_mem_region(GPIO_PC_MODEL, 1, DRIVER_NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting GPIO_PC_MODEL memory region, already in use?\n");
        return -1;
    }
    if (request_mem_region(GPIO_PC_DOUT, 1, DRIVER_NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting GPIO_PC_DOUT memory region, already in use?\n");
        return -1;
    }
    if (request_mem_region(GPIO_PC_DIN, 1, DRIVER_NAME) == NULL ) {
        printk(KERN_ALERT "Error requesting GPIO_PC_DIN memory region, already in use?\n");
        return -1;
    }

    //Setup I/O
    iowrite32(0x333333333, (void*)GPIO_PC_MODEL);
    iowrite32(0xFF, (void*)GPIO_PC_DOUT);

    //Request access to interupt ports
    if(request_mem_region((int)GPIO_EXTIPSELL, 1, DRIVER_NAME)==NULL){
	printk(KERN_ALERT "Error accessing *GPIO_EXTIPSELL");
    }

    if(request_mem_region((int)GPIO_EXTIFALL, 1, DRIVER_NAME)==NULL){
	printk(KERN_ALERT "Error accessing *EXTIFALL");
    }

    if(request_mem_region((int)GPIO_IEN, 1, DRIVER_NAME)==NULL){
	printk(KERN_ALERT "Error accessing *GPIO_IEN");
    } 

    if(request_mem_region((int)GPIO_IFC, 1, DRIVER_NAME)==NULL){
	printk(KERN_ALERT "Error accessing *GPIO_IFC");
    }


    //Setup interupts
    iowrite32(0x22222222, GPIO_EXTIPSELL);
    iowrite32(0xFF, GPIO_EXTIFALL);
    iowrite32(0xFF, GPIO_IEN);

    //add interupt ??
    request_irq(GPIO_EVEN_IRQ_LINE, (irq_handler_t)irq_handler, 0, DRIVER_NAME, &my_cdev);
    request_irq(GPIO_ODD_IRQ_LINE, (irq_handler_t)irq_handler, 0, DRIVER_NAME, &my_cdev);


   printk("Gamepad driver started\n");
   return 0;
}

/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit template_cleanup(void)
{
    release_mem_region((int)GPIO_PC_MODEL, 1);
    release_mem_region((int)GPIO_PC_DOUT, 1);
    release_mem_region((int)GPIO_PC_DIN, 1);

    release_mem_region((int)GPIO_EXTIPSELL, 1);
    release_mem_region((int)GPIO_EXTIFALL, 1);
    release_mem_region((int)GPIO_IEN, 1);
    release_mem_region((int)GPIO_IFC, 1);

    cdev_del(&my_cdev);
    device_destroy(my_class, dev);
    class_destroy(my_class);

    free_irq(GPIO_EVEN_IRQ_LINE, &my_cdev);
    free_irq(GPIO_ODD_IRQ_LINE, &my_cdev);

    printk("Short life for a small module...\n");
}



module_init(template_init);
module_exit(template_cleanup);

MODULE_DESCRIPTION("Small module, demo only, not very useful.");
MODULE_LICENSE("GPL");

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include <linux/fs.h> //For file ops
#include <linux/slab.h> //kmalloc
#include <linux/errno.h> //For error codes
//Referenced from https://www.gnu.org/software/libc/manual/html_node/Error-Codes.html

#define DEVICE_NAME "simple_character_device"
#define MAJOR_NUM 511
#define BUFFER_SIZE 900

//kernel memory buffer for device
static char *deviceBuffer;
static int openCount;
static int closeCount;

//checks if the buffer is allocated and increments count if it is
static int char_driver_open(struct inode *inode, struct file *filep)
{
    //Checking if the device buffer is properly allocated and returning
    if(!deviceBuffer){
        printk(KERN_ERR "%s:Buffer didn't allocate properly\n",DEVICE_NAME);
        //using error codes ENODEV specifically stands for "No such device"
        return -ENODEV;
    }
    //log openCount and returns success
    openCount++;
    printk(KERN_INFO "%s:Opened %d time(s)\n",DEVICE_NAME,openCount);
    return 0;
}

//used when the device is closed in order to increment count
static int char_driver_release(struct inode *inode, struct file *filep)
{
    closeCount++;
    printk(KERN_INFO "%s:Closed %d time(s)\n",DEVICE_NAME,closeCount);
    return 0;
}

//reading from device buffer to user space
static ssize_t char_driver_read(struct file *filep,char __user *userBuf,size_t count,loff_t *filePos){
    ssize_t bytesToCopy;

    //Checking for valid userspace buffer
    if(!userBuf){
        printk(KERN_ERR "%s:Usespace buffer is messed up\n",DEVICE_NAME);
        return -EINVAL; //return code for invalid argument
    }

    //checking position to be within the buffer
    if(*filePos<0||*filePos>BUFFER_SIZE){
        printk(KERN_WARNING "%s:Reading at %lld is out of scope\n",DEVICE_NAME,*filePos);
        //I had trouble with the clamp_t function, so I used https://en.cppreference.com/w/cpp/algorithm/clamp to reference how to use it properly
        *filePos=clamp_t(loff_t,*filePos,0,BUFFER_SIZE);
    }

    //End of file condition
    if(*filePos == BUFFER_SIZE){
        return 0;
    }
    //calculating how many bytes can be read based on the state of the buffer
    bytesToCopy = min(count,(size_t)(BUFFER_SIZE-*filePos));
    if(bytesToCopy==0){
        return 0;
    }
    //Transfer from kernel space to user
    if(copy_to_user(userBuf,deviceBuffer+*filePos,bytesToCopy)){
        printk(KERN_ERR "%s:Copy failed\n",DEVICE_NAME);
        return -EFAULT; //Invalid pointer error which would only run if wrong memory address is trying to be accessed
    }

    //Updating positions and returning the number of bytes read
    *filePos+=bytesToCopy;
    printk(KERN_INFO "%s: Read %zd bytes,position at %lld.\n",DEVICE_NAME,bytesToCopy,*filePos);
    return bytesToCopy;
}

//writing to device buffer from user space
static ssize_t char_driver_write(struct file *filep,const char __user *userBuf,size_t count,loff_t *filePos){
    ssize_t bytesToCopy;
    //valid buffer check
    if(!userBuf){
        printk(KERN_ERR "%s:Buffer is NULL\n",DEVICE_NAME);
        return -EINVAL;
    }

    //clamping position to be within the buffer
    if(*filePos<0||*filePos>BUFFER_SIZE){
        printk(KERN_WARNING "%s:Writing at %lld is out of scope\n",DEVICE_NAME,*filePos);
        *filePos = clamp_t(loff_t, *filePos, 0, BUFFER_SIZE);
    }

    //End conditional
    if(*filePos==BUFFER_SIZE){
        printk(KERN_ERR "%s:No more space, currently at %lld).\n",DEVICE_NAME,*filePos);
        return -ENOSPC;  //No space error
    }

    //Ensuring writing to only to available space
    bytesToCopy=min(count,(size_t)(BUFFER_SIZE-*filePos));
    if(bytesToCopy==0){
        return 0;
    }
    //Copy from user to kernel space and returning errors if they occur
    if(copy_from_user(deviceBuffer+*filePos,userBuf,bytesToCopy)){
        printk(KERN_ERR "%s:Writing to Kernel failed\n",DEVICE_NAME);
        return -EFAULT;
    }
    //updating trackers
    *filePos+=bytesToCopy;
    printk(KERN_INFO "%s:Wrote %zd bytes, now at %lld.\n",DEVICE_NAME,bytesToCopy,*filePos);
    return bytesToCopy;
}

//changes file positions
static loff_t char_driver_llseek(struct file *filep,loff_t offset,int whence)
{
    loff_t newPos;
    //Whence values for each seek type
    switch(whence){
    case SEEK_SET:
        newPos=offset;
        break;
    case SEEK_CUR:
        newPos=filep->f_pos+offset;
        break;
    case SEEK_END:
        newPos=BUFFER_SIZE+offset;
        break;
    default:
    //invalid input handling
        printk(KERN_ERR "%s:Invalid Whence value%d.\n",DEVICE_NAME,whence);
        return -EINVAL;
    }

    //new position clamping within the buffer
    if(newPos<0){
        printk(KERN_WARNING "%s:Seeked before Start, set to 0\n",DEVICE_NAME);
        newPos=0;
    }
    if(newPos>BUFFER_SIZE){
        printk(KERN_WARNING "%s:Seeked past End, at%d.\n",DEVICE_NAME,BUFFER_SIZE);
        newPos = BUFFER_SIZE;
    }
    //Updating file position
    filep->f_pos=newPos;
    printk(KERN_INFO "%s:File position at %lld.\n",DEVICE_NAME,newPos);
    return newPos;
}

//linking file operations for functions, since close doesn't exist used release
static struct file_operations char_driver_fops={
    .owner   = THIS_MODULE,
    .open    = char_driver_open,
    .release = char_driver_release,
    .read    = char_driver_read,
    .write   = char_driver_write,
    .llseek  = char_driver_llseek,
};

//Module initialization
static int char_driver_init(void){
    int ret;

    //Allocating the kernel buffer
    //Used Linux kernel documentation for kmalloc to understand memory allocation
    //https://www.kernel.org/doc/html/v5.0/core-api/memory-allocation.html
    deviceBuffer=kmalloc(BUFFER_SIZE,GFP_KERNEL);
    if(!deviceBuffer){
        printk(KERN_ERR "%s:Allocation failed w/ %d bytes.\n",DEVICE_NAME, BUFFER_SIZE);
        //Memory allocation error
        return -ENOMEM;
    }
    //Zeroing the buffer
    memset(deviceBuffer,0,BUFFER_SIZE);

    //registering char device for file ops
    ret=register_chrdev(MAJOR_NUM,DEVICE_NAME,&char_driver_fops);
    if(ret<0){
        printk(KERN_ERR "%s:register_chrdev returned %d.\n",DEVICE_NAME,ret);
        kfree(deviceBuffer);
        return ret;
    }

    printk(KERN_INFO "%s:Module loaded with major %d and bufferSize of %d).\n",DEVICE_NAME,MAJOR_NUM,BUFFER_SIZE);
    return 0;
}

//unregisters the device and frees the buffer
static void char_driver_exit(void){
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    if(deviceBuffer){
        kfree(deviceBuffer);
        deviceBuffer=NULL;
    }
    printk(KERN_INFO "%s:Unregistered and Resources freed\n",DEVICE_NAME);
}

//init and exit functions called when the module is loaded and unloaded
module_init(char_driver_init);
module_exit(char_driver_exit);


//sources:
	//https://sites.google.com/site/linuxkernel99/sample-code/writing-a-character-driver

#include<linux/init.h> //required for module initialization
#include<linux/module.h> //tells kernel this is an LKM

#include<linux/fs.h> // use to make counter 852 or 916
#include<linux/slab.h>
#include<linux/uaccess.h>

#define BUFFER_SIZE 1024
char *device_buffer;
int counto = 0; // global open counter
int countc = 0; // global close counter
/* Define device_buffer and other global data structures you will need here */

loff_t simple_char_driver_seek (struct file *pfile, loff_t offset, int whence)
{
	/* Update open file position according to the values of offset and whence */
	loff_t currentposition = pfile->f_pos; // the current position is held in struct file
	if(whence == 0){ // set current position to offset
	 	currentposition = offset;
	}
	else if(whence == 1){ // set current position to be +offset
		currentposition += offset; 
	}
	else if(whence == 2){ // current position is offset bytes from end of file
		currentposition = BUFFER_SIZE - offset;
	}
	if(currentposition < 0 || currentposition > BUFFER_SIZE){ // current position out of bounds
		printk(KERN_ALERT "invalid seek. current position unchanged.");
		return -ESPIPE; // error return message
	}
	pfile->f_pos = currentposition; // update struct file
	return pfile->f_pos;
}

ssize_t simple_char_driver_read (struct file *pfile, char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer to where you are writing the data you want to be read from the device file*/
	/* length is the length of the userspace buffer*/
	/* offset will be set to current position of the opened file after read*/
	/* copy_to_user function: source is device_buffer and destination is the userspace buffer *buffer */
	size_t bytes;
	int bytes_left = BUFFER_SIZE - *offset; //how many bytes are left from current position in device_buffer
	if(bytes_left <= 0){ //no space left in device file -> at end of file
		printk(KERN_ALERT "invalid read. no bytes read.");
		return 0;
	}
	if(bytes_left > length){ // choose the smaller of the bytes left in the file and the requested read size
		bytes = length;
	}
	else{
		bytes = bytes_left;
	}
	copy_to_user(buffer, device_buffer + *offset, bytes); // move data to user space buffer from device file
	*offset += bytes; // update current position
	printk(KERN_ALERT "bytes read in the file: %zd\n", bytes);
	return bytes;
}



ssize_t simple_char_driver_write (struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	/* *buffer is the userspace buffer where you are writing the data you want to be written in the device file*/
	/* length is the length of the userspace buffer*/
	/* current position of the opened file*/
	/* copy_from_user function: destination is device_buffer and source is the userspace buffer *buffer */
	ssize_t bytes;
	loff_t bytes_left = BUFFER_SIZE - *offset; // how many bytes are left in device file from the current position
	if(bytes_left <= 0){ //end of device file or beyond - cannot write
		printk(KERN_ALERT "invalid write. no bytes written.");
		return -ENOSPC; //error
	}
	if(bytes_left > length){ // choose the smaller of the bytes left in the file and the number of bytes the user wants to write
		bytes = length;
	}
	else{
		bytes = bytes_left;
	}
	copy_from_user(device_buffer + *offset, buffer, bytes); // move data from user buffer to device file
	*offset += bytes; // update current position
	printk(KERN_ALERT "bytes written in the file: %zd\n", bytes);
	return bytes;
}


int simple_char_driver_open (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is opened and also print the number of times this device has been opened until now*/
	counto++; //number of times opened
	printk(KERN_ALERT "the device is open.\n");
	printk(KERN_ALERT "it has been opened %d times until now.\n",counto);
	return 0;
}

int simple_char_driver_close (struct inode *pinode, struct file *pfile)
{
	/* print to the log file that the device is closed and also print the number of times this device has been closed until now*/
	countc++; // number of times closed
	printk(KERN_ALERT "the device is closed.\n");	
	printk(KERN_ALERT "it has been closed %d times until now.\n",countc);
	return 0;
}



struct file_operations simple_char_driver_file_operations = {

	.owner   = THIS_MODULE,
	.open    = simple_char_driver_open,
	.release = simple_char_driver_close,
	.read    = simple_char_driver_read,
	.write   = simple_char_driver_write,
	.llseek  = simple_char_driver_seek,
	/* add the function pointers to point to the corresponding file operations. look at the file fs.h in the linux souce code*/
};


static int simple_char_driver_init(void)
{
	/* print to the log file that the init function is called.*/
	register_chrdev(240, "simple_char_device", &simple_char_driver_file_operations);
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	device_buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	memset(device_buffer, 0, sizeof(BUFFER_SIZE)); // set all device file bytes to 0
	/* register the device */
	return 0;
}

static void simple_char_driver_exit(void)
{
	/* print to the log file that the exit function is called.*/
	unregister_chrdev(240, "simple_char_device");
	printk(KERN_ALERT "inside %s function\n",__FUNCTION__);
	kfree(device_buffer);
	/* unregister  the device using the register_chrdev() function. */
}



module_init(simple_char_driver_init);
module_exit(simple_char_driver_exit);

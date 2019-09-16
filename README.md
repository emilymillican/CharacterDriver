# Programming Assignment Two - Operating Systems

Created a loadable kernel module to install a simple character device driver. Also created a device file  in the /dev folder to work with the character device driver. Then, used an interactive testing program to allow a user to read from and write to the device file.

## File Descriptions

* **simple_char_driver.c** - *Device Driver Module*  - Gives instructions for the device driver's initialization and exit as well as response to various system calls - namely open, close, read, write, and seek.  

* **Makefile** - Builds the device driver LKM using obj-m to represent that the object being built is a module.

* **char_driver_test.c** - *Interactive Test Program* - Makes system calls, based on user input, to the device driver module, which then interacts with the device file in a corresponding manner.


## Instructions for Building and Running Test Function
* Navigate to the directory in which the char_driver_test.c function is located.
* First, a device file corresponding to the device driver must be created using the command: **sudo mknod -m 777 /dev/simple_char_device c 240 0**
* Then, the device driver module must be compiled using the command: **sudo make -C /lib/modules/$(uname -r)/build M=$PWD modules**
* Once made, the module can be inserted in the kernel with the command: **sudo insmod simple_char_driver.ko**
* Once the LKM is appropriately set up, in terminal the test function can be built with the command: **gcc char_driver_test.c -o test**
* The function can then be run with the command: **./test**  and interacted with using the commands given by the print statements.
* Once finished with the device driver, it can be removed with the command : **sudo rmmood simple_char_driver.ko**.
* To view kernel output, the command **dmesg** can be used.


### Emily Millican - Emily.Millican@Colorado.Edu

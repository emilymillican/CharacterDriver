#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
//sources: 
	//https://www.geeksforgeeks.org/input-output-system-calls-c-create-open-close-read-write/
	//man7.org/linux/man-pages/man3/strcpy3.html
	//https://stackoverflow.com/questions/4729395/error-free-invalid-next-size-fast
	//man7.org/linux/man-pages/man2/lseek.2.html


#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void test_function()
{
	char choice;
	int max = 1024;
	size_t bytes;
	char line[100];
	char *buffer;
	const char *Path = "/dev/simple_char_device"; //character device file
	int fd = open(Path, O_RDWR); //open device file
	while(1){
		printf("What would you like to do? \n(r)ead from the device, \n(w)rite to the device, \n(s)eek into the device, \nor (e)xit from the device? \nPress anything else to keep reading or writing from the device. \n");
		if(fgets(line, 2, stdin) != NULL)
		{
			choice = line[0]; // only take in first character of input
		}
		if(choice == 'r') //read 
		{
			printf("Enter the number of bytes you want to read:\n");
			scanf("%zd", &bytes);
			if(bytes > max){ //limit bytes value in case of negative
				bytes = max;
			}
			buffer = malloc(bytes);
			size_t bytes_read = read(fd, buffer, bytes); // read device file from current position
			if(bytes_read == 0 && bytes != 0){ // returns 0 if nothing is read - at end of file
				printf("Invalid read. Current position is the end of file.\n");
			}
			else{
				printf("Data read from the device: %s \n", buffer);
			}
			free(buffer);
			char c = fgetc(stdin); // clear stdin buffer

		}
		else if(choice == 'w') //write
		{
			printf("Enter the data you want to write to device:\n");
			bytes = 0;
			char c;
			c = fgetc(stdin); // clear stdin buffer
			c = fgetc(stdin); // clear stdin buffer
			while(c != '\n') // to take in input & include spaces
			{
				line[bytes] =c;
				bytes++;
				c = fgetc(stdin);
			}
			buffer = malloc(bytes);
			strncpy(buffer, line, bytes); // copy from line to buffer
			ssize_t bytes_written = write(fd, buffer, bytes);
			if(bytes_written < 0){ // returns the number of bytes written to the file
				printf("Invalid write. Device is full.\n");
			}
			free(buffer);
		}
		else if(choice == 's') // seek
		{
			off_t offset;
			int whence;
			printf("Enter an offset value:\n");
			scanf("%jd", &offset);
			printf("Enter a value for whence (third parameter):\n");
			scanf("%d", &whence);
			const char *Path = "/dev/simple_char_device";
			off_t seek = lseek(fd, offset, whence);
			if(seek < 0) // seek returns new offset
			{
				printf("Invalid seek. Current position in file unchanged.\n");
			}
			char c = fgetc(stdin);

		}
		else if(choice == 'e') //exit
		{
			return;
		}
	}
	close(fd); // close device file
	return;
}

int main()
{
	test_function();
	return 0;
}
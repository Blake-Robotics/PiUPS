#include <unistd.h>				//Needed for I2C port
#include <fcntl.h>				//Needed for I2C port
#include <sys/ioctl.h>			//Needed for I2C port
#include <linux/i2c-dev.h>		//Needed for I2C port
#include <stdint.h>
#include <stdio.h>


int write_i2c_reg(int file, uint8_t reg, uint16_t data);
int read_i2c_reg(int file_i2c, uint8_t reg, uint16_t* data);

int main (void)
{
	int file_i2c;
	int length;
	unsigned char buffer[60] = {0};

	
	//----- OPEN THE I2C BUS -----
	char *filename = (char*)"/dev/i2c-1";
	if ((file_i2c = open(filename, O_RDWR)) < 0)
	{
		//ERROR HANDLING: you can check errno to see what went wrong
		printf("Failed to open the i2c bus");
		return -1;
	}
	
	int addr = 0x13;          //<<<<<The I2C address of the slave
	if (ioctl(file_i2c, I2C_SLAVE, addr) < 0)
	{
		printf("Failed to acquire bus access and/or talk to slave.\n");
		//ERROR HANDLING; you can check errno to see what went wrong
		return -2;
	}
	
	//----- WRITE BYTES -----
	buffer[0] = 0x20;
        length = 1;                     //<<< Number of bytes to write
	if (write(file_i2c, buffer, length) != length)          //write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write to the i2c bus.\n");
	}
	
	usleep (10000);

	//----- READ BYTES -----
	length = 2;			//<<< Number of bytes to read
	if (read(file_i2c, buffer, length) != length)		//read() returns the number of bytes actually read, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		//ERROR HANDLING: i2c transaction failed
		printf("Failed to read from the i1c bus.\n");
	}
	else
	{
		printf("Data read: 0x");
		for (int i=0; i< length; i++) printf ("%02x", buffer[i]);
		printf("\n");
	}

	usleep (1000);	

	//----- WRITE BYTES -----
	/*
 	buffer[0] = 0x01;
	buffer[1] = 0x02;
	length = 2;			//<<< Number of bytes to write
	if (write(file_i2c, buffer, length) != length)		//write() returns the number of bytes actually written, if it doesn't match then an error occurred (e.g. no response from the device)
	{
		// ERROR HANDLING: i2c transaction failed
		printf("Failed to write to the i2c bus.\n");
	}*/
	
	uint16_t vcc;
	if (read_i2c_reg(file_i2c, 0x20, &vcc) == 0)
	{
	  printf("Read VCC as %d mV. \n", vcc);
	}
	else
	{
	  printf("Failed to read VCC \n");
	}
	
  uint16_t vbat;
	if (read_i2c_reg(file_i2c, 0x22, &vbat) == 0)
	{
	  printf("Read VCC as %d mV. \n", vbat);
	}
	
	usleep (1000);	
	// Write and Read Vbat conv:
	uint16_t vbat_conv_set = 4880, vbat_conv_read;
	
	if (write_i2c_reg(file_i2c, 0x50, vbat_conv_set) == 0)
	{
	  printf("Set vbat_conv to %i. \n", vbat_conv_set);
	}
	
	usleep (1000);	
	if (read_i2c_reg(file_i2c, 0x50, &vbat_conv_read) == 0)
	{
	  printf("Read vbat_conv as %i. \n", vbat_conv_read);
	}
	
	
	
	
	
	return 0;
}

//Write to a specific I2C register:
int write_i2c_reg(int file_i2c, uint8_t reg, uint16_t data)
{
  // Setup data to write:
  uint8_t writebuffer[3];
  writebuffer[0] = reg;
  writebuffer[1] = (data >> 8);
  writebuffer[2] = data;
  
  // Write the data, and check that the bytes were really written
  if (write(file_i2c, writebuffer, 3) != 3)      
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write to the i2c bus.\n");
		return -1;
	}
	else
	{
	  return 0;
	}
}


int read_i2c_reg(int file_i2c, uint8_t reg, uint16_t* data)
{
  // Setup data to write:
  uint8_t writebuffer[1];
  uint8_t readbuffer[2];
  
  // Write the register to read from:
  writebuffer[0] = reg;
  
  // Write the data, and check that the bytes were really written
  if (write(file_i2c, writebuffer, 1) != 1)      
	{
		/* ERROR HANDLING: i2c transaction failed */
		printf("Failed to write to the i2c bus.\n");
		return -1;
	}
	else
	{
	  // Sleep and do a read of the register:
	  usleep (1000);
	  
	  if (read(file_i2c, readbuffer, 2) != 2)		
	  {
		  //ERROR HANDLING: i2c transaction failed
		  printf("Failed to read from the i1c bus.\n");
		  return -2;
	  }
	  else
	  {
	    *data = ((uint16_t)readbuffer[0] << 8) + readbuffer[1];
	    return 0;
	  }
	}
}

// Copyright(c) 2019 Analog Devices, Inc.
// All Rights Reserved.
// This software is proprietary to Analog Devices, Inc. and its licensors.

#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <err.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include "adm1266_pmbus_interface.h"
#include "adi_pmbus.h"

int file[2];

void i2c_init()
{
    // The digital board sequencer 0x4F is on i2c-1 bus
    // The power board sequencer 0x4E is on i2c-2 bus
    const char *path_dig_seq_bus = "/dev/i2c-1";
    const char *path_pwr_seq_bus = "/dev/i2c-2";
    int rc;
    
    // Below is a temporary fix to allow communicating with devices on 2 
    // different buses in the same program

    file[0] = open(path_dig_seq_bus, O_RDWR);
	if (file[0] < 0)
    {
		err(errno, "Tried to open '%s'", path_dig_seq_bus);
    }

    file[1] = open(path_pwr_seq_bus, O_RDWR);
	if (file[1] < 0)
    {
		err(errno, "Tried to open '%s'", path_pwr_seq_bus);
    }
}

__u32 i2c_block_write(__u8 device_addr,__u8 dataout_length, __u8 *dataout)
{
    __u32 bytes_written = 0;
    __u8 command;
    __u8 length;
    __u8 i;    
    __u8 datawrite[255];


    //set_i2c_addr(device_addr);

    command = dataout[0];
    length = dataout_length - 1;


    for (i = 1; i <= dataout_length; i++)
    {
        datawrite[i-1] = dataout[i];   
    } 

    // If ioctrl system call in Linux is not used for communicating with i2c devices 
    // i2c_smbus_block_write_big should be replaced with i2c master api used in the system

    // Parameters passed to i2c_smbus_block_write_big function
    //---------------------------------------------------------------------------
    // file is a pointer for the i2c master in a system
    // device_addr is the i2c address of the slave device
    // command is the register address for which the data will be sent
    // length is the number of bytes of data which will be sent to the i2c slave device
    // datawrite is the data which will be written to the i2c slave device

    if (device_addr == 0x4F){           // 0x4F is on i2c-1 bus
        bytes_written = i2c_smbus_block_write_big(file[0], device_addr, command, length, datawrite);
    } else if (device_addr == 0x4E) {   // 0x4E is on i2c-2 bus
        bytes_written = i2c_smbus_block_write_big(file[1], device_addr, command, length, datawrite);
    } else {
        return bytes_written; // No bytes written
    }
    
 
   return bytes_written;
}


__u32 i2c_block_write_block_read(__u8 device_addr, __u8 dataout_length, __u8 *dataout, __u8 read_no_bytes, __u8 *datain)
{
	
    __u8 write_length = 0;
	__u32 bytes_read = 0;
	__u8 actual_data_written = 0;
	__u8 actual_data_read = 0;
    __u8 command;
    __u8 length;
    __u8 i; 
    __u8 datawrite[255];  

    command = dataout[0];
    length = dataout_length - 1;
    for (i = 1; i <= dataout_length; i++)
    {
        datawrite[i-1] = dataout[i];   
    }


    // If ioctrl system call in Linux is not used for communicating with i2c devices 
    // i2c_smbus_block_write_block_read should be replaced with i2c master api used in the system

    // Parameters passed to i2c_smbus_block_write_block_read function
    //---------------------------------------------------------------------------
    // file is a pointer for the i2c master in a system
    // device_addr is the i2c address of the slave device
    // command is the register address for which the data will be sent
    // length is the number of bytes of data which will be sent to the i2c slave device
    // datawrite is the data which will be written to the i2c slave device
    // read_no_bytes is the number of bytes of data which will be read back from the i2c slave device
    // datain is the data which is read back from the i2c slave device

	if (device_addr == 0x4F){           // 0x4F is on i2c-1 bus
        bytes_read = i2c_smbus_block_write_block_read(file[0], device_addr, command, length, datawrite, read_no_bytes, datain);
    } else if (device_addr == 0x4E) {   // 0x4E is on i2c-2 bus
        bytes_read = i2c_smbus_block_write_block_read(file[1], device_addr, command, length, datawrite, read_no_bytes, datain);
    } else {
        return bytes_read; // No bytes read
    }

    return bytes_read;
}

// This function was never used
void set_i2c_addr(__u8 addr)
{
    int rc;
    rc = ioctl(file[0], I2C_SLAVE, addr);
    if (rc < 0)
    {
		err(errno, "Tried to set device address '0x%02x'", addr);       
    }   

}

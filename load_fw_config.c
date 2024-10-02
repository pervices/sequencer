// Copyright(c) 2019 Analog Devices, Inc.
// All Rights Reserved.
// This software is proprietary to Analog Devices, Inc. and its licensors.


//=========================================================================
// INCLUDES
//=========================================================================
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "library/adm1266.h"
#ifndef _MSC_VER
#include <linux/types.h>
#endif /* __MSC_VER */

#define ADM1266_NUM 1 // Always be one

//=========================================================================
// MAIN PROGRAM ENTRY POINT
//=========================================================================
int main(int argc, char *argv[])
{
	int opt;
	const char* file_path;
	const char* device_name;
	while ((opt = getopt(argc, argv, "i:b:")) != -1) {
		switch (opt) {
			case 'i': 
				file_path = optarg;
				break;
			case 'b': 
				device_name = optarg;
				break;
			default:
				printf("Usage: %s [-i] [file...] [-b] [digital | power]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	if (argc < 4) {
		printf("Insufficient arguments\nUsage: %s [-i] [file...] [-b] [digital | power]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Setting the sequencer address
	__u8 ADM1266_Address[ADM1266_NUM];
	const char *i2c_bus_path;

	if (strcmp(device_name, "digital") == 0 || strcmp(device_name, "dig") == 0) {
		// Address for digital board sequencer is 0x4F
		ADM1266_Address[0] = 0x4F;
		// The digital board sequencer 0x4F is on i2c-1 bus
		i2c_bus_path = "/dev/i2c-1";

		// Check if file name is correct
		if (strstr(file_path, "digital") != NULL) {
			// file name have digital in it
			// good
			printf("\033[0;32m[INFO]\033[0m Updating digital board sequencer with '%s'.\n", file_path);
		} else {
			printf("\033[0;31m[ERROR]\033[0m Sequencer update file specified does not match board type.\n");
			exit(EXIT_FAILURE);
		}
	} else if (strcmp(device_name, "power") == 0 || strcmp(device_name, "pwr") == 0) {
		// Address for power board sequencer is 0x4E
		ADM1266_Address[0] = 0x4E;
		// The power board sequencer 0x4E is on i2c-2 bus
		i2c_bus_path = "/dev/i2c-2";

		// Check if file name is correct
		if (strstr(file_path, "power") != NULL) {
			// file name have power in it
			// good
			printf("\033[0;32m[INFO]\033[0m Updating power board sequencer with '%s'.\n", file_path);
		} else {
			printf("\033[0;31m[ERROR]\033[0m Sequencer update file specified does not match board type.\n");
			exit(EXIT_FAILURE);
		}
	} else {
		printf("\033[0;31m[ERROR]\033[0m Wrong board type entered\n");
		exit(EXIT_FAILURE);
	}

	i2c_init(i2c_bus_path); // Uncomment for Linux System
	//int aardvark_id = 1845961448; // Uncomment when using Aardvark
	//aardvark_open(aardvark_id); // Uncomment when using Aardvark

	// Pointer to the variable for storing the path to ADM1266 firmware .*hex file
	FILE *ADM1266_ptr_file_fw;
	// Pointer to the variable for storing the path to ADM1266 configuration files .*hex file
	FILE *ADM1266_ptr_file_cfg[ADM1266_NUM];

	// Address of all the ADM1266 in the system
	// __u8 ADM1266_Address[ADM1266_NUM] = { 0x4E, 0x4F };

	// For storing user input for update type: configuration, firmware or both
	__u8 ADM1266_update_type;

	// For storing user input for reset type: seamless or restart
	__u8 ADM1266_reset_type;

	// For storing the password
	//__u8 ADM1266_password;

	// Path of the firmware file
	// ADM1266_ptr_file_fw = fopen("./config_files/adm1266_v1.15.4.hex", "r");
	// We will not program the firmware

	// Path of the configuration file for linux
	// ADM1266_ptr_file_cfg[0] = fopen("./config_files/tate-pwr-4E.hex", "r");
	// ADM1266_ptr_file_cfg[1] = fopen("./config_files/tate-dig-4F.hex", "r");
	ADM1266_ptr_file_cfg[0] = fopen(file_path, "r");

	char foo[100];
	printf("\033[0;32m[INFO]\033[0m Press Enter to start programming");
	fgets(foo, 100, stdin);

	// Check for if refresh is running and all the devices are present
	if ((ADM1266_Refresh_Status(ADM1266_Address, ADM1266_NUM) == 1) || (ADM1266_Device_Present(ADM1266_Address, ADM1266_NUM) == 0))
	{
		if ((ADM1266_Refresh_Status(ADM1266_Address, ADM1266_NUM) == 1))
		{
			printf("\033[0;31m[ERROR]\033[0m Memory refresh is currently running, please try after 10 secounds.\n");
		}
		else
		{
			printf("\033[0;31m[ERROR]\033[0m Not all the devices defined are present.\n");
		}
	}
	else
	{
		// always update configuration only
		ADM1266_Program_Config(ADM1266_Address, ADM1266_NUM, ADM1266_ptr_file_cfg, 1);
		// check CRC
		ADM1266_CRC_Summary(ADM1266_Address, ADM1266_NUM);
	}

	return 0;
}
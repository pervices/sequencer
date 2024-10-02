// Copyright(c) 2019 Analog Devices, Inc.
// All Rights Reserved.
// This software is proprietary to Analog Devices, Inc. and its licensors.

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


int main(int argc, char *argv[])
{
	int opt;
	const char* device_name;
	while ((opt = getopt(argc, argv, "b:")) != -1) {
		switch (opt) {
			case 'b': 
				device_name = optarg;
				break;
			default:
				printf("Usage: %s [-b] [digital | power]\n", argv[0]);
				exit(EXIT_FAILURE);
		}
	}

	if (argc < 2) {
		printf("Insufficient arguments\nUsage: %s [-b] [digital | power]\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Setting the sequencer address
	__u8 ADM1266_Address[ADM1266_NUM];

	if (strcmp(device_name, "digital") == 0 || strcmp(device_name, "dig") == 0) {
		// Address for digital board sequencer is 0x4F
		ADM1266_Address[0] = 0x4F;
		printf("Trying to read from digital board sequencer.\n");
	} else if (strcmp(device_name, "power") == 0 || strcmp(device_name, "pwr") == 0) {
		// Address for power board sequencer is 0x4E
		ADM1266_Address[0] = 0x4E;
		printf("Trying to read from power board sequencer.\n");
	} else {
		printf("\033[0;31m[ERROR]\033[0m Wrong board type entered\n");
		exit(EXIT_FAILURE);
	}

	i2c_init(); // Uncomment for Linux System
	//int aardvark_id = 1845961448; // Uncomment when using Aardvark
	//aardvark_open(aardvark_id); // Uncomment when using Aardvark
	
	// Specify the hex PMBus address for each ADM1266 in your system
	// __u8 ADM1266_Address[ADM1266_NUM] = { 0x4E, 0x4F }; 

	// Include following Variables in your code
	__s32 temp = 1;
	__u8 i = 0;
	__u8 k = 0;
	__u8 ADM1266_VH_Data[ADM1266_NUM][5][15] = { "" };
	__u8 ADM1266_VP_Data[ADM1266_NUM][14][15] = { "" };
	__u8 ADM1266_Signals_Data[ADM1266_NUM][25][7] = { "" };
	__u8 ADM1266_BB_Data[ADM1266_NUM][64] = { "" };
	__u8 ADM1266_System_Data[ADM1266_NUM*2048] = { "" };
	__u16 ADM1266_State_Name[100][2];
	__u16 ADM1266_Rail_Name[(ADM1266_NUM * 17)+10][2];
	__u16 ADM1266_Signal_Name[ADM1266_NUM * 25][2];
	__u16 ADM1266_Record_Index = 0;
	__u16 ADM1266_Num_Records = 0;
	__u8 ADM1266_PDIO_GPIO_Pad[26] = { 0,22,30,31,32,33,34,35,36,37,23,24,25,26,27,28,29,14,15,16,44,45,46,43,18,19 };
	__u8 ADM1266_VX_Pad[18] = { 0,47,48,49,50,51,56,57,58,59,60,61,62,63,52,53,54,55 };

	// Check if Refresh Memory feature is running
	if ((ADM1266_Refresh_Status(ADM1266_Address, ADM1266_NUM) == 1) || (ADM1266_Device_Present(ADM1266_Address, ADM1266_NUM) == 0))
	{
		if ((ADM1266_Refresh_Status(ADM1266_Address, ADM1266_NUM) == 1))
		{
			printf("Memory refresh is currently running, please try after 10 secounds.");
		}
		else
		{
			printf("Not all the devices defined are present.");
		}
	}
	else
	{
		// This function takes in the Number of ADM1266 and their PMBus address, and reads back all the system information
		// like Rail,Signal and State names and returns the raw data back to the 'ADM1266_System_Data' Array
		ADM1266_System_Read(ADM1266_NUM, ADM1266_Address, ADM1266_System_Data);

		// This function takes in the raw system data 'ADM1266_System_Data' Array, and parses it to return various arrays
		// 'ADM1266_State_Name' Array has all the state names,  'ADM1266_Rail_Name' array has all the rail names
		// 'ADM1266_Signal_Name' Array has all the signal names, 'ADM1266_VH_Data' has the VH status and PDIO and Rails mapping
		// 'ADM1266_VP_Data' has the VP status and PDIO and Rails mapping, 'ADM1266_Signals_Data' has the Signals status and PDIO/GPIO mapping
		ADM1266_System_Parse(ADM1266_System_Data, (__u16 *)ADM1266_State_Name, (__u16 *)ADM1266_Rail_Name, (__u16 *)ADM1266_Signal_Name, (__u8 *)ADM1266_VH_Data, (__u8 *)ADM1266_VP_Data, (__u8 *)ADM1266_Signals_Data, ADM1266_PDIO_GPIO_Pad, ADM1266_VX_Pad);

		// This function takes the PMBus address, and returns the Number of Blackbox Records and the Last Blackbox Record Index
		ADM1266_Get_Num_Records(ADM1266_Address, &ADM1266_Record_Index, &ADM1266_Num_Records);

		printf("%d records found. Enter the record number you want to read, or type 0 for all, or type 12345 for clearing the blackbox : ", ADM1266_Num_Records);
		scanf("%d", &temp);


		if (temp == 12345)
		{
			// This function takes in the Number of ADM1266 and their PMBus address, and Erases the Blackbox Records from the EEPROM of each ADM1266
			ADM1266_BB_Clear(ADM1266_NUM, ADM1266_Address);
			ADM1266_Delay(500);
			ADM1266_Get_Num_Records(ADM1266_Address, &ADM1266_Record_Index, &ADM1266_Num_Records);
			if (ADM1266_Num_Records == 0)
			{
				printf("Blackbox Erase successfull");
			}
			else
			{
				printf("Blackbox Erase failed");
			}
		}
		else {
			if (temp < 33) {
				if (temp == 0)
				{
					for (i = 1; i < ADM1266_Num_Records + 1; i++)
					{
						// This function takes in the raw 'ADM1266_System_Data' and prints the name of the User Configuration present in ADM1266
						ADM1266_Configuration_Name(ADM1266_System_Data);

						// This function takes in the Number of ADM1266 and their PMBus address, It also takes in the index of the Blackbox record to readback
						// It reads back the Blackbox Record Raw Data and saves it in the 'ADM1266_BB_Data' array
						ADM1266_Get_BB_Raw_Data(ADM1266_NUM, ADM1266_Address, i, ADM1266_Record_Index, ADM1266_Num_Records, (__u8 *)ADM1266_BB_Data);

						// This function takes in the raw blackbox data 'ADM1266_BB_Data' Array, and parses it to print the blackbox Record
						ADM1266_BB_Parse(ADM1266_NUM, (__u8 *)ADM1266_BB_Data, ADM1266_System_Data, (__u16 *)ADM1266_State_Name, (__u8 *)ADM1266_VH_Data, (__u8 *)ADM1266_VP_Data, (__u8 *)ADM1266_Signals_Data, (__u16 *)ADM1266_Rail_Name, (__u16 *)ADM1266_Signal_Name);
						printf("\n\n\n\n\n");
					}
				}
				else if (ADM1266_Num_Records > 0)
				{
					ADM1266_Get_BB_Raw_Data(ADM1266_NUM, ADM1266_Address, temp, ADM1266_Record_Index, ADM1266_Num_Records, (__u8 *)ADM1266_BB_Data);
					ADM1266_Configuration_Name(ADM1266_System_Data);
					ADM1266_BB_Parse(ADM1266_NUM, (__u8 *)ADM1266_BB_Data, ADM1266_System_Data, (__u16 *)ADM1266_State_Name, (__u8 *)ADM1266_VH_Data, (__u8 *)ADM1266_VP_Data, (__u8 *)ADM1266_Signals_Data, (__u16 *)ADM1266_Rail_Name, (__u16 *)ADM1266_Signal_Name);
				}
			}
		}
	}


	char foo[100];
	printf("\nPress Enter to exit the program");
	fgets(foo, 100, stdin);

	return 0;
}



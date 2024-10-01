# 'Makefile'
CC		= gcc
CFLAGS	= -lm 

all: load_fw blackbox monitor

load_fw: 
	$(CC) load_fw_config.c library/adm1266.c library/adm1266_pmbus_interface.c library/adi_pmbus.c $(CFLAGS) -o adm1266_load_fw_config

blackbox: 
	$(CC) Blackbox.c library/adm1266.c library/adm1266_pmbus_interface.c library/adi_pmbus.c $(CFLAGS) -o adm1266_blackbox

monitor:
	$(CC) Monitor.c library/adm1266.c library/adm1266_pmbus_interface.c library/adi_pmbus.c $(CFLAGS) -o adm1266_monitor

.PHONY: clean

clean:
	rm -f adm1266_load_fw_config adm1266_blackbox adm1266_monitor
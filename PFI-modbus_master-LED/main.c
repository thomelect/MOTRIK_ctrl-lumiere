/*
 * PFI-modbus_master.c
 *
 * Created: 2021-05-26 15:11:06
 * Author : 201770397
 */ 

#include <avr/io.h>
#include "ledControl.h"

int main(void)
{
    ledControlInit();
    while (1) 
    {
		ledControl(0);
    }
}



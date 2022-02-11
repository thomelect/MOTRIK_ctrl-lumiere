/**
 * @file 		  main.c
 * @brief 	  Partie du projet Motrik qui sera utilisé afin de contrôler les lumières de la moto.
 * @author 	  Thomas Desrosiers
 * @version   1.0
 * @date 		  2022/02/11

 * @mainpage 	PFI-modbus_slave-capteur
 * @author 	  Thomas Desrosiers
 * @section 	MainSection1 Description
              Partie du projet Motrik qui sera utilisé afin de contrôler les lumières de la moto.
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
/**
 * @file 	  main.c
 * @brief 	  Partie du projet Motrik qui sera utilisé afin de contrôler les lumières de la moto.
 * @author 	  Thomas Desrosiers
 * @version   1.0
 * @date 	  2022/02/11

 * @mainpage  MOTRIK_ctrl-lumiere
 * @author 	  Thomas Desrosiers
 * @section   MainSection1 Description
              Partie du projet Motrik qui sera utilisé afin de contrôler les lumières de la moto.
*/

#define F_CPU 16000000UL
#include <avr/io.h>
#include "usart.h"


/**********
 * DEFINE *
 **********/
#define OUT_1_INIT()	(DDRB |= (1<<4)) //initialise PB4 comme étant une sortie.
#define OUT_2_INIT()	(DDRB |= (1<<5)) //initialise PB5 comme étant une sortie.
#define OUT_3_INIT()	(DDRB |= (1<<7)) //initialise PB7 comme étant une sortie. */
#define OUT_1_SET(a)	(PORTB = (PORTB & ~(1<<4)) | ((a && 1) << 4)) //État de la sortie #1.
#define OUT_2_SET(a)	(PORTB = (PORTB & ~(1<<5)) | ((a && 1) << 5)) //État de la sortie #2.
#define OUT_3_SET(a)	(PORTB = (PORTB & ~(1<<7)) | ((a && 1) << 7)) //État de la sortie #3.
#define OUT_1_TGL()		(PORTB ^= (1<<4)) //État de la sortie #1. */
#define OUT_2_TGL()		(PORTB ^= (1<<5)) //État de la sortie #2. */
#define OUT_3_TGL()		(PORTB ^= (1<<7)) //État de la sortie #3. */
#define OUT_2(val) 		(OCR1A = val) // Valeur PWM output V.
#define OUT_3(val) 		(OCR1C = val) // Valeur PWM output B.

#define _TIMER_1_TOP 100
#define _TIMER_1_CYC_CNT 25
#define INCREMENT_DEL 25


/************
 * VARIABLE *
 ************/
volatile uint8_t cinqCentMsFlag = 0;
volatile uint8_t cntCinqCentMs = 0;
uint8_t intensiteDEL1 = 0;


enum COMM_STATES {WAIT,NUM,DATA,VALIDATE};
enum COMM_STATES commState = WAIT;

/**************************
 * PROTOTYPES DE FONCTION *
 **************************/
/**
*@brief Fonction d'initialisation des différents I/O et fonctions.
*/
void miscInit(void);
void lightsAssingment(void);
/**
 *@brief  Fonction d'initialisation du timer 0 avec une période de 4ms.
 */
void timer1Init();
uint8_t rxData = 0;
uint8_t numLumiere = 0;
uint8_t valLumiere = 0;

/********
 * MAIN *
 ********/
int main(void)
{
  miscInit();
  while (1)
  {
	  if(usartRxAvailable())
	  {
		  rxData = usartRemRxData();
		  switch (commState)
		  {
			  default : // Pour WAIT
				  if (rxData == '<')
				  {
					  commState = NUM;
				  }
				  break;
			  case NUM :
				  numLumiere = rxData;
				  if ((numLumiere >= 1)&&(numLumiere <= 3))
				  {
					  commState = DATA;
				  }
				  else
					  commState = WAIT;
				  break;
			  case DATA :
				  if(numLumiere == 1)
				  {
					  if ((rxData == 0) || (rxData == 1))
					  {
						  valLumiere = rxData;
						  commState = VALIDATE;
					  }
					  else
						commState = WAIT;
				  }
				  else
				  {
					  if ((rxData >= 0)&&(rxData <= 255))
					  {
						  valLumiere = rxData;
						  commState = VALIDATE;
					  }
					  else
						  commState = WAIT;
				  }
				  break;
			  case VALIDATE :
				  if(rxData == '>')
				   	  lightsAssingment();
				  commState = WAIT;
				  break;
		  }
	  }
  }
}


/************************
 * DÉFINITION FONCTIONS *
 ************************/
void miscInit(void)
{
	timer1Init(); //Initialisation des timers
	OUT_1_INIT();
	OUT_2_INIT();
	OUT_3_INIT();
}

void timer1Init(void)
{
	// TCCR1A : COM1A1 COM1A0 COM1B1 COM1B0 COM1C1 COM1C0 WGM11 WGM10
	// TCCR1B: ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
	// TIMSK1: – – ICIE1 – OCIE1C OCIE1B OCIE1A TOIE1
	TCCR1A = (1 << COM1A1) | (1 << COM1C1); // Pwm sur OCR1A et OCR1C.
	TCCR1A |= (1 << WGM10); // Fast PWM, 8-bit top -> 0x00FF.
	TCCR1B = (1 << WGM12); // Fast PWM, 8-bit top -> 0x00FF.
	TCCR1B |= (1 << CS11) | (1 << CS11); // Prescaler de 1024. 
	OCR1A = 0; // 62.5ns * 64 * 250 * 5 = 5ms.
	OCR1C = 0;
}

void lightsAssingment(void)
{
	switch(numLumiere)
	{
		case 1:
			OUT_1_SET(valLumiere);
			break;
		case 2:
			OUT_2(valLumiere);
			break;
		case 3:
			OUT_3(valLumiere);
			break;
	}
}
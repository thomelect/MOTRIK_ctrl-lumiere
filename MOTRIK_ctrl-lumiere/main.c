/**
 * @file 	  main.c
 * @brief 	  Partie du projet Motrik qui sera utilisé afin de contrôler les lumières de la moto.
 * @author 	  Thomas Desrosiers
 * @version   2.0
 * @date 	  2022/05/25

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
#define OUT_1_INIT() (DDRB |= (1 << 4))								  // Initialise PB4 comme étant une sortie.
#define OUT_2_INIT() (DDRB |= (1 << 5))								  // Initialise PB5 comme étant une sortie.
#define OUT_3_INIT() (DDRB |= (1 << 7))								  // Initialise PB7 comme étant une sortie.
#define OUT_1_SET(a) (PORTB = (PORTB &= ~(1 << 4)) | ((a && 1) << 4)) //État de la sortie #1.
#define OUT_2(val) (OCR1A = val)									  // Valeur PWM output #2.
#define OUT_3(val) (OCR1C = val)									  // Valeur PWM output #3.

/************
 * VARIABLE *
 ************/
volatile uint8_t cinqCentMsFlag = 0;
volatile uint8_t cntCinqCentMs = 0;
uint8_t rxData = 0;
uint8_t numSortie = 0;
uint8_t valSortie = 0;

/******************
 *      ENUM      *
 * STRUCT & UNION *
 ******************/
enum COMM_STATES
{
	WAIT,
	NUM,
	DATA,
	VALIDATE
};
enum COMM_STATES commState = WAIT;

/**************************
 * PROTOTYPES DE FONCTION *
 **************************/
/**
 *@brief Fonction d'initialisation des différents I/O et fonctions.
 */
void miscInit(void);

/**
 * @brief 			 Fonction qui reçoit en paramètre un numéro de sortie et lu affecte une valeur de sortie elle aussi reçue en paramètre.
 * @param numSortie  Choix de la sortie à affecter. 1 / 2 /3
 * @param valSortie  Valeur que prendra la sortie. 0 - 255
 */
void outState(uint8_t numSortie, uint8_t valSortie);

/**
 *@brief  Fonction d'initialisation du timer 0 avec une période de 4ms.
 */
void timer1Init();

/********
 * MAIN *
 ********/
int main(void)
{
	miscInit();
	outState(1, 1);
	outState(2, 0);
	outState(3, 0);
	while (1)
	{
		if (usartRxAvailable())
		{
			rxData = usartRemRxData();
			switch (commState)
			{
			default: // Pour WAIT
				if (rxData == '<')
				{
					commState = NUM;
				}
				break;
			case NUM:
				numSortie = rxData;
				if ((numSortie >= 1) && (numSortie <= 3))
				{
					commState = DATA;
				}
				else
					commState = WAIT;
				break;
			case DATA:
				if (numSortie == 1)
				{
					if ((rxData == 0) || (rxData == 1))
					{
						valSortie = rxData;
						commState = VALIDATE;
					}
					else
						commState = WAIT;
				}
				else
				{
					if ((rxData >= 0) && (rxData <= 255))
					{
						valSortie = rxData;
						commState = VALIDATE;
					}
					else
						commState = WAIT;
				}
				break;
			case VALIDATE:
				if (rxData == '>')
					outState(numSortie, valSortie);
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
	timer1Init(); // Initialisation des timers
	usartInit(1000000, F_CPU);
	OUT_1_INIT();
	OUT_2_INIT();
	OUT_3_INIT();
}

void outState(uint8_t numSortie, uint8_t valSortie)
{
	switch (numSortie)
	{
	case 1:

		if (valSortie)
			OUT_1_SET(valSortie);
		break;
	case 2:
		OUT_2(valSortie);
		break;
	case 3:
		OUT_3(valSortie);
		break;
	}
}

void timer1Init(void)
{
	// TCCR1A : COM1A1 COM1A0 COM1B1 COM1B0 COM1C1 COM1C0 WGM11 WGM10
	// TCCR1B: ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
	// TIMSK1: – – ICIE1 – OCIE1C OCIE1B OCIE1A TOIE1
	TCCR1A = (1 << COM1A1) | (1 << COM1C1); // PWM sur OCR1A et OCR1C.
	TCCR1A |= (1 << WGM10);					// Fast PWM, 8-bit top -> 0x00FF.
	TCCR1B = (1 << WGM12);					// Fast PWM, 8-bit top -> 0x00FF.
	TCCR1B |= (1 << CS11) | (1 << CS11);	// Prescaler de 1024.
	OCR1A = 0;								// 62.5ns * 64 * 250 * 5 = 5ms.
	OCR1C = 0;
}
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
#include <avr/interrupt.h>
#include "usart.h"

/**********
 * DEFINE *
 **********/
#define OUT_INIT() (DDRB |= (1 << 4) | (1 << 5) | (1 << 7))			  // Initialise les trois sorties.
#define OUT_1_SET(a) (PORTB = (PORTB &= ~(1 << 4)) | ((a && 1) << 4)) // État de la sortie #1.
#define OUT_2(val) (OCR1A = val)									  // Valeur PWM output #2.
#define OUT_3(val) (OCR1C = val)									  // Valeur PWM output #3.

#define INIT_STATE_OUT_1 0	// État initial de la sortie #1
#define INIT_STATE_OUT_2 25 // État initial de la sortie #2
#define INIT_STATE_OUT_3 25 // État initial de la sortie #3

#define TIMER_CNT_CYCLE_FADE 50 // Nombre de cycles comptés en interruption.

#define INCREMENT_STEP 1 // Incrément pour le fade.
#define FADE_FLOOR 1	 // Valeur minimale du fade. Lorsque cette valeur est atteinte la valeur de la sortie commence à augmenter.
#define FADE_TOP 75		 // Valeur maximale du fade. Lorsque cette valeur est atteinte la valeur de la sortie commence à diminuer.

#define _MAX_RXDATASIZE_ 3

/************
 * VARIABLE *
 ************/
/* Variables utilisés pour le fade. */
volatile uint16_t msCntFade = 0; // Compteur utilisés pour compter 50 fois un délai de 1ms pour le fade de la sortie.
volatile uint8_t msFlagFade = 0; // Flags qui est mis à 1 à chaques 50ms pour le fade de la sortie.
uint8_t valueOut;

/* Variables nécessaires à la communication sérielle: */
uint8_t rxDataSize;
uint8_t rxDataCnt = 0; // Compteur de donnés reçus.
uint8_t rxData[_MAX_RXDATASIZE_];
uint8_t rxErrorCommCnt = 0;

/******************
 *      ENUM      *
 * STRUCT & UNION *
 ******************/
/* Enum des différents étapes d'une réception: */
enum RX_STATES
{
	WAIT,
	RXSIZE,
	RXCOMMANDE,
	RXDATA,
	VALIDATE
};

/* Enum des différentes commandes utilisées en réception: */
enum RX_COMMANDES
{
	INIT,
	FADE,
	SET_VAL,
	SET_VAL_ALL
};

// déclaration des enums
enum RX_STATES rxState;
enum RX_COMMANDES rxCommande;

/**************************
 * PROTOTYPES DE FONCTION *
 **************************/
/**
 * @brief  Fonction de traitement des données et commandes reçues.
 */
void execRxCommand(void);

/**
 * @brief  Fonction utilisée pour gérer la modification des valeurs des sortie en mode fade.
 */
void fade(void);

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
 * @brief		Fonction qui remplis la structure de donnés avec les paramètres correspondants qui lui sont envoyés en paramètre par la fonction usartRemRxData. Le switch case commence à l'état WAIT qui attend la réception de "<". RXDATA place alors les donnés reçus dans l'union de structure jusqu'à ce que la dernière donnée (ici, la longueur de la trame à été spécifié manuellement à 7 puisque nous n'envoyons pas l'octet qui contient la longueur de la trame. Finalement, VALIDATE s'assure que la trame se termine par ">"
 * @param data  Octet reçu par la fonction usartRemRxData
 */
void parseRxData(uint8_t data);

/**
 *@brief  Fonction d'initialisation du timer 0 avec une période de 4ms.
 */
void timer0Init();

/**
 *@brief  Fonction d'initialisation du timer 1 avec une période de 4ms.
 */
void timer1Init();

/********
 * MAIN *
 ********/
int main(void)
{
	miscInit();
	while (1)
	{
		if (usartRxAvailable())
		{
			parseRxData(usartRemRxData());
		}
		usartSendByte(rxErrorCommCnt);
		if (rxCommande == FADE)
		{
			fade();
		}
	}
}

/****************
 * INTERRUPTION *
 ****************/
/**
 *@brief Interruption de la comparaison sur OCR0A (timer0) qui met msFlag à 1 lorsque msCnt atteint TIMER_CNT_CYCLE qui est définie dans le define. msCnt est incrémenté à chaques 1ms.
 */
ISR(TIMER0_COMPA_vect)
{
	msCntFade++;
	if (msCntFade >= TIMER_CNT_CYCLE_FADE)
	{
		msCntFade -= TIMER_CNT_CYCLE_FADE;
		msFlagFade = 1;
	}
}

/************************
 * DÉFINITION FONCTIONS *
 ************************/
void execRxCommand(void)
{
	switch (rxCommande)
	{
	case INIT:
		outState(1, INIT_STATE_OUT_1);
		outState(2, INIT_STATE_OUT_2);
		outState(3, INIT_STATE_OUT_3);
	break;
	case FADE:
		fade();
		break;
	case SET_VAL:
		outState(rxData[0], rxData[1]);
		break;
	case SET_VAL_ALL:
		outState(1, rxData[0]);
		outState(2, rxData[0]);
		outState(3, rxData[0]);
		break;
	}
}

void fade(void)
{
	int increment = INCREMENT_STEP; // Valeur actuelle de l'incrément pour le fade.

	if (valueOut <= FADE_FLOOR) // Lorsque oc4aValue à atteint son minimum.
	{
		increment = INCREMENT_STEP;
	}
	if (valueOut >= FADE_TOP) // Lorsque oc4aValue à atteint son maximum.
	{
		increment = -INCREMENT_STEP;
	}
	if (msFlagFade)
	{
		msFlagFade = 0;
		valueOut += increment;
	}
	outState(2, valueOut);
	outState(3, FADE_TOP - valueOut);
}

void miscInit(void)
{
	timer0Init(); // Initialisation des timers
	timer1Init();
	usartInit(1000000, F_CPU);
	// 	OUT_1_INIT();
	// 	OUT_2_INIT();
	// 	OUT_3_INIT();
	OUT_INIT();
	outState(1, INIT_STATE_OUT_1);
	outState(2, INIT_STATE_OUT_2);
	outState(3, INIT_STATE_OUT_3);
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

void parseRxData(uint8_t data)
{
	// switch case des différents paramètres de la trame de réception
	switch (rxState)
	{
	// confirmation que la trame débute par '<'
	default:
		if (data == '<')
		{
			rxState = RXSIZE;
			rxDataCnt = 0;
		}
		break;
	//////////////////////////////////////////////////////////////////////////
	case RXSIZE:
		rxDataSize = data;
		if (rxDataSize >= _MAX_RXDATASIZE_)
			rxState = WAIT;
		else
			rxState = RXCOMMANDE;
		break;
	//////////////////////////////////////////////////////////////////////////
	case RXCOMMANDE:
		rxCommande = data;
		if (rxDataSize)
			rxState = RXDATA;
		else
			rxState = VALIDATE;
		break;
	//////////////////////////////////////////////////////////////////////////
	case RXDATA:
		rxData[rxDataCnt++] = data;
		if (rxDataCnt == rxDataSize)
			rxState = VALIDATE;
		break;
	//////////////////////////////////////////////////////////////////////////
	// confirmation que la trame se termine par '>'
	case VALIDATE:
		rxState = WAIT;
		if (data == '>')
			execRxCommand(); // si oui la fonction execRxCommand() est appelée
		else
			rxErrorCommCnt++; // sinon le nombre d'erreur augmente
		break;
	}
}

void timer0Init(void)
{
	// TCCR0A : COM0A1 COM0A0 COM0B1 COM0B0 – – WGM01 WGM00
	// TCCR0B : FOC0A FOC0B – – WGM02 CS02 CS01 CS00
	// TIMSK0 : – – – – – OCIE0B OCIE0A TOIE0
	uint8_t top = 250;	 // Valeur de OCR0A.
	TCCR0A = 0b00000010; // Mode CTC.
	TCCR0B = 0b00000011; // Prescaler de 64.
	TIMSK0 = 0b00000010; // Output compare match A interrupt enable.
	OCR0A = top - 1;	 // 62.5ns * 64 * 250 = 1ms.
	sei();
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
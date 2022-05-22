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
#include <avr/interrupt.h>


/**********
 * DEFINE *
 **********/
#define OUT_1_INIT()	(DDRB |= (1<<4)) //initialise PB4 comme étant une sortie.
#define OUT_2_INIT()	(DDRB |= (1<<5)) //initialise PB5 comme étant une sortie.
#define OUT_3_INIT()	(DDRB |= (1<<7)) //initialise PB7 comme étant une sortie. */
#define OUT_1_SET(a)	(PORTB = (PORTB & ~(1<<4)) | ((a && 1) << 4)) //État de la sortie #1.
#define OUT_2_SET(a)	(PORTB = (PORTB & ~(1<<5)) | ((a && 1) << 5)) //État de la sortie #2.
#define OUT_3_SET(a)	(PORTB = (PORTB & ~(1<<7)) | ((a && 1) << 7)) //État de la sortie #3. */
#define OUT_1_TGL()		(PORTB ^= (1<<4)) //État de la sortie #1. */
#define OUT_2_TGL()		(PORTB ^= (1<<5)) //État de la sortie #2. */
#define OUT_3_TGL()		(PORTB ^= (1<<7)) //État de la sortie #3. */
#define OUT_1(val) 		(OCR4A = val) // Valeur PWM output R.
#define OUT_2(val) 		(OCR4B = val) // Valeur PWM output V.
#define OUT_3(val) 		(OCR0A = val) // Valeur PWM output B.

#define _TIMER_1_TOP 100
#define _TIMER_1_CYC_CNT 25
#define INCREMENT_DEL 25


/************
 * VARIABLE *
 ************/
volatile uint8_t cinqCentMsFlag = 0;
volatile uint8_t cntCinqCentMs = 0;
uint8_t intensiteDEL1 = 0;


/**************************
 * PROTOTYPES DE FONCTION *
 **************************/
/**
*@brief Fonction d'initialisation des différents I/O et fonctions.
*/
void miscInit(void);

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
  OUT_1_SET(1);
  OUT_2(50);
  OUT_3(100);
  while (1)
  {

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
	TCCR0A = (1 << COM0A1) | (1 << WGM00) | (1 << WGM01); // Pwm sur OCR1A et OCR1C.
	TCCR0B = (1 << CS02) | (1 << CS00); // mode CTC.
	TCCR1B |= (1 << CS11); // Prescaler de 1024.
	OCR0A = 127 - 1; // 62.5ns * 64 * 250 * 5 = 5ms.
	//sei();
}
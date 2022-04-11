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
#define DEL_1_INIT()	(DDRC |= (1<<7)) //initialise PC7 comme étant une sortie.
#define DEL_1(val) (OCR4A = val) // Valeur PWM output R.
#define DEL_2(val) (OCR4B = val) // Valeur PWM output V.
#define DEL_3(val) (OCR4D = val) // Valeur PWM output B.

#define _TIMER_1_TOP 250
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

/**
 *@brief  Fonction d'initialision du timer 4 en mode PWM.
 */
void timer4Init();


/********
 * MAIN *
 ********/
int main(void)
{
  miscInit();
  while (1)
  {
    if (cinqCentMsFlag)
    {
      cinqCentMsFlag = 0;
      intensiteDEL1 += 1;
      if (intensiteDEL1 >= 255)
        intensiteDEL1 -= 255;
      DEL_1(intensiteDEL1);
    }
  }
}


/****************
 * INTERRUPTION *
 ****************/
/**
 * @brief  Interruption qui met cinqCentMSFlag à 1 à chaques 250ms.
 */
ISR(TIMER1_COMPA_vect)
{
  	cntCinqCentMs++;
	if (cntCinqCentMs >= _TIMER_1_CYC_CNT)
	{
		cntCinqCentMs -= _TIMER_1_CYC_CNT;
		cinqCentMsFlag = 1; //À chaque 500ms. Ce flag sera utilisé pour changer le backlight et l'affichage des valeurs des capteurs.
	}
}


/************************
 * DÉFINITION FONCTIONS *
 ************************/
void miscInit(void)
{
	timer1Init(); //Initialisation des timers
	timer4Init();
	DEL_1_INIT();
}

void timer4Init(void) //Le timer 4 est utilisé pour contôler la sortie PD7.
{
	//TCCR4A: COM4A1 COM4A0 COM4B1 COM4B0 FOC4A FOC4B PWM4A PWM4B
	//TCCR4B: PWM4X PSR4 DTPS41 DTPS40 CS43 CS42 CS41 CS40
	//TCCR4C: COM4A1S COM4A0S COM4B1S COMAB0S COM4D1 COM4D0 FOC4D PWM4D
	//TCCR4D: FPIE4 FPEN4 FPNC4 FPES4 FPAC4 FPF4 WGM41 WGM40
	//TCCR4E: TLOCK4 ENHC4 OC4OE5 OC4OE4 OC4OE3 OC4OE2 OC4OE1 OC4OE0
	TCCR4C = 0b00001001; // PWM output OC4D.
	TCCR4A = 0b10100011; // PWM output OC4A et OC4B.
	TCCR4B = 0b00000100; // Prescaler de 4.
	OCR4C = 255 - 1;	 // 62.5ns * 4 * 255 = 127.5us.
	DEL_1(0);
	DEL_2(0);
	DEL_3(0);
}

void timer1Init(void)
{
	// TCCR1A : COM1A1 COM1A0 COM1B1 COM1B0 COM1C1 COM1C0 WGM11 WGM10
	// TCCR1B: ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
	// TIMSK1: – – ICIE1 – OCIE1C OCIE1B OCIE1A TOIE1
	TCCR1B = (1 << WGM12); // mode CTC.
	TCCR1B |= (1 << CS11) | (1 << CS10); // Prescaler de 64.
	TIMSK1 |= (1 << OCIE1A);
	OCR1A = _TIMER_1_TOP - 1; // 62.5ns * 64 * 250 * 5 = 5ms.
	sei();
}
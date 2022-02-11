/**
 * @file 	 ledControl.c
 * @brief 	 Librairie prenant en charge les différent mode de fonctionnement possibles pour le ruban de DELs RGB.
 * @author 	 Thomas Desrosiers
 * @version  1.0
 * @date 	 2021/06/02
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "ledControl.h"

#define _DEL_R_VALUE(val) (OCR4A = val) // Valeur PWM output R.
#define _DEL_V_VALUE(val) (OCR4B = val) // Valeur PWM output V.
#define _DEL_B_VALUE(val) (OCR4D = val) // Valeur PWM output B.

#define _TIMER_1_TOP 15625

uint8_t _flashFlag = 0;
uint8_t _valueLed = 255;
int8_t _increment = 0;
volatile uint8_t cinqCentMSFlag = 0;

// Prototypes de fonctions privés
/**
 * @brief  Fonction d'initialisation du timer #1 utiliser pour le faire clignoter le ruban de DELs.
 */
void _timer1Init(void);

/**
 * @brief  Fonction d'initialisation du timer #4 utilisé pour le PWM du ruban de DELs.
 */
void _timer4Init(void);

/**
 * @brief  Interruption qui met cinqCentMSFlag à 1 à chaques 250ms.
 */
ISR(TIMER1_COMPA_vect)
{
	cinqCentMSFlag = 1;
	PORTC ^= (1 << 7);
}

void _timer1Init(void)
{
	// TCCR1A : COM1A1 COM1A0 COM1B1 COM1B0 COM1C1 COM1C0 WGM11 WGM10
	// TCCR1B: ICNC1 ICES1 – WGM13 WGM12 CS12 CS11 CS10
	// TIMSK1: – – ICIE1 – OCIE1C OCIE1B OCIE1A TOIE1
	TCCR1B = (1 << WGM12); // mode CTC.
	TCCR1B |= (1 << CS12); // Prescaler de 256.
	TIMSK1 |= (1 << OCIE1A);
	OCR1A = 15625 - 1; // 62.5ns * 256 * 31250 = 250ms.
	sei();
}

void _timer4Init(void)
{
	// TCCR4A: COM4A1 COM4A0 COM4B1 COM4B0 FOC4A FOC4B PWM4A PWM4B
	// TCCR4B: PWM4X PSR4 DTPS41 DTPS40 CS43 CS42 CS41 CS40
	// TCCR4C: COM4A1S COM4A0S COM4B1S COMAB0S COM4D1 COM4D0 FOC4D PWM4D
	// TCCR4D: FPIE4 FPEN4 FPNC4 FPES4 FPAC4 FPF4 WGM41 WGM40
	// TCCR4E: TLOCK4 ENHC4 OC4OE5 OC4OE4 OC4OE3 OC4OE2 OC4OE1 OC4OE0
	TCCR4C = 0b00001001; // PWM output OC4D.
	TCCR4A = 0b10100011; // PWM output OC4A et OC4B.
	TCCR4B = 0b00000100; // Prescaler de 4.
	OCR4C = 255 - 1;	 // 62.5ns * 4 * 255 = 127.5us.
	_DEL_R_VALUE(0);	 // Sortie Rouge.
	_DEL_V_VALUE(0);	 // Sortie Vert.
	_DEL_B_VALUE(0);	 // Sortie Bleu.
}

void ledControl(uint8_t stateSystem)
{
	/*int8_t increment = 1;*/

	switch (stateSystem)
	{
	case 0: // LEDs OFF.
		_DEL_R_VALUE(0);
		_DEL_V_VALUE(0);
		_DEL_B_VALUE(0);
		break;
	case 1: // LEDs ROUGES ON.
		_DEL_R_VALUE(_valueLed);
		_DEL_V_VALUE(0);
		_DEL_B_VALUE(0);
		break;
	case 2: // LEDs VERTES ON.
		_DEL_R_VALUE(0);
		_DEL_V_VALUE(_valueLed);
		_DEL_B_VALUE(0);
		break;
	case 3: // LEDs BLEUES ON.
		_DEL_R_VALUE(0);
		_DEL_V_VALUE(0);
		_DEL_B_VALUE(_valueLed);
		break;
	case 4: // LEDs TOUTES ON.
		_DEL_R_VALUE(_valueLed);
		_DEL_V_VALUE(_valueLed);
		_DEL_B_VALUE(_valueLed);
		break;
	case 5: // LEDs ROUGES FLASH.
		if (cinqCentMSFlag && !_flashFlag)
		{
			cinqCentMSFlag = 0;
			_flashFlag = 1;
			_DEL_R_VALUE(0);
			_DEL_V_VALUE(0);
			_DEL_B_VALUE(0);
		}
		if (cinqCentMSFlag && _flashFlag)
		{
			cinqCentMSFlag = 0;
			_flashFlag = 0;
			_DEL_R_VALUE(_valueLed);
			_DEL_V_VALUE(0);
			_DEL_B_VALUE(0);
		}
		break;
	case 6: // LEDs VERTES FLASH.
		if (cinqCentMSFlag && !_flashFlag)
		{
			cinqCentMSFlag = 0;
			_flashFlag = 1;
			_DEL_R_VALUE(0);
			_DEL_V_VALUE(0);
			_DEL_B_VALUE(0);
		}
		if (cinqCentMSFlag && _flashFlag)
		{
			cinqCentMSFlag = 0;
			_flashFlag = 0;
			_DEL_R_VALUE(0);
			_DEL_V_VALUE(_valueLed);
			_DEL_B_VALUE(0);
		}
		break;
	case 7: // WHITE FADE.
		if (cinqCentMSFlag)
		{
			cinqCentMSFlag = 0;
			if (_valueLed >= 250)
				_increment = -2;
			if (_valueLed <= 0)
				_increment = 2;
			_DEL_R_VALUE(_valueLed += _increment);
			_DEL_V_VALUE(_valueLed += _increment);
			_DEL_B_VALUE(_valueLed += _increment);
		}
		break;
	}
}

void ledControlBringhtness(uint8_t brightness)
{
	if (brightness <= 255)
	{
		_valueLed = brightness;
	}
}

void ledControlInit(void)
{
	_timer1Init();
	_timer4Init();

	DEL_R_INIT();
	DEL_V_INIT();
	DEL_B_INIT();
}

void ledControlSpeed(uint8_t doubleSpeenO_N)
{
	if (doubleSpeenO_N)
	{
		doubleSpeenO_N++;
		OCR1A = (_TIMER_1_TOP / doubleSpeenO_N);
	}
	else
		OCR1A = _TIMER_1_TOP;
}
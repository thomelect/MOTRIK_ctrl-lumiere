/*
 * ledControl.h
 *
 * Created: 2021-05-27 00:18:11
 *  Author: thomas
 */ 


#ifndef LEDCONTROL_H_
#define LEDCONTROL_H_

#define DEL_R_INIT()		DDRC |= (1<<7) //Init output R.
#define DEL_V_INIT()		DDRB |= (1<<6) //Init output V.
#define DEL_B_INIT()		DDRD |= (1<<7) //Init output B.

/**
*@brief  Fonction qui assure le contr�le des l'�tat, de la luminosit� et de la couleur du ruban de DELs en fonction du param�tre re�u.
*@param  stateSystem Code d'�tat du ruban de DELs.
*/
void ledControl(uint8_t stateSystem);

/**
*@brief  Fonction d'initialisation des diff�rents I/O et fonctions.
*/
void ledControlInit();

#endif /* LEDCONTROL_H_ */
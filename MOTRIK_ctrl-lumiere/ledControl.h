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
*@brief  Fonction qui assure le contrôle des l'état, de la luminosité et de la couleur du ruban de DELs en fonction du paramètre reçu.
*@param  stateSystem Code d'état du ruban de DELs.
*/
void ledControl(uint8_t stateSystem);

/**
*@brief  Fonction d'initialisation des différents I/O et fonctions.
*/
void ledControlInit();

#endif /* LEDCONTROL_H_ */
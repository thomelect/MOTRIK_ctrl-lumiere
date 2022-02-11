/**
 * @file 	 ledControl.h
 * @brief 	 Librairie prenant en charge les différent mode de fonctionnement possibles pour le ruban de DELs RGB.
 * @author 	 Thomas Desrosiers
 * @version  1.0
 * @date 	 2021/06/02
 */

#ifndef LEDCONTROL_H_
#define LEDCONTROL_H_

#define DEL_R_INIT() DDRC |= (1 << 7) // Init output R.
#define DEL_V_INIT() DDRB |= (1 << 6) // Init output V.
#define DEL_B_INIT() DDRD |= (1 << 7) // Init output B.

/**
 * @brief               Fonction qui assure le contrôle des l'état, de la luminosité et de la couleur du ruban de DELs en fonction du paramètre reçu.
 * @param  stateSystem  Code d'état du ruban de DELs.
 */
void ledControl(uint8_t stateSystem);

/**
 * @brief              Fontion qui permet de changer l'intensitée lumineuse du ruban de DELs.
 * @param  brightness  Valeur de 0 - 255 représentant l'intensitée lumineuse du ruban de DELs.
 */
void ledControlBringhtness(uint8_t brightness);

/**
 * @brief  Fonction d'initialisation des différents I/O et fonctions.
 */
void ledControlInit(void);

/**
 * @brief                  Fonction qui permet de changer la vitesse du clignotement.
 * @param  doubleSpeenO_N  Vitesse du fade. Si la fonction reçoit 1, le clignotement est deux fois plus rapide.
 */
void ledControlSpeed(uint8_t doubleSpeenO_N);

#endif /* LEDCONTROL_H_ */
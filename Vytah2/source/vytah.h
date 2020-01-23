/*
 * vytah.h
 *
 *  Created on: 17. 1. 2020
 *      Author: rober
 */
/**
 * @file    vytah.h
 * @brief   Hlavickovy subor vytah.h
 * @details Nachadzaju sa tu hlavicky funkcii
 * 			pre pracu s vytahom.
 */

#ifndef VYTAH_H_
#define VYTAH_H_

#include "board.h"
#include <stdint.h>
#include "stdint.h"
#include "komunikacia.h"
#include "doplnky.h"
#include "clock_config.h"
#include "pin_mux.h"


/***************************************************************************//**
 * Hlavicky funkcii pre pracu s prvkami vytahu
 ******************************************************************************/
void Motor(int rychlost);
void MotorStop();
void LedOn(int8_t led);
void LedOff(int8_t led);
void LedAllOff();
void DvereBrzda(bool stav);
void Dvere(bool stav);
void StlaceneTlacidloPoschodia(NACITANEDATA data);
void PoschodieTransport(uint8_t snimacPoschodia, int polohaPoschodia, uint8_t LedPoschodia);
void StlaceneTlacidloVytahu(NACITANEDATA data);
signed int ZisitPoziciuKabiny();
float ZisitRychlostMotora();
void vypisNaDisplej(int smer, int poschodie);
bool jeSnimac(NACITANEDATA sprava);
bool jeNaPoschodi(signed int poloha,int pozadovanePoschodie);

#endif /* VYTAH_H_ */

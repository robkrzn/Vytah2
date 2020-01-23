/*
 * doplnky.c
 *
 *  Created on: 18. 1. 2020
 *      Author: rober
 */
/**
 * @file    doplnky.c
 * @brief   Subor pre doplnky.c
 * @details Nachadzaju sa tu funckcie ktore
 * 			plnia dodatkove funkcie programu ktore
 * 			nemaju zasadny vlpyv na funkcnost programu
 */

#include "stdint.h"
#include "komunikacia.h"

/***************************************************************************//**
 * Cakacia funkcia
 *
 * @param cycles pocet cyklov
 ******************************************************************************/

void delay(int cycles)
{
    while (cycles--)
    asm("nop");
}
/***************************************************************************//**
 * Funkcia na vypis nacitanych dat do konzoly.
 *
 * @param data NACITANEDATA data
 ******************************************************************************/
void vypisData(NACITANEDATA data){
	printf("=====================\n");
	for(int i = 0; i<=data.pocetZnakov;i++){
		printf("%d: %x \n",i,data.data[i]);
	}
	printf("Pocet dat : %d, pravdivost %d\n",data.pocetZnakov,data.crcOk);
	if(data.typSpravy==0){
		printf("Typ spravy : data\n");
	}else if(data.typSpravy==1){
		printf("Typ spravy : ACK\n");
	}
	printf("---------------------\n");
}

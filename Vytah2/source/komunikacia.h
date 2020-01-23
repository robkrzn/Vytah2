/*
 * komunikacia.h
 *
 *  Created on: 17. 1. 2020
 *      Author: rober
 */
/**
 * @file    komunikacia.h
 * @brief   Subor obsahuje hlavickovy funkcii komunikacia.c
 * @details Subor obsahuje hlavicky a pomocne premenne a datovu
 * 			strukturu na ukladanie dat.
 */
#ifndef KOMUNIKACIA_H_
#define KOMUNIKACIA_H_

#include <stdio.h>
#include "fsl_device_registers.h"
#include "board.h"
#include "fsl_lpsci.h"

#include "clock_config.h"
#include "pin_mux.h"

/***************************************************************************//**
 * Datova struktura na ulozenie nacitanych dat
 * data[]-nacitana sprava
 * odosielatel-odosielal spravy
 * pocetZnakov-pocet znakov spravy
 * typSpravy - 0- data , 1 ACK
 * crcOk- informacia o spravnosti CRC 1-ok 0 nie
 ******************************************************************************/
typedef struct nacitaneData{
	uint8_t data[20];
	uint8_t odosielatel;
	int pocetZnakov;
	int typSpravy; 			//0-data 1-ACK cokolvek ine iny typ spravy
	//bool status;			//0-nepodarilo sa precitat 1-sprava bola precitana
	bool crcOk;				//1-ok, 0 false
}NACITANEDATA;


#define DEMO_LPSCI UART0
#define DEMO_LPSCI_CLKSRC kCLOCK_CoreSysClk
#define DEMO_LPSCI_CLK_FREQ CLOCK_GetFreq(kCLOCK_CoreSysClk)


/***************************************************************************//**
 * Hlavicky funkcii
 ******************************************************************************/
void komunikaciaInit();
uint8_t CrcGenerator(uint8_t * sprava,int velkost);
void LPSCISendString(uint8_t *text, uint8_t dlzka);
void LPSCISendData(uint8_t adresa, uint8_t velkost, uint8_t data);
void LPSCISendLongData(uint8_t adresa, uint8_t velkost, uint8_t *data);
NACITANEDATA LPSCIRecieveData();
void AckGenerator(NACITANEDATA data);
void analyzujData(NACITANEDATA data);
bool AckDelay();

#endif /* KOMUNIKACIA_H_ */

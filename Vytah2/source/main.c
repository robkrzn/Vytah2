#include "komunikacia.h"
#include "vytah.h"
#include "doplnky.h"

/**
 * @file    main.c
 * @brief   Hlavny spustaci subor
 * @details  * Funckia main inicializuje komunikiáciu cez LPSCI, vytvori datovu strukturu na ukladanie
 * nacitanych dat 'data'. Zaroven sa vypnu led diody ak su nejake aktivne z minuleho projektu
 * a zastavi motor.
 * V nekonejcej slucne sa vykona prve citanie a v nom sa overi ci je spravne crc.
 * Ak je crc spravne vygeneruje sa ack na prijatu spravu. Nasledne data analytzuju.
 */

int main(void)
{
	komunikaciaInit();
	NACITANEDATA data;
	LedAllOff();
	MotorStop();
	DvereBrzda(0);
	while (1){
		data=LPSCIRecieveData();
		if(data.crcOk==true){
			AckGenerator(data);
			analyzujData(data);
		}
	}
}

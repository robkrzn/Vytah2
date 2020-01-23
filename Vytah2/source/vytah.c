/*
 * vytah.c
 *
 *  Created on: 17. 1. 2020
 *      Author: rober
 */
#include "vytah.h"
/**
 * @file    vytah.c
 * @brief   Subor pre vytah.c
 * @details Nachadzaju sa tu funkcie na pracu s
 * 			prvkami vytahu. Su to funkcie na ovladanie prvkov,
 * 			riadenie vytahu, vytvaranie sprav vytahu.
 */


/***************************************************************************//**
 * Funkcia motor sluzi na ovladanie rychlosti motora.
 *
 * @param rychlost ktora je nasledne prevedena na datovy paket
 ******************************************************************************/
void Motor(int rychlost){
	//NACITANEDATA sprava;
	uint8_t data[5];
	data[0]=0x02;
	memcpy(data+1,&rychlost, 4);
	//while((!sprava.crcOk)||(sprava.typSpravy!=1)||(sprava.odosielatel!=0xf1)){
		LPSCISendLongData(0xf1, 0x05, data);
	//	sprava=LPSCIRecieveData();
	//}
}
/***************************************************************************//**
 * Funkcia na vytvorenie spravy pre zastavenie chodu motora.
 ******************************************************************************/
void MotorStop(){
	LPSCISendData(0xf1,0x01,0x01);
	printf("Motor zastavuje\n");
}
/***************************************************************************//**
 * Funkcia na zapnutie led diody.
 *
 * @param led je ozacenie diody v hex tvare.
 ******************************************************************************/
void LedOn(int8_t led){
	NACITANEDATA data;
	while((!data.crcOk)||(data.typSpravy!=1)||(data.odosielatel!=led)){
		LPSCISendData(led,0x01,0x01);
		data=LPSCIRecieveData();
	}
}
/***************************************************************************//**
 * Funkcia na vypnutie led diody.
 *
 *  @param led je ozacenie diody v hex tvare.
 ******************************************************************************/
void LedOff(int8_t led){
	NACITANEDATA data;
	data.crcOk=false;
	data.typSpravy=0;
	while((!data.crcOk)||(data.typSpravy!=1)||(data.odosielatel!=led)){
		LPSCISendData(led,0x01,0x00);
		data=LPSCIRecieveData();
	}
	printf("Led %x sa vypla\n",led);

}
/***************************************************************************//**
 * Funkcia na vypnutie vsetkych dostupnych led diod.
 ******************************************************************************/
void LedAllOff(){
	for(int i=0x10;i<=0x14;i++){
		LedOff(i);
	}
	for(int i=0x20;i<=0x24;i++){
		LedOff(i);
	}
	printf("Vsetky ledky boli zhasnute\n");
}
/***************************************************************************//**
 * Funkcia na zabrzdenie bezpecnostnej brzdy.
 *
 * @param stav je pozadovany stav pricom
 * true - 1 - zodpoveda zamknutiu
 * false - 0 - zodpoveda odomknutiu
 ******************************************************************************/
void DvereBrzda(bool stav){
	NACITANEDATA data;
	if(stav){
		while((!data.crcOk)||(data.typSpravy!=1)){
			LPSCISendData(0xf,0x01,0x01);
			data=LPSCIRecieveData();
		}
	}else{
		while((!data.crcOk)||(data.typSpravy!=1)){
			LPSCISendData(0xf,0x01,0x00);
			data=LPSCIRecieveData();
		}
	}
}
/***************************************************************************//**
 * Funkcia na otvorenie / zatvorenie dveri kabiny.
 *
 * @param stav je pozadovany stav pricom
 * true - 1 - zodpoveda zavretiu
 * false - 0 - zodpoveda otvoreniu
 ******************************************************************************/
void Dvere(bool stav){
	NACITANEDATA data;
	if(stav){
		while((!data.crcOk)||(data.typSpravy!=1)||(data.odosielatel!=0xf0)){
			LPSCISendData(0xf0,0x01,0x01);
			data=LPSCIRecieveData();
		}
		printf("Dvere sa zatvaraju\n");
	}else{
		while((!data.crcOk)||(data.typSpravy!=1)||(data.odosielatel!=0xf0)){
			LPSCISendData(0xf0,0x01,0x00);
			data=LPSCIRecieveData();
		}
		printf("Dvere sa otvaraju\n");
	}
}
/***************************************************************************//**
 * Funkcia na detekovanie toho ktore tlacidlo na poschodi bolo stlacene. Po detekcii zavola pohyb kabiny.
 *
 * @param data je datova struktura s obsahom dat zo spravy
 ******************************************************************************/
void StlaceneTlacidloPoschodia(NACITANEDATA data){
	if(data.data[2]==0xc0){
		PoschodieTransport(0xe0,50,0x10);
	}else if(data.data[2]==0xc1){
		PoschodieTransport(0xe1,275,0x11);
	}else if(data.data[2]==0xc2){
		PoschodieTransport(0xe2,500,0x12);
	}else if(data.data[2]==0xc3){
		PoschodieTransport(0xe3,725,0x13);
	}else if(data.data[2]==0xc4){
		PoschodieTransport(0xe4,950,0x14);
	}
}
/***************************************************************************//**
 * Funkcia ktora sa stara o pohyb kabiny na pozadovane poschodie.
 * Ak je vytah na pozadovanom poschodi nevykona sa ziadna akcia.
 * Ak nieje na pozadovanom poschodi rozsvieti sa led dioda pozadovaneho poschodia,
 * uzavru sa dvere, a zisti sa smer aky sa ma kabina vydat. Nasledne sa rychlost
 * motora nastavi na 80 pozadovanim smerom a neustale sa citaju data ci sa uz nachadya
 * sprava od poschodia na ktore sa ma vytah vydat. Ak je takyto snimac najdeny nastavi sa rychlost
 * na 15 a ak sa najde snimac v tesnej blizskosti tak sa kabina zastavi a transport kabiny je dokonceny.
 *
 * * @param snimacPoschodia je snimac pozadovaneho poschodia
 *   @param polohaPoschodia poloha poschodia v tvare definovanom pre polohu
 *   @param LedPoschodia led dioda pozadovaneho poschodia
 ******************************************************************************/
void PoschodieTransport(uint8_t snimacPoschodia, int polohaPoschodia, uint8_t LedPoschodia){
	int pozicia=ZisitPoziciuKabiny();
	if(!(jeNaPoschodi(pozicia,polohaPoschodia))){
		LedOn(LedPoschodia);
		Dvere(1);
		delay(1000000);
		NACITANEDATA sprava;
		bool cesta=0;
		sprava.crcOk=false;
		uint8_t zapisanySnimac=0;
		int rychlost=1;
		if(pozicia>polohaPoschodia){
			rychlost=-1;
		}
		Motor(80*rychlost);
		while(!cesta){
			sprava=LPSCIRecieveData();
			if(jeSnimac(sprava)){
				if(sprava.data[2]!=zapisanySnimac){
					zapisanySnimac=sprava.data[2];
					vypisNaDisplej(rychlost, sprava.data[2]-0xe0);
				}
			}
			if(sprava.data[2]==snimacPoschodia){
				if(sprava.data[4]==0x00){
					Motor(40*rychlost);
				}
				if(sprava.data[4]==0x01){
					Motor(15*rychlost);
				}
				if(sprava.data[4]==0x02){
					MotorStop();
					cesta=true;
				}
			}
			if(sprava.data[2]==0xdf){	//pri chybe a narazeni vytahu o posledny snimac sa pusti opacnym smerom
				cesta=true;
			}
			if(sprava.data[2]==0xe5){		//pri chybe a narazeni vytahu o posledny snimac sa pusti opacnym smerom
				cesta=true;
			}
		}
		delay(10000000);
		Dvere(0);
		LedOff(LedPoschodia);
		vypisNaDisplej(3, sprava.data[2]-0xe0);
	}
	printf("Vytah je na pozadovanom podlazi\n");
}
/***************************************************************************//**
 * Funkcia na detekovanie toho ktore tlacidlo vo vytahu bolo stlacene. Po detekcii zavola pohyb kabiny.
 *
 * @param data datova struktura s obsahom dat
 ******************************************************************************/
void StlaceneTlacidloVytahu(NACITANEDATA data){
	if(data.data[2]==0xb0){
		LedOn(0x20);
		PoschodieTransport(0xe0,50,0x10);
		LedOff(0x20);
	}else if(data.data[2]==0xb1){
		LedOn(0x21);
		PoschodieTransport(0xe1,275,0x11);
		LedOff(0x21);
	}else if(data.data[2]==0xb2){
		LedOn(0x22);
		PoschodieTransport(0xe2,500,0x12);
		LedOff(0x22);
	}else if(data.data[2]==0xb3){
		LedOn(0x23);
		PoschodieTransport(0xe3,725,0x13);
		LedOff(0x23);
	}else if(data.data[2]==0xb4){
		LedOn(0x24);
		PoschodieTransport(0xe4,950,0x14);
		LedOff(0x24);
	}
}
/***************************************************************************//**
 * Funkcia na zistenie sucasnej polohy vytahu
 * Poloha je navratova hodnota sucasnej polohy kabiny
 ******************************************************************************/
signed int ZisitPoziciuKabiny(){
	LPSCISendData(0xf1, 0x01, 0x03);
	NACITANEDATA data;
	while((data.typSpravy!=0)||(!data.crcOk)){
		data=LPSCIRecieveData();
	}
	AckGenerator(data);
	signed int poloha = (data.data[5]<<8) | data.data[4];
	//printf("Poloha: %i \n",poloha);
	//vypisData(data);
	return poloha;
}
/***************************************************************************//**
 * Funkcia na zistenie sucasnej rychlosti vytahu
 * rychlost je sucasna rychlost kabiny
 ******************************************************************************/
float ZisitRychlostMotora(){
	LPSCISendData(0xf1, 0x01, 0x04);
	NACITANEDATA data;
	while((data.typSpravy!=0)||(!data.crcOk)){
		data=LPSCIRecieveData();
	}
	AckGenerator(data);
	uint8_t data3[4]={data.data[4],data.data[5],data.data[6],data.data[7]};
	float rychlost3=(float)*data3;
	printf("Sucasna rychlost: %f \n",rychlost3);
	//vypisData(data);
	return rychlost3;
}
/***************************************************************************//**
 * Funkcia ktora sluzi na vypis udajov na displej.
 *
 * @param smer pre oznacenie smeru pohybu
 * @param poschodie je cislo poschodia
 ******************************************************************************/
void vypisNaDisplej(int smer, int poschodie){
	if(smer == -1){
		smer=2;
	}
	uint8_t text[5];
	text[2]='.';
	text[3]=' ';
	text[4]='p';
	text[0]=smer;
	text[1]=poschodie+48;
	LPSCISendLongData(0x30, 0x05, text);
	printf("Vytah je na poschodi %d\n",poschodie);
}
/***************************************************************************//**
 * Funkcia pre zistenie ci precitana sprava je od snimaca polohy kabiny.
 * True ak je sprava od snimaca / false ak nie
 *
 * @param sprava je datova struktura s obsahom dat
 ******************************************************************************/
bool jeSnimac(NACITANEDATA sprava){
	if(sprava.data[2]>=0xe0 && sprava.data[2]<=0xe4){
		return true;
	}
	return false;
}
/***************************************************************************//**
 * Funkcia na zistenie ci sa kabina vytahu nachaduza na pozadovanom poschodi.
 * True - ak sa nachadza na pozadovanom poschodi / falsek ak nie
 *
 * @param poloha je informacia o sucasnej polohe zistena od vytahu
 * @param polohaPoschodia je idealna poloha poschodia.
 ******************************************************************************/
bool jeNaPoschodi(signed int poloha,int polohaPoschodia){
	if((poloha>polohaPoschodia-20)&&(poloha<polohaPoschodia+20)){
		return true;
	}
	return false;
}


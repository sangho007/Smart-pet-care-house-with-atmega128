/*
 * ex_eeprom.h
 *
 * Created: 2022-11-28 오후 4:39:02
 *  Author: sangho
 */ 


#ifndef EX_EEPROM_H_
#define EX_EEPROM_H_

#include "main.h"
#include <avr/eeprom.h>

unsigned int eeprom_address;

void EEPROM_Write(unsigned int EE_Addr, unsigned char EE_Data)
{
	while(EECR&(1<<EEWE));
	EEAR = EE_Addr;
	EEDR = EE_Data;// EEDR <- EE_Data
	cli();// Global Interrupt Disable
	EECR |= (1 << EEMWE);// SET EEMWE
	EECR |= (1 << EEWE); // SET EEWE
	sei(); // Global Interrupt Enable
}

unsigned char EEPROM_Read(unsigned int EE_addr)
{
	while(EECR&(1<<EEWE));
	EEAR = EE_addr;
	EECR |= (1 <<EERE);// SET EERE
	return EEDR; // Return Read Value
}

void EEPROM_init(){
	//EEPROM_Write(0x10,11);
	eeprom_address = EEPROM_Read(0x10);
}

#endif /* EX_EEPROM_H_ */
/*
 * humidifier.h
 *
 * Created: 2022-12-04 오후 7:46:13
 *  Author: sangho
 */ 


#ifndef HUMIDIFIER_H_
#define HUMIDIFIER_H_

#include "main.h"

unsigned char humidifier_flag;

void humidifier_init(void){
	//가습기 = PD6 , PD7
	DDRC |=(1<<6)|(1<<7);
	humidifier_flag = 0;  // off 상태
}

void humidifier_on(void){
	//push switch
	if(humidifier_flag==0){  // off 상태인 경우 
		PORTC &= ~(1<<6);
		_delay_ms(100);
		PORTC |= (1<<6); 
	}
	humidifier_flag = 1;
}

void humidifier_off(void){
	//push switch
	if(humidifier_flag==1){  // on 상태인 경우
		PORTC &= ~(1<<6);
		_delay_ms(100);
		PORTC |= (1<<6);
	}
	humidifier_flag = 0;
}



#endif /* HUMIDIFIER_H_ */
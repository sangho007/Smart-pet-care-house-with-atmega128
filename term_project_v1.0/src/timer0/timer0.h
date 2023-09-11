/*
 * timer0.h
 *
 * Created: 2022-11-28 오후 8:51:12
 *  Author: sangho
 */ 


#ifndef TIMER0_H_
#define TIMER0_H_

#include <avr/io.h>
#include <avr/interrupt.h>

int count0;
int ms_time0;

void timer0_init(void){
	count0 = 0;
	ms_time0 = 0;
	
	TCCR0 = 0;
	TCNT0 = 156;
	TCCR0 |= (1 << CS01);	// 8분주
	TIMSK |= (1 << TOIE0);	// 타이머0 오버플래그 인터럽트 허가
	sei();
}







#endif /* TIMER0_H_ */
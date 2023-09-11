/*
 * servo.h
 *
 * Created: 2022-11-28 오후 3:52:08
 *  Author: sangho
 */ 


#ifndef SERVO_H_
#define SERVO_H_

void servo_init(void){
	// PB5
	DDRB|=0x20;
	PORTB|=0x20;
	
	TCCR1A=0x82;
	TCCR1B=0x1b;
	ICR1=4999;     //TOP
	OCR1A=375;     //0도
}

void servo_move2N90(){
	OCR1A=250;     //-90도
	//_delay_ms(1000);
}

void servo_move2P90(){
	OCR1A=500;     //90도
	//_delay_ms(1000);
}

#endif /* SERVO_H_ */
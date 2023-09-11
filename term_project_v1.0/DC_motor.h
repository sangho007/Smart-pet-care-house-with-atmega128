/*
 * DC_motor.h
 *
 * Created: 2022-11-28 오후 3:41:58
 *  Author: sangho
 */ 


#ifndef DC_MOTOR_H_
#define DC_MOTOR_H_

unsigned char dc_motor_flag; // 1: 동작중, 0: 정지

void DC_motor_init(void){
	// IN1핀 = PC0, IN2핀 = PC2  
	DDRC |= (1<<0) & (1<<2);
	dc_motor_flag = 0;
}

void DC_motor_start(void){
	PORTC |= (1<<0);
	dc_motor_flag = 1;
}

void DC_motor_stop(void){
	PORTC = 0x00;
	dc_motor_flag = 0;
}

#endif /* DC_MOTOR_H_ */
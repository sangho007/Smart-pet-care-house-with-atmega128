/*
 * term_project_v1.0.c
 *
 * Created: 2022-12-01 오후 8:41:53
 * Author : sangho
 */ 

#include "main.h"

//Global variable
	
//Function


int main(void)
{	
	//Init();
	
	main_init();
	humidifier_init();
	LCD_Init();
	servo_init();
	DC_motor_init();
	USART_init();
	EEPROM_init();
	
	
	//variable
		
    while (1) 
    {	
		receive_USART();	
    }
}


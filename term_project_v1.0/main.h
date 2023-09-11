/*
 * main.h
 *
 * Created: 2022-12-02 오후 10:22:48
 *  Author: sangho
 */ 


#ifndef MAIN_H_
#define MAIN_H_

#define F_CPU 14745600UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "dht11.h"
#include "lcd.h"
#include "servo.h"
#include "ex_eeprom.h"
#include "DC_motor.h"
#include "usart.h"
#include "humidifier.h"

//Global variable
uint8_t temperature;
uint8_t humidity;

uint8_t temperature_threshold;
uint8_t humidity_threshold;

uint8_t record_mode;

char lcd_buf[40];

//function
main_init(){
	temperature = 0;
	humidity = 0;
	record_mode = 0;
	memset(lcd_buf,0,40);
	temperature_threshold = 30;
	humidity_threshold = 90;
	humidifier_off();
	
	//DDRD = 0x00;
	EIMSK = 0x0f;
	EICRA = 0xff;
}

void Display_temperature(){
	dht11_getdata(0, &temperature);
	if((temperature > 0) && (temperature <50)){
		sprintf(lcd_buf, "Temp:%d", temperature);
	}
	LCD_Pos(0,0);
	LCD_Str(lcd_buf);
	_delay_ms(1000);
}

void Display_Humidity(){
	dht11_getdata(1, &humidity);
	if((humidity> 0) && (humidity<=100)){
		sprintf(lcd_buf, "Hum:%d", humidity);
	}
	LCD_Pos(1,0);
	LCD_Str(lcd_buf);
	_delay_ms(1000);
}

void Control_Temperature(){
	if(temperature > temperature_threshold){
		if(dc_motor_flag == 0){ // 기준온도보다 높고 선풍기 off 상태일 때 
			DC_motor_start();
		}
	}
	else{
		if(dc_motor_flag == 1){ // 기준온도보다 낮고 선풍기 on 상태일 때 
			DC_motor_stop();
		}
	}
}

void Control_Humidity(){
	if(humidity < humidity_threshold){
		humidifier_on();// 기준습도보다 낮고 가습기 off 상태일 때
	}
	else{
		humidifier_off();// 기준습도보다 높고 가습기 on 상태일 때
	}
}

void EEPROM_Write_Str(unsigned char* str, unsigned int* EE_adr){
	for(int i = 0; i<strlen(str) ;i++){
		EEPROM_Write(*EE_adr,str[i]);
		(*EE_adr)++;
		EEPROM_Write(0x10,*EE_adr);
	}	
}

void EEPROM_Read_Str(unsigned int EE_adr, int length,unsigned char * str ){
	int j = 0;
	for(int i = length; i>0 ;i--){
		str[j++] = EEPROM_Read(EE_adr-i);
	}
	str[j] = 0;
}


int parsingPacket(char *recBuff, int length){ // 패킷을 파싱하는 함수
	// 패킷검사 1. 만약 시작 데이터가 정해진 데이터(0xFF)가 아닌 경우 종료.
	if(recBuff[0] != 0xFF) return -1;
	// 패킷검사 2. 장치의 이름이 1이 아닌 경우 종료.
	if(recBuff[1] != 0x01) return -2;
	// 패킷검사 3. 패킷의 체크섬을 확인한뒤 일치하지 않은 경우 종료.
	// 수신된 체크섬 데이터는 recBuff[length -1]
	// 체크섬 계산 값은 체크섬 데이터 앞까지 더하는 chksum(recBuff,length-1)
	if(recBuff[length -1] != chksum(recBuff,length-1)) return -3;
	switch(recBuff[2])
	{
		case 0x11:  // 데이터의 종류가 온습도인 경우
		if( recBuff[3] == 0x02){ // 데이터가 쓰기인 경우
			record_mode = 0;
			LCD_Clear();
			Display_temperature();
			Display_Humidity();
			Control_Temperature();
			Control_Humidity();
		}
		break;
		
		case 0x12:  // 데이터의 종류가 서보모터인 경우
		record_mode = 0;
		if(recBuff[5] == 0x01){ // 데이터가 0x01인 경우 (서보모터 -90도로 이동)
			servo_move2N90();
		}
		else if(recBuff[5] == 0x02){
			servo_move2P90();
		}
		break;
		
		case 0x13:  // 데이터의 종류가 몸무게인 경우
		if(recBuff[3] == 0x02){ /// 데이터가 쓰기인 경우
			record_mode = 0;
			LCD_Clear();
			memcpy(STR_forLCD, &recBuff[5], recBuff[4]);
			STR_forLCD[recBuff[4]] = 0;   //문자열 끝을 0으로 설정
			
			EEPROM_Write_Str(STR_forLCD,&eeprom_address);
			EEPROM_Read_Str(eeprom_address,strlen(STR_forLCD),STR_forEEPROM);
			
			LCD_Pos(0,0);
			LCD_Str(STR_forEEPROM);
			resPacket(0x13,0x02,strlen(STR_forEEPROM),STR_forEEPROM);// LCD 문자열을 lRecBuff[4] 만큼 반환
		}
		break;
		
		case 0x14:
		if(recBuff[3] == 0x01){ /// 데이터가 읽기인 경우 이전 기록 출력
			LCD_Clear();
			LCD_Pos(0,0);
			LCD_Str("Records");
			record_mode = 1;
			while(1){
				if(((PIND & (1<<5)) == 0)){		//5번 스위치 눌렀을 때 탈출
					while(((PIND & (1<<2)) <= 0));
					eeprom_address = EEPROM_Read(0x10);
					LCD_Clear();
					record_mode = 0;
					break;
				}
			}
		}
		else if(recBuff[3] == 0x02){ /// 데이터가 읽기인 경우 이전 기록 출력
			eeprom_address = EEPROM_Read(0x10);
			LCD_Clear();
		}
		break;
	}
	return 0;
}

void USART_init(void){
	DDRB |= 0xff;
	DDRD = 0x00;
	RecBuff_estLength = REC_BUFF_MAX_LENGTH;
	lRecBuffLength = 0;   // 수신패킷 길이 저장
	usart_res = 0;
	Init_USART1_IntCon();
	sei();
}

void receive_USART(void){
	if(RecFlg == 1) // 패킷 수신왈료 플래그가 설정된 경우
	{
		// 데이터의 연속 수신을 고려하여, 임시변수에 수신된 데이터 저장
		memcpy(lRecBuff, RecBuff, RecBuff_estLength);
		lRecBuffLength = RecBuff_estLength;
		
		// 임시저장 후 또다시 패킷 수신을 위한 버퍼 및 관련변수 초기화
		RecFlg = 0;
		memset(RecBuff, 0, REC_BUFF_MAX_LENGTH);
		RecBuff_estLength = REC_BUFF_MAX_LENGTH;
		RecBuffindex = 0;
		
		// 수신된 패킷을 파싱하는 함수
		usart_res = parsingPacket(lRecBuff,lRecBuffLength);
		
		if(usart_res < 0){
			// packet이 이상이 있는 경우 상태알림
		}
	}
}


//Interrupt
ISR(USART1_RX_vect){// 인터럽트 루틴에서의 데이터 수신
	RecBuff[RecBuffindex] = UDR1;
	RecBuffindex++;
	if(RecBuffindex > 4){  // 데이터 길이정보가 수신된 경우 패킷 길이 갱신
		RecBuff_estLength = (RecBuff[4] + 6); // 패킷길이는 데이터 길이+6
	}
	if(RecBuffindex == RecBuff_estLength){          // 수신된 데이터의 순서가 패킷 길이와 같으면 패킷 수신완료
		RecFlg = 1; // 수신 완료 플래그 활성화
	}
}

ISR(INT0_vect){
	if(record_mode){
		EEPROM_Read_Str(eeprom_address,0x0c,STR_forEEPROM);
		LCD_Pos(1,0);
		LCD_Str(STR_forEEPROM);
		if (eeprom_address > (0x0c+0x10)){
			eeprom_address -= 0x0c;
		}
	}
}

ISR(INT1_vect){
	if(record_mode){
		EEPROM_Read_Str(eeprom_address,0x0c,STR_forEEPROM);
		LCD_Pos(1,0);
		LCD_Str(STR_forEEPROM);
		if (eeprom_address < EEPROM_Read(0x10) - 0x0c){
			eeprom_address += 0x0c;
		}
	}
}

ISR(INT2_vect){
	if(dc_motor_flag){
		DC_motor_stop();
	}else{
		DC_motor_start();
	}
}

ISR(INT3_vect){
	if(humidifier_flag){
		humidifier_off();
	}else{
		
		humidifier_on();
	}
}

#endif /* MAIN_H_ */
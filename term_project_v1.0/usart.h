/*
 * usart.h
 *
 * Created: 2022-11-28 오후 4:10:14
 *  Author: sangho
 */ 


#ifndef USART_H_
#define USART_H_

#include "main.h"

#define REC_BUFF_MAX_LENGTH 100

unsigned char RecBuff[REC_BUFF_MAX_LENGTH];
unsigned char RecBuffindex;
unsigned char RecFlg;
unsigned char RecBuff_estLength;

unsigned char STR_forLCD[20]; // LCD 출력 문자 저장 위함
unsigned char STR_forEEPROM[20];

unsigned char lRecBuff[100];        // 수신패킷 임시저장용 배열
unsigned char lRecBuffLength;   // 수신패킷 길이 저장
int usart_res;

void putch_USART1(char data) {
	while( !(UCSR1A & (1<<UDRE1)));
	UDR1 = data;
}
void puts_USART1(char *str){  // 문자열 출력 루틴
	while( *str != 0){  // 문자의 마지막에는 ‘\0’이 들어가 있으므로
		putch_USART1(*str);  // ‘\0’이 나올 때까지 출력한다.
		str++;
	}
}
void sendBuff_USART1(char *str,int length){  // 문자열 출력 루틴
	while (length--){
		putch_USART1(*str);  // ‘\0’이 나올 때까지 출력한다.
		str++;
	}
}
void Init_USART1_IntCon(void){
	// ① RXCIE1=1(수신 인터럽트 허가), RXEN0=1(수신 허가), TXEN0 = 1(송신 허가)
	UCSR1B = (1<<RXCIE1)| (1<<RXEN1) | (1 <<TXEN1);
	UBRR1H = 0x00; // ② 57600bps 보오레이트 설정
	UBRR1L = 0x0F; // ② 57600bps 보오레이트 설정
	sei(); // ③ 인터럽트 동작 시작(전체 인터럽트 허가)
}

unsigned char chksum(char *buff,int length){
	unsigned char sum = 0;
	int i = 0;
	for(i=0 ; i<length ; i++){
		sum += buff[i];
	}
	return sum;
}
void resPacket(char type, char rw, unsigned char length, unsigned char *resData){
	unsigned char rescBuff[REC_BUFF_MAX_LENGTH];
	unsigned char resBuffLength = 0;
	int i=0;
	rescBuff[resBuffLength++] = 0xff;
	rescBuff[resBuffLength++] = 0x01;
	rescBuff[resBuffLength++] = type;
	rescBuff[resBuffLength++] = rw;
	rescBuff[resBuffLength++] = length;
	for(i = 0 ; i<length ; i++){
		rescBuff[resBuffLength++] = resData[i];
	}
	rescBuff[resBuffLength] = chksum(rescBuff,resBuffLength);
	resBuffLength++;
	
	sendBuff_USART1(rescBuff,resBuffLength);
}



#endif /* USART_H_ */
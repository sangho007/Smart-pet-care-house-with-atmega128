/*
 * dht11.h
 *
 * Created: 2021-02-05 오후 2:53:57
 *  Author: User
 */ 


#ifndef DHT11_H_
#define DHT11_H_

#define DHT11_ERROR 255

#define DHT11_DDR DDRF
#define DHT11_PORT PORTF
#define DHT11_PIN PINF
#define DHT11_INPUTPIN (PINF & (1 << 1))

void dht11_getdata(uint8_t num, uint8_t *data);
uint8_t getdata(uint8_t select);


#endif /* DHT11_H_ */
#ifndef __TEMP_H_
#define __TEMP_H_

#include<reg52.h>
//---�ض���ؼ���---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

sbit Data=P0^2;   //����������


//--����ȫ�ֺ���--//
void DHT11_delay_us(uchar n);
void DHT11_delay_ms(uint z);
void DHT11_start();
uchar DHT11_rec_byte();
void DHT11_receive() ;


#endif

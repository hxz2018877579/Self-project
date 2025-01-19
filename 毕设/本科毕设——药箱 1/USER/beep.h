#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"




#define BEEP PAout(4)	// BEEP,蜂鸣器接口		   

void BEEP_Init(void);	//初始化
void scan_QR_codes_tone(void);
void alert(void);		 				    
#endif
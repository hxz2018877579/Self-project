#ifndef __BEEP_H
#define __BEEP_H	 
#include "sys.h"




#define BEEP PAout(4)	// BEEP,�������ӿ�		   

void BEEP_Init(void);	//��ʼ��
void scan_QR_codes_tone(void);
void alert(void);		 				    
#endif
#include "reg52.h" //���ļ��ж����˵�Ƭ����һЩ���⹦�ܼĴ���
#include "temp.h"
#include <stdio.h> //����sprintf����
#include <OLED.h>


typedef unsigned int u16; //���������ͽ�����������

sbit led0 = P2 ^ 0;
sbit led1 = P2 ^ 1;
sbit led2 = P2 ^ 2;
sbit led3 = P2 ^ 3;
sbit led4 = P2 ^ 4;
sbit led5 = P2 ^ 5;
sbit led6 = P2 ^ 6;
sbit led7 = P2^7;
sbit Rst = P0 ^ 3;
sbit jidianqi1 = P1^1;
sbit jidianqi2 = P0 ^ 1;
sbit DC_Motor=P1^0;
uchar Receive, i;
uchar Recive_table[35];    //���ջ�����
uint USART_R = 0, n, sign; //������ɱ�־λ���¶ȼ����м���������״̬��־λ
uchar tabT[4] = {0};       //�¶Ȼ��浥Ԫ
uchar tabH[4] = {0};       //ʪ�Ȼ��浥Ԫ
uchar temp[2] = {0};       //��������
uchar temp1[2] = {0};      //��������
uchar Tem, Hum;
u8 autoctrl = 0;
u8 max_Hum = 90;
u8 max_Tem = 30;
u8 times = 0,move;
u8 count = 0;
uint counter=0;
u8 wait_times = 0;
uchar hour=18,min=30,sec=00;	
unsigned int Timer2_Server_Count;
void delay_10us(u16 ten_us)
{
	while(ten_us--);	
}
void ms_delay(uint t) //��ʱ1ms
{
    uint i, j;
    for (i = t; i > 0; i--)
        for (j = 110; j > 0; j--)
            ;
}

void us_delay(uchar t) //��ʱ1us
{
    while (t--)
        ;
}
/********************************************************************
���ƣ����ڳ�ʼ��
********************************************************************/
void Uart_Init() //ʹ�ö�ʱ��1��Ϊ�����ʷ�����
{
    TMOD &= 0x0F; //ģʽ1
    TMOD |= 0x20;

    SCON = 0x50; //���ô��з�ʽ
    TH1 = 0xFD;  //������9600
    TL1 = TH1;
    PCON = 0x00;
    PS = 1; //���ж����ȼ�

    EA = 1;  //�����ж�
    ES = 1;  //�������ж�
    TR1 = 1; //������ʱ��1
}

/********************************************************************
��ʱ��0��ʼ��
********************************************************************/

void Time0_Init() //��ʱ��0��ʼ��
{
    TMOD &= 0xF0;
    TMOD &= 0xF0; //ģʽ1
    TMOD |= 0x01;

    TH0 = 0xDC; //����ʱ������ֵ����ʱ10ms
    TL0 = 0x00;
    ET0 = 1; //�򿪶�ʱ��0�ж�����
    EA = 1;  //�����ж�
}


/********************************************************************
���ƣ����ڷ��ͺ���  ���ܣ�MCU������WIFIģ��ESP8266��������
********************************************************************/
void Send_Uart(uchar value)
{
    ES = 0;       //�رմ����ж�
    TI = 0;       //�巢������ж������־λ
    SBUF = value; //����
    while (TI == 0)
        ;   //�ȴ��������
    TI = 0; //�巢������ж������־λ
    ES = 1; //�������ж�
}
/********************************************************************
���ƣ�WIFIģ�����ú���  ����: ����ģ�飬�Ա����ʵ�����߽���Ϳ���
********************************************************************/

void ESP8266_Set(uchar *puf) // ����ָ��*pufָ���ַ�������
{
    while (*puf != '\0')
    {
        Send_Uart(*puf); //��WIFIģ�鷢�Ϳ���ָ�
        us_delay(5);
        puf++;
    }
    us_delay(5);
    Send_Uart('\r'); //�س�
    us_delay(5);
    Send_Uart('\n'); //����
}
/********************************************************************
���ƣ�WIFIģ�����ú���  ����: ����ģ�飬�Ա����ʵ�����߽���Ϳ���
********************************************************************/
void WIFI_Send(uchar *puf) // ����ָ��*pufָ���ַ�������
{
    while (*puf != '\0')
    {
        Send_Uart(*puf); //��������ź�
        us_delay(5);
        puf++;
    }
}

/********************************************************************
���ƣ�TCP���ӱ���
********************************************************************/
void Tcp_Connect()
{
    Send_Uart(0x10);   //�̶���ͷ
    Send_Uart(0x70);   //ʣ�೤��=0X10���ͻ���ID����+�û�������+���볤�ȵ�16����
    Send_Uart(0x00);   //�ɱ䱨ͷ
    Send_Uart(0x04);   //�ɱ䱨ͷ
    WIFI_Send("MQTT"); //�ɱ䱨ͷ
    Send_Uart(0x04);   //�ɱ䱨ͷ
    Send_Uart(0xC2);   //�ɱ䱨ͷ
    Send_Uart(0x00);   //�ɱ䱨ͷ
    Send_Uart(0x64);   //�ɱ䱨ͷ
    Send_Uart(0x00);   //�ͻ���ID����00 28	��40�ֽ�
    Send_Uart(0x27);
    WIFI_Send("esp10|securemode=3,signmethod=hmacsha1|"); //�ͻ���IDesp10|securemode=3,signmethod=hmacsha1|
    Send_Uart(0x00);                                      //�û�������00 12 ��18�ֽ�
    Send_Uart(0x11);
    WIFI_Send("esp10&a1ZqmIXSuSi"); //�û���esp10&a1ZqmIXSuSi
    Send_Uart(0x00);                //���볤��00 28	 ��40�ֽ�
    Send_Uart(0x28);
    WIFI_Send("ce35bf80a1b30a4aa6f944afabe63e791c91ce29"); //����
}
/********************************************************************
���ƣ��ϱ��¶�
********************************************************************/
void Up_Report()
{
    DHT11_receive();
    tabT[0] = Tem / 100 + 0x30;
    tabT[1] = Tem / 10 % 10 + 0x30;
    tabT[2] = Tem % 10 + 0x30;
    tabH[0] = Hum / 100 + 0x30;
    tabH[1] = Hum / 10 % 10 + 0x30;
    tabH[2] = Hum % 10 + 0x30;
    Send_Uart(0x30); //�̶���ͷ
    Send_Uart(0x9A); //ʣ�೤��
    Send_Uart(0x01);

    Send_Uart(0x00); //�ɱ䱨ͷ
    Send_Uart(0x30);
    WIFI_Send("/sys/a1ZqmIXSuSi/esp10/thing/event/property/post");
    WIFI_Send("{\"method\":\"thing.event.property.post\",\"id\":\"395041230\",\"params\":{\"");
    WIFI_Send("Tem"); //��ʶ��Tem
    WIFI_Send("\":");
    WIFI_Send(&tabT);
    WIFI_Send(",\"Hum"); //��ʶ��Hum
    WIFI_Send("\":");
    WIFI_Send(&tabH);
    WIFI_Send("},\"version\":\"1.0.0\"}");
}

/*******************************************************************************
 * �� �� ��       : main
 * ��������		 : ������
 * ��    ��       : ��
 * ��    ��    	 : ��
 *******************************************************************************/
void main()
{
	
    Time0_Init();
    TR0 = 0; //��ʼ��������
RCAP2H=0x0B;    //���ʱ����RCAP2H��װ��TH2
	RCAP2L=0xDC;    //���ʱ����RCAP2L��װ��TL2
	TR2=1;          //����T2����
	ET2=1;		    //����T2�ж�
	EA=1;	  
    ms_delay(1000);
    Uart_Init();
    sign = 1;
		OLED_Init();										//��ʼ��OLED  
	OLED_Clear();										//�����Ļ
    // TR0 = 1; //�򿪶�ʱ��0
    while (1)
    {   		OLED_ShowNum(78,0,hour/10,1,8);	//Сʱʮλ
				OLED_ShowNum(84,0,hour%10,1,8);	//Сʱʮλ
				OLED_ShowString(90,0,":",8);		//Сʱ�����ð��
				OLED_ShowNum(96,0,min/10,1,8);	//����ʮλ
				OLED_ShowNum(102,0,min%10,1,8);	//���Ӹ�λ
				OLED_ShowString(108,0,":",8);		//���Ӻ����ð��
				OLED_ShowNum(114,0,sec/10,1,8);	//����ʮλ		
				OLED_ShowNum(120,0,sec%10,1,8);	//���Ӹ�λ}
         OLED_ShowCHinese(0,0,6);	
			 OLED_ShowCHinese(15,0,7);	
			 OLED_ShowCHinese(30,0,8);	
			 OLED_ShowCHinese(45,0,9);	
			 OLED_ShowCHinese(60,0,10);	
				
        if (sign == 1) // sign=1����ʾ��Ҫ�������ӷ�����
        {
            TR0 = 0;
            Rst = 0;       //����esp8266ģ��
            ms_delay(100); //����esp8266ģ��
            Rst = 1;       //����esp8266ģ��
            ms_delay(2000);
            n = 18;
            ESP8266_Set("ATE0");                                     //ȡ���ش�
            while (Recive_table[0] != 'O' || Recive_table[1] != 'K') //�ȴ�esp8266ģ�鷵��ok
            {
                led1 = 0; // led1��
                ms_delay(100);
            }
            led1 = 1;            // led1��
            Recive_table[0] = 0; //������ջ�����
            Recive_table[1] = 0;

            ESP8266_Set("AT+CWMODE=1");                              //�ͻ���ģʽ����Ϊclintʹ��
            while (Recive_table[0] != 'O' || Recive_table[1] != 'K') //�ȴ�esp8266ģ�鷵��ok
            {
                led2 = 0; // led2��
                ms_delay(100);
            }
            led2 = 1;            // led1��
            Recive_table[0] = 0; //������ջ�����
            Recive_table[1] = 0;
            ESP8266_Set("AT+CWJAP=\"WIFI\",\"00000000\"");           //����WiFi
            while (Recive_table[0] != 'O' || Recive_table[1] != 'K') //�ȴ�esp8266ģ�鷵��ok
            {
                led3 = 0; // led3��
                ms_delay(100);
            }
            led3 = 1;            // led3��
            Recive_table[0] = 0; //������ջ�����
            Recive_table[1] = 0;
            ESP8266_Set("AT+CIPSTART=\"TCP\",\"a1ZqmIXSuSi.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883"); //����tcp������
            while (Recive_table[0] != 'O' || Recive_table[1] != 'K')                                      //�ȴ�esp8266ģ�鷵��ok
            {
                led4 = 0; // led4��
                ms_delay(100);
            }
            led4 = 1;            // led4��
            Recive_table[0] = 0; //������ջ�����
            Recive_table[1] = 0;
            ESP8266_Set("AT+CIPMODE=1");                             //����͸��ģʽ
            while (Recive_table[0] != 'O' || Recive_table[1] != 'K') //�ȴ�esp8266ģ�鷵��ok
            {
             
                ms_delay(100);
            }
                      
            ESP8266_Set("AT+CIPSEND");     //����͸��ģʽ
            while (Recive_table[0] != '>') //�ȴ�esp8266ģ�鷵��ok
            {
                led6 = 0; // led6��
                ms_delay(100);
            }
            led6 = 1; // led6��

            Tcp_Connect(); //�������ӱ���
            ms_delay(100);
            TR0 = 1; //�򿪶�ʱ��0
						
            led7 = 0;
            sign = 0; //���ӳɹ���sign��0
        }

        if (sign == 0) // sign=0��ʾ�����������жϽ��ܿ���ָ��
        {
           
            ms_delay(100);
            if (times == 1)
            {

                TR0 = 0; //�رն�ʱ��
						 
                Up_Report(); //�ϴ��¶�
                times = 0;
							if (move==1)
             {				 DC_Motor=1;				
							OLED_ShowCHinese(50,2,0);				//Ͷ	�ӵ�0�У�ռ��0-15����16�У���   ��0ҳ��ռ0-1ҳ��0-16�����أ���Hzk[0]:��Ͷ��	
	              OLED_ShowCHinese(66,2,1);				
	           		OLED_ShowCHinese(82 ,2,2);
							counter++;
						 }
						 if (move==0)
{	 DC_Motor=0;	
                OLED_ShowCHinese(50,2,3);				//Ͷ	�ӵ�0�У�ռ��0-15����16�У���   ��1ҳ��ռ0-1ҳ��0-16�����أ���Hzk[0]:��Ͷ��	
	              OLED_ShowCHinese(66,2,4);				
	              OLED_ShowCHinese(82 ,2,5);	
OLED_ShowNum(90,2,counter,6,8);
	
}
                if (autoctrl == 0) // �Զ�����
                {
                    if (Hum >= max_Hum)
                    {

                     
	                           	led5=0;
	                                   	
                    }
                 
                   
                 if (Tem >= max_Tem)
                    {

                      
	                        
		                             	led5=0;
	                                  
                    }
										if (Tem <max_Tem&&Hum < max_Hum)
										{
										led5=1;
										}
                  
                    temp[0] = ~DC_Motor;
                    temp[0] = temp[0] + 48;
                    temp1[0] = ~led5;
                    temp1[0] = temp1[0] + 48;
                    Send_Uart(0x30);
                    Send_Uart(0x92);
                    Send_Uart(0x01);
                    Send_Uart(0x00);
                    Send_Uart(0x30);
                    WIFI_Send("/sys/a1ZqmIXSuSi/esp10/thing/event/property/post");
                    WIFI_Send("{\"method\":\"thing.event.property.post\",\"id\":\"395041230\",\"params\":{\"");
                    WIFI_Send("M");
                    WIFI_Send("\":");
                    WIFI_Send(&temp); // bool
                    WIFI_Send(",\"N");
                    WIFI_Send("\":");
                    WIFI_Send(&temp1); // bool
                    WIFI_Send("},\"version\":\"1.0.0\"}");
                }
                TR0 = 1; //�رն�ʱ��
            }


            if (USART_R == 1)//Ӧ�������APP�ϻ��ֶ����Զ�ģʽ
            {
                TR0 = 0;
                if (Recive_table[2] == 'T' && ((Recive_table[5] == '1') || (Recive_table[5] == '0')))
                {
                    if (autoctrl == 0)
                    {
                        autoctrl = 1;
                    }
                    else
                    {
                        autoctrl = 0;
                    }
                    temp[0] = autoctrl + 48;
                    Send_Uart(0x30);
                    Send_Uart(0x8C);
                    Send_Uart(0x01);
                    Send_Uart(0x00);
                    Send_Uart(0x30);
                    WIFI_Send("/sys/a1ZqmIXSuSi/esp10/thing/event/property/post");
                    WIFI_Send("{\"method\":\"thing.event.property.post\",\"id\":\"395041230\",\"params\":{\"");
                    WIFI_Send("D");
                    WIFI_Send("\":");
                    WIFI_Send(&temp); // bool
                    WIFI_Send("},\"version\":\"1.0.0\"}");
                    count = 0;
                }

                if (autoctrl == 1) // �ֶ�����
                {

                    if (Recive_table[2] == 'A' && ((Recive_table[5] == '1') || (Recive_table[5] == '0')))
                    {
                        if (DC_Motor == 0)
                        {
                            DC_Motor = 1;
                        }
                        else
                        {
                            DC_Motor = 0;
                        }
                        temp[0] = ~DC_Motor;
                        temp[0] = temp[0] + 48;
                        Send_Uart(0x30);
                        Send_Uart(0x8C);
                        Send_Uart(0x01);
                        Send_Uart(0x00);
                        Send_Uart(0x30);
                        WIFI_Send("/sys/a1ZqmIXSuSi/esp10/thing/event/property/post");
                        WIFI_Send("{\"method\":\"thing.event.property.post\",\"id\":\"395041230\",\"params\":{\"");
                        WIFI_Send("M");
                        WIFI_Send("\":");
                        WIFI_Send(&temp); // bool
                        WIFI_Send("},\"version\":\"1.0.0\"}");
                        count = 0;
                    }

                    if (Recive_table[2] == 'B' && ((Recive_table[5] == '1') || (Recive_table[5] == '0')))
                    {
                        if (jidianqi2 == 0)
                        {
                            jidianqi2 = 1;
                        }
                        else
                        {
                            jidianqi2 = 0;
                        }
                        temp[0] = ~jidianqi2;
                        temp[0] = temp[0] + 48;

                        Send_Uart(0x30);
                        Send_Uart(0x8C);
                        Send_Uart(0x01);
                        Send_Uart(0x00);
                        Send_Uart(0x30);
                        WIFI_Send("/sys/a1ZqmIXSuSi/esp10/thing/event/property/post");
                        WIFI_Send("{\"method\":\"thing.event.property.post\",\"id\":\"395041230\",\"params\":{\"");
                        WIFI_Send("N");
                        WIFI_Send("\":");
                        WIFI_Send(&temp); // bool?
                        WIFI_Send("},\"version\":\"1.0.0\"}");
                        count = 0;
                    }
                }

                TR0 = 1;                  //�򿪶�ʱ��0
                USART_R = 0;              //������ձ�־λ
                for (i = 0; i++; i <= 35) //������ջ�����
                {
                    Recive_table[i] = 0;
                }
            }
        }

        if (sign == 2) // sign=2���Է����������ܷ�����ͨѶ
        {
           
                 led6 = 0;
            Send_Uart(0xC0); //����������
            Send_Uart(0x00);
            Send_Uart(0xC0);
            Send_Uart(0x00);

            Send_Uart(0xC0); //����������
            Send_Uart(0x00);
            Send_Uart(0xC0);
            Send_Uart(0x00);
            ms_delay(500);
            wait_times = 200;
            while (--wait_times)
            {
                TR0 = 0;

                ms_delay(10);
                if (Recive_table[0] == 0xD0) //�յ��������ظ�0xD0��ʾ��������������sign=1��������tcp������
                {
                    led6 = 1;
                    sign = 0;
                    count = 0;
                    i = 0;
                    Recive_table[0] = 0;
                    TR0 = 1;
                    break;
                }
            }
            if (wait_times <= 0)
            {
                sign = 1;
                i = 0;
                Recive_table[0] = 0;
            }
        }
OLED_ShowString(60,4,"Tem",16); //�ӵ�71�����ء���4ҳ����32�����أ���ʼ��ʾ��СΪ8*16���ص��ַ���"Ab123%"
OLED_ShowString(60,6,"Hum",16); 
OLED_ShowNum(75,4,Tem,6,8);	
OLED_ShowNum(75,6,Hum,6,8);	
	
	
    }
}

void Uart_Interrupt() interrupt 4 //���ڽ����жϺ���
{
    static uchar i = 0;
    if (RI == 1)
    {
        ET0 = 0;
        RI = 0;
        Receive = SBUF;
        Recive_table[i] = Receive;
        if (Recive_table[i] == '{' || Recive_table[i] == 'p' || Recive_table[i] == 0xD0)
        {
            i = 0;
            Recive_table[i] = Receive;
        }
        if (Recive_table[i] == '\n' || (Recive_table[i] == '}' && Recive_table[i - 1] == '\"') || (Recive_table[i - 1] == 0xD0 && Recive_table[i] == 0x00)) //���յ�{...��}ʱ�������
        {
            i = 0;
            USART_R = 1;
        }
        else
        {
            i++;
            USART_R = 0;
        }
        if (i >= 34)
        {
            i = 0;
            USART_R = 0;
            for (i = 0; i++; i <= 35) //������ջ�����
            {
                Recive_table[i] = 0;
            }
        }
        ET0 = 1;
    }
    else
    {
        TI = 0;
    }
}
void Timer0() interrupt 1 //��ʱ��0����ж�
{
    static uint m = 0;
    TH0 = 0XDC; //����ʱ������ֵ����ʱ10ms
    TL0 = 0X00;
    m++;

    if (m >= 300) // 3�봫һ���¶�����
    {
        m = 0; //����жϼ���
        count++;
        times = 1;
        move=0;
 led7 = ~led7; // led��ָʾ״̬
    }
 if (count%5==0)
 {
  move=1;
 }
    if (count >= 10) // �������
    {  
        TR0 = 0;
        count = 0;
        sign = 2;
    }
}


 
void timer2() interrupt 5{
	TF2=0;        //����жϱ�־λ
Timer2_Server_Count++;

if(Timer2_Server_Count==16)                    // T2��ʱ����Ԥװ��ֵΪ0x0BDC�����16�ξ���1���ӡ�

{


sec++;					//��ʱ��50��countΪ1��
	if(sec == 60) 									//60��1����
	{
		sec = 0;
		min++;
	}
	if(min == 60)										//60����1Сʱ
	{
		min = 0;
		hour++;
	}
	if(hour == 24)									//24Сʱ��
	{
		hour = 0;
	}	
           Timer2_Server_Count=0; 

}

}

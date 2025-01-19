#include "reg52.h" //此文件中定义了单片机的一些特殊功能寄存器
#include "temp.h"
#include <stdio.h> //包含sprintf函数
#include <OLED.h>


typedef unsigned int u16; //对数据类型进行声明定义

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
uchar Recive_table[35];    //接收缓存区
uint USART_R = 0, n, sign; //接收完成标志位、温度计算中间量、运行状态标志位
uchar tabT[4] = {0};       //温度缓存单元
uchar tabH[4] = {0};       //湿度缓存单元
uchar temp[2] = {0};       //缓存数组
uchar temp1[2] = {0};      //缓存数组
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
void ms_delay(uint t) //延时1ms
{
    uint i, j;
    for (i = t; i > 0; i--)
        for (j = 110; j > 0; j--)
            ;
}

void us_delay(uchar t) //延时1us
{
    while (t--)
        ;
}
/********************************************************************
名称：串口初始化
********************************************************************/
void Uart_Init() //使用定时器1作为波特率发生器
{
    TMOD &= 0x0F; //模式1
    TMOD |= 0x20;

    SCON = 0x50; //设置串行方式
    TH1 = 0xFD;  //波特率9600
    TL1 = TH1;
    PCON = 0x00;
    PS = 1; //高中断优先级

    EA = 1;  //打开总中断
    ES = 1;  //开串口中断
    TR1 = 1; //启动定时器1
}

/********************************************************************
定时器0初始化
********************************************************************/

void Time0_Init() //定时器0初始化
{
    TMOD &= 0xF0;
    TMOD &= 0xF0; //模式1
    TMOD |= 0x01;

    TH0 = 0xDC; //给定时器赋初值，定时10ms
    TL0 = 0x00;
    ET0 = 1; //打开定时器0中断允许
    EA = 1;  //打开总中断
}


/********************************************************************
名称：串口发送函数  功能：MCU向无线WIFI模块ESP8266发送数据
********************************************************************/
void Send_Uart(uchar value)
{
    ES = 0;       //关闭串口中断
    TI = 0;       //清发送完毕中断请求标志位
    SBUF = value; //发送
    while (TI == 0)
        ;   //等待发送完毕
    TI = 0; //清发送完毕中断请求标志位
    ES = 1; //允许串口中断
}
/********************************************************************
名称：WIFI模块设置函数  作用: 启动模块，以便可以实现无线接入和控制
********************************************************************/

void ESP8266_Set(uchar *puf) // 数组指针*puf指向字符串数组
{
    while (*puf != '\0')
    {
        Send_Uart(*puf); //向WIFI模块发送控制指令。
        us_delay(5);
        puf++;
    }
    us_delay(5);
    Send_Uart('\r'); //回车
    us_delay(5);
    Send_Uart('\n'); //换行
}
/********************************************************************
名称：WIFI模块设置函数  作用: 启动模块，以便可以实现无线接入和控制
********************************************************************/
void WIFI_Send(uchar *puf) // 数组指针*puf指向字符串数组
{
    while (*puf != '\0')
    {
        Send_Uart(*puf); //串口输出信号
        us_delay(5);
        puf++;
    }
}

/********************************************************************
名称：TCP连接报文
********************************************************************/
void Tcp_Connect()
{
    Send_Uart(0x10);   //固定报头
    Send_Uart(0x70);   //剩余长度=0X10＋客户端ID长度+用户名长度+密码长度的16进制
    Send_Uart(0x00);   //可变报头
    Send_Uart(0x04);   //可变报头
    WIFI_Send("MQTT"); //可变报头
    Send_Uart(0x04);   //可变报头
    Send_Uart(0xC2);   //可变报头
    Send_Uart(0x00);   //可变报头
    Send_Uart(0x64);   //可变报头
    Send_Uart(0x00);   //客户端ID长度00 28	即40字节
    Send_Uart(0x27);
    WIFI_Send("esp10|securemode=3,signmethod=hmacsha1|"); //客户端IDesp10|securemode=3,signmethod=hmacsha1|
    Send_Uart(0x00);                                      //用户名长度00 12 即18字节
    Send_Uart(0x11);
    WIFI_Send("esp10&a1ZqmIXSuSi"); //用户名esp10&a1ZqmIXSuSi
    Send_Uart(0x00);                //密码长度00 28	 即40字节
    Send_Uart(0x28);
    WIFI_Send("ce35bf80a1b30a4aa6f944afabe63e791c91ce29"); //密码
}
/********************************************************************
名称：上报温度
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
    Send_Uart(0x30); //固定报头
    Send_Uart(0x9A); //剩余长度
    Send_Uart(0x01);

    Send_Uart(0x00); //可变报头
    Send_Uart(0x30);
    WIFI_Send("/sys/a1ZqmIXSuSi/esp10/thing/event/property/post");
    WIFI_Send("{\"method\":\"thing.event.property.post\",\"id\":\"395041230\",\"params\":{\"");
    WIFI_Send("Tem"); //标识符Tem
    WIFI_Send("\":");
    WIFI_Send(&tabT);
    WIFI_Send(",\"Hum"); //标识符Hum
    WIFI_Send("\":");
    WIFI_Send(&tabH);
    WIFI_Send("},\"version\":\"1.0.0\"}");
}

/*******************************************************************************
 * 函 数 名       : main
 * 函数功能		 : 主函数
 * 输    入       : 无
 * 输    出    	 : 无
 *******************************************************************************/
void main()
{
	
    Time0_Init();
    TR0 = 0; //初始化但不打开
RCAP2H=0x0B;    //溢出时，将RCAP2H重装入TH2
	RCAP2L=0xDC;    //溢出时，将RCAP2L重装入TL2
	TR2=1;          //允许T2工作
	ET2=1;		    //允许T2中断
	EA=1;	  
    ms_delay(1000);
    Uart_Init();
    sign = 1;
		OLED_Init();										//初始化OLED  
	OLED_Clear();										//清空屏幕
    // TR0 = 1; //打开定时器0
    while (1)
    {   		OLED_ShowNum(78,0,hour/10,1,8);	//小时十位
				OLED_ShowNum(84,0,hour%10,1,8);	//小时十位
				OLED_ShowString(90,0,":",8);		//小时后面的冒号
				OLED_ShowNum(96,0,min/10,1,8);	//分钟十位
				OLED_ShowNum(102,0,min%10,1,8);	//分钟个位
				OLED_ShowString(108,0,":",8);		//分钟后面的冒号
				OLED_ShowNum(114,0,sec/10,1,8);	//秒钟十位		
				OLED_ShowNum(120,0,sec%10,1,8);	//秒钟个位}
         OLED_ShowCHinese(0,0,6);	
			 OLED_ShowCHinese(15,0,7);	
			 OLED_ShowCHinese(30,0,8);	
			 OLED_ShowCHinese(45,0,9);	
			 OLED_ShowCHinese(60,0,10);	
				
        if (sign == 1) // sign=1，表示需要重新连接服务器
        {
            TR0 = 0;
            Rst = 0;       //重启esp8266模块
            ms_delay(100); //重启esp8266模块
            Rst = 1;       //重启esp8266模块
            ms_delay(2000);
            n = 18;
            ESP8266_Set("ATE0");                                     //取消回传
            while (Recive_table[0] != 'O' || Recive_table[1] != 'K') //等待esp8266模块返回ok
            {
                led1 = 0; // led1亮
                ms_delay(100);
            }
            led1 = 1;            // led1灭
            Recive_table[0] = 0; //清除接收缓存区
            Recive_table[1] = 0;

            ESP8266_Set("AT+CWMODE=1");                              //客户端模式，作为clint使用
            while (Recive_table[0] != 'O' || Recive_table[1] != 'K') //等待esp8266模块返回ok
            {
                led2 = 0; // led2亮
                ms_delay(100);
            }
            led2 = 1;            // led1灭
            Recive_table[0] = 0; //清除接收缓存区
            Recive_table[1] = 0;
            ESP8266_Set("AT+CWJAP=\"WIFI\",\"00000000\"");           //连接WiFi
            while (Recive_table[0] != 'O' || Recive_table[1] != 'K') //等待esp8266模块返回ok
            {
                led3 = 0; // led3亮
                ms_delay(100);
            }
            led3 = 1;            // led3灭
            Recive_table[0] = 0; //清除接收缓存区
            Recive_table[1] = 0;
            ESP8266_Set("AT+CIPSTART=\"TCP\",\"a1ZqmIXSuSi.iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883"); //连接tcp服务器
            while (Recive_table[0] != 'O' || Recive_table[1] != 'K')                                      //等待esp8266模块返回ok
            {
                led4 = 0; // led4亮
                ms_delay(100);
            }
            led4 = 1;            // led4灭
            Recive_table[0] = 0; //清除接收缓存区
            Recive_table[1] = 0;
            ESP8266_Set("AT+CIPMODE=1");                             //设置透传模式
            while (Recive_table[0] != 'O' || Recive_table[1] != 'K') //等待esp8266模块返回ok
            {
             
                ms_delay(100);
            }
                      
            ESP8266_Set("AT+CIPSEND");     //进入透传模式
            while (Recive_table[0] != '>') //等待esp8266模块返回ok
            {
                led6 = 0; // led6亮
                ms_delay(100);
            }
            led6 = 1; // led6灭

            Tcp_Connect(); //发送连接报文
            ms_delay(100);
            TR0 = 1; //打开定时器0
						
            led7 = 0;
            sign = 0; //连接成功，sign置0
        }

        if (sign == 0) // sign=0表示连接正常，判断接受控制指令
        {
           
            ms_delay(100);
            if (times == 1)
            {

                TR0 = 0; //关闭定时器
						 
                Up_Report(); //上传温度
                times = 0;
							if (move==1)
             {				 DC_Motor=1;				
							OLED_ShowCHinese(50,2,0);				//投	从第0列（占用0-15，共16列）、   第0页（占0-1页：0-16行像素）、Hzk[0]:‘投’	
	              OLED_ShowCHinese(66,2,1);				
	           		OLED_ShowCHinese(82 ,2,2);
							counter++;
						 }
						 if (move==0)
{	 DC_Motor=0;	
                OLED_ShowCHinese(50,2,3);				//投	从第0列（占用0-15，共16列）、   第1页（占0-1页：0-16行像素）、Hzk[0]:‘投’	
	              OLED_ShowCHinese(66,2,4);				
	              OLED_ShowCHinese(82 ,2,5);	
OLED_ShowNum(90,2,counter,6,8);
	
}
                if (autoctrl == 0) // 自动控制
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
                TR0 = 1; //关闭定时器
            }


            if (USART_R == 1)//应该是这个APP上换手动和自动模式
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

                if (autoctrl == 1) // 手动控制
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

                TR0 = 1;                  //打开定时器0
                USART_R = 0;              //清除接收标志位
                for (i = 0; i++; i <= 35) //清除接收缓存区
                {
                    Recive_table[i] = 0;
                }
            }
        }

        if (sign == 2) // sign=2测试服务器连接能否正常通讯
        {
           
                 led6 = 0;
            Send_Uart(0xC0); //发送心跳包
            Send_Uart(0x00);
            Send_Uart(0xC0);
            Send_Uart(0x00);

            Send_Uart(0xC0); //发送心跳包
            Send_Uart(0x00);
            Send_Uart(0xC0);
            Send_Uart(0x00);
            ms_delay(500);
            wait_times = 200;
            while (--wait_times)
            {
                TR0 = 0;

                ms_delay(10);
                if (Recive_table[0] == 0xD0) //收到心跳包回复0xD0表示连接正常，否则sign=1重新连接tcp服务器
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
OLED_ShowString(60,4,"Tem",16); //从第71列像素、第4页（第32行像素）开始显示大小为8*16像素的字符串"Ab123%"
OLED_ShowString(60,6,"Hum",16); 
OLED_ShowNum(75,4,Tem,6,8);	
OLED_ShowNum(75,6,Hum,6,8);	
	
	
    }
}

void Uart_Interrupt() interrupt 4 //串口接收中断函数
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
        if (Recive_table[i] == '\n' || (Recive_table[i] == '}' && Recive_table[i - 1] == '\"') || (Recive_table[i - 1] == 0xD0 && Recive_table[i] == 0x00)) //接收到{...”}时接收完成
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
            for (i = 0; i++; i <= 35) //清除接收缓存区
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
void Timer0() interrupt 1 //定时器0溢出中断
{
    static uint m = 0;
    TH0 = 0XDC; //给定时器赋初值，定时10ms
    TL0 = 0X00;
    m++;

    if (m >= 300) // 3秒传一次温度数据
    {
        m = 0; //清除中断计数
        count++;
        times = 1;
        move=0;
 led7 = ~led7; // led灯指示状态
    }
 if (count%5==0)
 {
  move=1;
 }
    if (count >= 10) // 检测连接
    {  
        TR0 = 0;
        count = 0;
        sign = 2;
    }
}


 
void timer2() interrupt 5{
	TF2=0;        //清除中断标志位
Timer2_Server_Count++;

if(Timer2_Server_Count==16)                    // T2定时器的预装载值为0x0BDC，溢出16次就是1秒钟。

{


sec++;					//计时，50个count为1秒
	if(sec == 60) 									//60秒1分钟
	{
		sec = 0;
		min++;
	}
	if(min == 60)										//60分钟1小时
	{
		min = 0;
		hour++;
	}
	if(hour == 24)									//24小时制
	{
		hour = 0;
	}	
           Timer2_Server_Count=0; 

}

}

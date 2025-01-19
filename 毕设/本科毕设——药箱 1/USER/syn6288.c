#include "syn6288.h"
#include "usart2.h"
#include "string.h"
#include "delay.h"
#include "oled.h"
#include "beep.h"

extern u8 UsartDisPlay[200];
//extern u8 FLAG1;
vu8 take_medicine1_FLAG,take_medicine2_FLAG,take_medicine3_FLAG = 0;



/**************芯片设置命令*********************/
u8 SYN_StopCom[] = {0xFD, 0X00, 0X02, 0X02, 0XFD}; //停止合成
u8 SYN_SuspendCom[] = {0XFD, 0X00, 0X02, 0X03, 0XFC}; //暂停合成
u8 SYN_RecoverCom[] = {0XFD, 0X00, 0X02, 0X04, 0XFB}; //恢复合成
u8 SYN_ChackCom[] = {0XFD, 0X00, 0X02, 0X21, 0XDE}; //状态查询
u8 SYN_PowerDownCom[] = {0XFD, 0X00, 0X02, 0X88, 0X77}; //进入POWER DOWN 状态命令

//选择背景音乐2。(0：无背景音乐  1-15：背景音乐可选)
//m[0~16]:0背景音乐为静音，16背景音乐音量最大
//v[0~16]:0朗读音量为静音，16朗读音量最大
//t[0~5]:0朗读语速最慢，5朗读语速最快
//其他不常用功能请参考数据手册
//		SYN_FrameInfo(2,"[v7][m1][t5]SYN6288语音合成模块");
//Music:选择背景音乐。0:无背景音乐，1~15：选择背景音乐
void SYN_FrameInfo(u8 Music, u8 *HZdata)
{
  /****************需要发送的文本**********************************/
  unsigned  char  Frame_Info[50];
  unsigned  char  HZ_Length;
  unsigned  char  ecc  = 0;  			//定义校验字节
  unsigned  int i = 0;
  HZ_Length = strlen((char*)HZdata); 			//需要发送文本的长度

  /*****************帧固定配置信息**************************************/
  Frame_Info[0] = 0xFD ; 			//构造帧头FD
  Frame_Info[1] = 0x00 ; 			//构造数据区长度的高字节
  Frame_Info[2] = HZ_Length + 3; 		//构造数据区长度的低字节
  Frame_Info[3] = 0x01 ; 			//构造命令字：合成播放命令
  Frame_Info[4] = 0x01 | Music << 4 ; //构造命令参数：背景音乐设定

  /*******************校验码计算***************************************/
  for(i = 0; i < 5; i++)   				//依次发送构造好的5个帧头字节
  {
    ecc = ecc ^ (Frame_Info[i]);		//对发送的字节进行异或校验
  }

  for(i = 0; i < HZ_Length; i++)   		//依次发送待合成的文本数据
  {
    ecc = ecc ^ (HZdata[i]); 				//对发送的字节进行异或校验
  }
  /*******************发送帧信息***************************************/
  memcpy(&Frame_Info[5], HZdata, HZ_Length);
  Frame_Info[5 + HZ_Length] = ecc;
  USART2_SendString(Frame_Info, 5 + HZ_Length + 1);
}


/***********************************************************
* 名    称： YS_SYN_Set(u8 *Info_data)
* 功    能： 主函数	程序入口
* 入口参数： *Info_data:固定的配置信息变量
* 出口参数：
* 说    明：本函数用于配置，停止合成、暂停合成等设置 ，默认波特率9600bps。
* 调用方法：通过调用已经定义的相关数组进行配置。
**********************************************************/
void YS_SYN_Set(u8 *Info_data)
{
  u8 Com_Len;
  Com_Len = strlen((char*)Info_data);
  USART2_SendString(Info_data, Com_Len);
}

/*根据需求自定义OPENMV传送数据32是否接收到*/
void Voice_Text(void)
{ 
   

    if (UsartDisPlay[0] == '1'){
        scan_QR_codes_tone();                                 //扫码音
        take_medicine1_FLAG = 1;                             //吃药1标志位
        SYN_FrameInfo(0,"[v8][m0][t5]退烧药一次一袋一日两次");
        delay_ms(1500);      
        memset(UsartDisPlay, 0, sizeof UsartDisPlay);         //清空数组        
    }
    else if (UsartDisPlay[0] == '2'){
        scan_QR_codes_tone();
        take_medicine2_FLAG = 1;                              //吃药2标志位
        SYN_FrameInfo(0,"[v8][m0][t5]胶囊一次一粒一日两次");
        delay_ms(1500);   
        memset(UsartDisPlay, 0, sizeof UsartDisPlay);         //清空数组        
    }
    else if (UsartDisPlay[0] == '3'){
        scan_QR_codes_tone();
        take_medicine3_FLAG = 1;                              //吃药3标志位
        SYN_FrameInfo(0,"[v8][m0][t5]布洛芬片一次一片一日两次");
        delay_ms(1500);   
        memset(UsartDisPlay, 0, sizeof UsartDisPlay);         //清空数组        
    }
    else{
        memset(UsartDisPlay, 0, sizeof UsartDisPlay);         //清空数组               
  }
    
  

}



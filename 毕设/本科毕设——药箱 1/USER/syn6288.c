#include "syn6288.h"
#include "usart2.h"
#include "string.h"
#include "delay.h"
#include "oled.h"
#include "beep.h"

extern u8 UsartDisPlay[200];
//extern u8 FLAG1;
vu8 take_medicine1_FLAG,take_medicine2_FLAG,take_medicine3_FLAG = 0;



/**************оƬ��������*********************/
u8 SYN_StopCom[] = {0xFD, 0X00, 0X02, 0X02, 0XFD}; //ֹͣ�ϳ�
u8 SYN_SuspendCom[] = {0XFD, 0X00, 0X02, 0X03, 0XFC}; //��ͣ�ϳ�
u8 SYN_RecoverCom[] = {0XFD, 0X00, 0X02, 0X04, 0XFB}; //�ָ��ϳ�
u8 SYN_ChackCom[] = {0XFD, 0X00, 0X02, 0X21, 0XDE}; //״̬��ѯ
u8 SYN_PowerDownCom[] = {0XFD, 0X00, 0X02, 0X88, 0X77}; //����POWER DOWN ״̬����

//ѡ�񱳾�����2��(0���ޱ�������  1-15���������ֿ�ѡ)
//m[0~16]:0��������Ϊ������16���������������
//v[0~16]:0�ʶ�����Ϊ������16�ʶ��������
//t[0~5]:0�ʶ�����������5�ʶ��������
//���������ù�����ο������ֲ�
//		SYN_FrameInfo(2,"[v7][m1][t5]SYN6288�����ϳ�ģ��");
//Music:ѡ�񱳾����֡�0:�ޱ������֣�1~15��ѡ�񱳾�����
void SYN_FrameInfo(u8 Music, u8 *HZdata)
{
  /****************��Ҫ���͵��ı�**********************************/
  unsigned  char  Frame_Info[50];
  unsigned  char  HZ_Length;
  unsigned  char  ecc  = 0;  			//����У���ֽ�
  unsigned  int i = 0;
  HZ_Length = strlen((char*)HZdata); 			//��Ҫ�����ı��ĳ���

  /*****************֡�̶�������Ϣ**************************************/
  Frame_Info[0] = 0xFD ; 			//����֡ͷFD
  Frame_Info[1] = 0x00 ; 			//�������������ȵĸ��ֽ�
  Frame_Info[2] = HZ_Length + 3; 		//�������������ȵĵ��ֽ�
  Frame_Info[3] = 0x01 ; 			//���������֣��ϳɲ�������
  Frame_Info[4] = 0x01 | Music << 4 ; //����������������������趨

  /*******************У�������***************************************/
  for(i = 0; i < 5; i++)   				//���η��͹���õ�5��֡ͷ�ֽ�
  {
    ecc = ecc ^ (Frame_Info[i]);		//�Է��͵��ֽڽ������У��
  }

  for(i = 0; i < HZ_Length; i++)   		//���η��ʹ��ϳɵ��ı�����
  {
    ecc = ecc ^ (HZdata[i]); 				//�Է��͵��ֽڽ������У��
  }
  /*******************����֡��Ϣ***************************************/
  memcpy(&Frame_Info[5], HZdata, HZ_Length);
  Frame_Info[5 + HZ_Length] = ecc;
  USART2_SendString(Frame_Info, 5 + HZ_Length + 1);
}


/***********************************************************
* ��    �ƣ� YS_SYN_Set(u8 *Info_data)
* ��    �ܣ� ������	�������
* ��ڲ����� *Info_data:�̶���������Ϣ����
* ���ڲ�����
* ˵    �����������������ã�ֹͣ�ϳɡ���ͣ�ϳɵ����� ��Ĭ�ϲ�����9600bps��
* ���÷�����ͨ�������Ѿ�������������������á�
**********************************************************/
void YS_SYN_Set(u8 *Info_data)
{
  u8 Com_Len;
  Com_Len = strlen((char*)Info_data);
  USART2_SendString(Info_data, Com_Len);
}

/*���������Զ���OPENMV��������32�Ƿ���յ�*/
void Voice_Text(void)
{ 
   

    if (UsartDisPlay[0] == '1'){
        scan_QR_codes_tone();                                 //ɨ����
        take_medicine1_FLAG = 1;                             //��ҩ1��־λ
        SYN_FrameInfo(0,"[v8][m0][t5]����ҩһ��һ��һ������");
        delay_ms(1500);      
        memset(UsartDisPlay, 0, sizeof UsartDisPlay);         //�������        
    }
    else if (UsartDisPlay[0] == '2'){
        scan_QR_codes_tone();
        take_medicine2_FLAG = 1;                              //��ҩ2��־λ
        SYN_FrameInfo(0,"[v8][m0][t5]����һ��һ��һ������");
        delay_ms(1500);   
        memset(UsartDisPlay, 0, sizeof UsartDisPlay);         //�������        
    }
    else if (UsartDisPlay[0] == '3'){
        scan_QR_codes_tone();
        take_medicine3_FLAG = 1;                              //��ҩ3��־λ
        SYN_FrameInfo(0,"[v8][m0][t5]�����Ƭһ��һƬһ������");
        delay_ms(1500);   
        memset(UsartDisPlay, 0, sizeof UsartDisPlay);         //�������        
    }
    else{
        memset(UsartDisPlay, 0, sizeof UsartDisPlay);         //�������               
  }
    
  

}



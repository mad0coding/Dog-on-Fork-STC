#ifndef _RGBDRV_H_
#define _RGBDRV_H_

#include <intrins.h>
#include "DEBUG.H"
#include "STC8H_GPIO.h"
#include "usb_req_class.h"
#include "Para.h"

/******************************�������******************************/
#define SW			P21		//����
#define KEY			P24		//���� ���µ͵�ƽ
#define EC_KEY		P32		//��ť���� ���µ͵�ƽ
/********************************************************************/

/***********************************�������***********************************/
#define CRST		P23		//HUB��λ���� �͵�ƽ��λ
#define GATE		P34		//�ܿ�USB�˿ڿ��� �͵�ƽ����
#define WS_DOUT		P37		//WS2812��������
//#define LED1		P01		//LED��������1
//#define LED2		P02		//LED��������2
//#define LED_ON		(LED1 = 1)//LED��
//#define LED_OFF		(LED1 = 0)//LED��
#define LED_ON		(P0_PULL_UP_ENABLE(GPIO_Pin_1 | GPIO_Pin_2))	//LED��
#define LED_OFF		(P0_PULL_UP_DISABLE(GPIO_Pin_1 | GPIO_Pin_2))	//LED��
/******************************************************************************/

/******************************�ܿ�USB�˿ڲ���******************************/
#define GATE_ON		(GATE = 0)	//�ܿ�USB�˿ڿ�
#define GATE_OFF	(GATE = 1)	//�ܿ�USB�˿ڹ�
#define DP_PU_ON	P2_PULL_UP_ENABLE(GPIO_Pin_6)	//DP������
#define DP_PU_OFF	P2_PULL_UP_DISABLE(GPIO_Pin_6)	//DP������
#define DM_PU_ON	P2_PULL_UP_ENABLE(GPIO_Pin_5)	//DM������
#define DM_PU_OFF	P2_PULL_UP_DISABLE(GPIO_Pin_5)	//DM������
/***************************************************************************/

#define ADC_VN		16	//ADCת������
#define colorAngle	167	//ɫ����1/6

/**************************************************HID��ӡ**************************************************/
#define PRINT_SEND(buf,n)		EP2_Send_Data(buf,n,' ')
#define PRINT0(s)				PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s))
#define PRINT1(s,x1)			PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s,x1))
#define PRINT2(s,x1,x2)			PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s,x1,x2))
#define PRINT3(s,x1,x2,x3)		PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s,x1,x2,x3))
extern uint8_t PRINT_BUF[64];
/***********************************************************************************************************/

/******************************extern����******************************/
extern bit CUSB_CUT, CUSB_EN, HUB_EN;//CUSB�����ж�,CUSBʹ��,HUBʹ��
extern uint8_t keyNow[];//��ǰ����
extern uint8_t keyOld[];//�ϴΰ���

extern UINT16X ADC_DATA[3][ADC_VN + 2];
extern UINT16D ADC_USB_LIMIT;//����ֵ
extern uint8_t LedInput[9];//LED����:0-NUM,1-CAPS,2-SCROLL,3-CUSB,4-HUB,5-hidCs,6-2.4G,7-CUSTOM1,8-CUSTOM2
/**********************************************************************/


void funcOutput(void);//�������
void funcHandle(void);//���ܴ���



#endif







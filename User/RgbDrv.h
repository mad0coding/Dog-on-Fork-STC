#ifndef _RGBDRV_H_
#define _RGBDRV_H_

#include <intrins.h>
#include "DEBUG.H"
#include "STC8H_GPIO.h"
#include "usb_req_class.h"
#include "Para.h"

/******************************输入操作******************************/
#define SW			P21		//开关
#define KEY			P24		//按键 按下低电平
#define EC_KEY		P32		//旋钮按键 按下低电平
/********************************************************************/

/***********************************输出操作***********************************/
#define CRST		P23		//HUB复位控制 低电平复位
#define GATE		P34		//受控USB端口控制 低电平开启
#define WS_DOUT		P37		//WS2812数据引脚
//#define LED1		P01		//LED驱动引脚1
//#define LED2		P02		//LED驱动引脚2
//#define LED_ON		(LED1 = 1)//LED亮
//#define LED_OFF		(LED1 = 0)//LED灭
#define LED_ON		(P0_PULL_UP_ENABLE(GPIO_Pin_1 | GPIO_Pin_2))	//LED亮
#define LED_OFF		(P0_PULL_UP_DISABLE(GPIO_Pin_1 | GPIO_Pin_2))	//LED灭
/******************************************************************************/

/******************************受控USB端口操作******************************/
#define GATE_ON		(GATE = 0)	//受控USB端口开
#define GATE_OFF	(GATE = 1)	//受控USB端口关
#define DP_PU_ON	P2_PULL_UP_ENABLE(GPIO_Pin_6)	//DP上拉开
#define DP_PU_OFF	P2_PULL_UP_DISABLE(GPIO_Pin_6)	//DP上拉关
#define DM_PU_ON	P2_PULL_UP_ENABLE(GPIO_Pin_5)	//DM上拉开
#define DM_PU_OFF	P2_PULL_UP_DISABLE(GPIO_Pin_5)	//DM上拉关
/***************************************************************************/

#define ADC_VN		16	//ADC转换次数
#define colorAngle	167	//色环的1/6

/**************************************************HID打印**************************************************/
#define PRINT_SEND(buf,n)		EP2_Send_Data(buf,n,' ')
#define PRINT0(s)				PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s))
#define PRINT1(s,x1)			PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s,x1))
#define PRINT2(s,x1,x2)			PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s,x1,x2))
#define PRINT3(s,x1,x2,x3)		PRINT_SEND(PRINT_BUF,sprintf(PRINT_BUF,s,x1,x2,x3))
extern uint8_t PRINT_BUF[64];
/***********************************************************************************************************/

/******************************extern变量******************************/
extern bit CUSB_CUT, CUSB_EN, HUB_EN;//CUSB过流切断,CUSB使能,HUB使能
extern uint8_t keyNow[];//当前按键
extern uint8_t keyOld[];//上次按键

extern UINT16X ADC_DATA[3][ADC_VN + 2];
extern UINT16D ADC_USB_LIMIT;//限流值
extern uint8_t LedInput[9];//LED输入:0-NUM,1-CAPS,2-SCROLL,3-CUSB,4-HUB,5-hidCs,6-2.4G,7-CUSTOM1,8-CUSTOM2
/**********************************************************************/


void funcOutput(void);//各类输出
void funcHandle(void);//功能处理



#endif







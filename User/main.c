
//#include "intrins.h"
#include "DEBUG.H"
#include "STC8H_GPIO.h"
#include "usb.h"
#include "RgbDrv.h"
#include "drv_RF24L01.h"
#include "W25Qxx.h"
#include "Para.h"


/****************************************HID报文相关数据****************************************/
UINT8I KeyBrd_if_send = 0;//键盘报文是否发送
UINT8I Mouse_if_send = 0;//鼠标报文是否发送
UINT8I Point_if_send = 0;//指针报文是否发送
UINT8I Vol_if_send = 0;//媒体报文是否发送

UINT8I KeyBrd_data[KB_len] = {1,0,0,0};//编号1,功能键,保留0,其他按键
//功能键:bit7-bit0分别为为右win alt shift ctrl,左win alt shift ctrl

UINT8I Mouse_data[5] = {2,0,0,0,0};//编号2,功能键,x,y,滚轮
//功能键:bit0为左键,bit1为右键,bit2为中键,bit6为x是否溢出,bit7为y是否溢出

UINT8I Point_data[7] = {3,0x10,1,0xFF,0xFF,0xFF,0xFF};//编号3,功能键,id,x_L,x_H,y_L,y_H
//功能键:bit0为Tip Switch,bit1为Barrel Switch,bit2为Invert,bit3为Eraser Switch,bit4为In Range

UINT8I Vol_data[2] = {4,0};//编号4,功能键
//功能键:bit0音量加,bit1音量减,bit2静音,bit3播放暂停
/***********************************************************************************************/

void main(void)
{
    P_SW2 |= 0x80;  //扩展寄存器(XFR)访问使能
	
	/****************************************输入引脚初始化****************************************/
	SW = KEY = EC_KEY = 1;
	P2_MODE_IO_PU(GPIO_Pin_1);//SW准双向
	P2_MODE_IO_PU(GPIO_Pin_4);//KEY准双向
	P3_MODE_IO_PU(GPIO_Pin_2);//EC_KEY准双向
	/**********************************************************************************************/
	
	paraInit(0);//读取并用参数初始化系统
	
	/****************************************输出引脚初始化****************************************/
	WS_DOUT = 0;
	P3_MODE_OUT_OD(GPIO_Pin_4);//GATE开漏
	P3_MODE_OUT_PP(GPIO_Pin_7);//DOUT推挽
	
	//CRST = 1;
	P2_MODE_OUT_OD(GPIO_Pin_3);//CRST开漏
	
//	LED1 = LED2 = 0;
//	P0_MODE_IO_PU(GPIO_Pin_1);//准双向
//	P0_MODE_IO_PU(GPIO_Pin_2);//准双向
	/**********************************************************************************************/
	
	/****************************************编码器初始化****************************************/
//	P20 = P22 = 1;
//	P2_MODE_IO_PU(GPIO_Pin_2);//准双向
//	P2_MODE_IO_PU(GPIO_Pin_0);//准双向
//	P2_MODE_IN_HIZ(GPIO_Pin_2);//高阻输入
//	P2_MODE_IN_HIZ(GPIO_Pin_0);//高阻输入
	P2_PULL_UP_ENABLE(GPIO_Pin_2);//上拉
	P2_PULL_UP_ENABLE(GPIO_Pin_0);//上拉

	PWMA_ENO = 0x00;		//失能所有PWM输出
	PWMA_PS = 0x05;			//引脚映射 PWM1P_2-P2.0 PWM2P_2-P2.2
	PWMA_PSCRH = 0x00;		//预分频寄存器
	PWMA_PSCRL = 0x00;
	PWMA_CCMR1 = PWMA_CCMR2 = 0x70 + 0x1;	//滤波器32时钟 通道模式配置为输入
	PWMA_SMCR = 0x03;		//编码器模式3
	PWMA_CCER1 = PWMA_CCER2 = 0x55;			//配置通道使能和极性
	PWMA_CR1 |= 0x01;		//使能计数器
	/********************************************************************************************/
	
	/****************************************定时器初始化****************************************/
	PWMB_PSCR = 24000 - 1;	//设置预分频器
	PWMB_ARR = 0xFFFF;		//自动重装载寄存器
	PWMB_CCER1 = 0x00;		//不使用输入捕获
	PWMB_CCMR1 = PWMB_CCMR2 = PWMB_CCMR3 = PWMB_CCMR4 = 0x00;	//不使用捕获/比较
	PWMB_CCR5 = PWMB_CCR6 = PWMB_CCR7 = PWMB_CCR8 = 0x00;		//捕获/比较数值为零
	PWMB_IER = 0x00;		//不使能中断
	PWMB_CR1 |= 0x81;		//使能 ARR 预装载，开始计时
	/********************************************************************************************/
	
	/****************************************SPI初始化****************************************/
	P1_MODE_OUT_PP(GPIO_Pin_5);	//SCK推挽
	P1_MODE_OUT_PP(GPIO_Pin_3);	//MOSI推挽
	P11 = P16 = 1;				//CS和CSN拉高
	P1_MODE_IO_PU(GPIO_Pin_1);	//CS准双向
	P1_MODE_IO_PU(GPIO_Pin_6);	//CSN准双向
	SPCTL = 0xD0;				//忽略SS引脚 主机模式 模式0 频率SYSclk/4
	SPSTAT = 0xC0;				//清标志
	/*****************************************************************************************/
	if(CFG_G24_MODE != 0) NRF24L01_Config(0);//无线模块初始化为接收模式
	
	/****************************************ADC初始化****************************************/
	ADCTIM = 0x3F;			//设置ADC内部时序
	ADCCFG = 0x20 + 0xF;	//结果右对齐 设置ADC时钟为系统时钟/2/16
	ADC_CONTR = 0x80;		//使能ADC模块
	//ADCEXCFG = 0x07;		//此寄存器的转换次数设置仅非DMA模式下有效
	DMA_ADC_STA = 0x00;		//清中断
	DMA_ADC_CFG = 0x80;		//bit7 1:Enable Interrupt
	DMA_ADC_RXAH = (uint8_t)((uint16_t)ADC_DATA >> 8);	//ADC转换数据存储地址
	DMA_ADC_RXAL = (uint8_t)ADC_DATA;
	DMA_ADC_CFG2 = 0x0B;	//每个通道转换16次
	DMA_ADC_CHSW0 = 0x00;	//ADC通道使能 None
	DMA_ADC_CHSW1 = 0x89;	//ADC通道使能 ADC8(电压) ADC11(电流) ADC15(1.19V参考)
	DMA_ADC_CR = 0xC0;		//bit7 1:Enable ADC_DMA, bit6 1:Start ADC_DMA
	/*****************************************************************************************/
	
//	if(/*!(RSTFLAG & 0x04)*/(RSTFLAG & 0x10)) usbMode = !KEY;//若不为软件复位

	usb_init();	//USB初始化
	EA = 1;		//总中断开启

    while(1){
		funcHandle();	//功能处理

		if(KeyBrd_if_send){	//若键盘报文需要发送
			EP1_Send_Data(KeyBrd_data, sizeof(KeyBrd_data));
			delay_ms(8);
		}
		if(Mouse_if_send){	//若鼠标报文需要发送
			EP1_Send_Data(Mouse_data, sizeof(Mouse_data));
			delay_ms(8);
		}
		if(Point_if_send){	//若触摸报文需要发送
			EP1_Send_Data(Point_data, sizeof(Point_data));
			delay_ms(8);
		}
		if(Vol_if_send){	//若媒体报文需要发送
			EP1_Send_Data(Vol_data, sizeof(Vol_data));
			delay_ms(8);
		}
		if(!(KeyBrd_if_send || Mouse_if_send || Point_if_send || Vol_if_send)) delay_ms(1);//无报文需要发送
    }
}












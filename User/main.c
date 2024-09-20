
//#include "intrins.h"
#include "DEBUG.H"
#include "STC8H_GPIO.h"
#include "usb.h"
#include "RgbDrv.h"
#include "drv_RF24L01.h"
#include "W25Qxx.h"
#include "Para.h"


/****************************************HID�����������****************************************/
UINT8I KeyBrd_if_send = 0;//���̱����Ƿ���
UINT8I Mouse_if_send = 0;//��걨���Ƿ���
UINT8I Point_if_send = 0;//ָ�뱨���Ƿ���
UINT8I Vol_if_send = 0;//ý�屨���Ƿ���

UINT8I KeyBrd_data[KB_len] = {1,0,0,0};//���1,���ܼ�,����0,��������
//���ܼ�:bit7-bit0�ֱ�ΪΪ��win alt shift ctrl,��win alt shift ctrl

UINT8I Mouse_data[5] = {2,0,0,0,0};//���2,���ܼ�,x,y,����
//���ܼ�:bit0Ϊ���,bit1Ϊ�Ҽ�,bit2Ϊ�м�,bit6Ϊx�Ƿ����,bit7Ϊy�Ƿ����

UINT8I Point_data[7] = {3,0x10,1,0xFF,0xFF,0xFF,0xFF};//���3,���ܼ�,id,x_L,x_H,y_L,y_H
//���ܼ�:bit0ΪTip Switch,bit1ΪBarrel Switch,bit2ΪInvert,bit3ΪEraser Switch,bit4ΪIn Range

UINT8I Vol_data[2] = {4,0};//���4,���ܼ�
//���ܼ�:bit0������,bit1������,bit2����,bit3������ͣ
/***********************************************************************************************/

void main(void)
{
    P_SW2 |= 0x80;  //��չ�Ĵ���(XFR)����ʹ��
	
	/****************************************�������ų�ʼ��****************************************/
	SW = KEY = EC_KEY = 1;
	P2_MODE_IO_PU(GPIO_Pin_1);//SW׼˫��
	P2_MODE_IO_PU(GPIO_Pin_4);//KEY׼˫��
	P3_MODE_IO_PU(GPIO_Pin_2);//EC_KEY׼˫��
	/**********************************************************************************************/
	
	paraInit(0);//��ȡ���ò�����ʼ��ϵͳ
	
	/****************************************������ų�ʼ��****************************************/
	WS_DOUT = 0;
	P3_MODE_OUT_OD(GPIO_Pin_4);//GATE��©
	P3_MODE_OUT_PP(GPIO_Pin_7);//DOUT����
	
	//CRST = 1;
	P2_MODE_OUT_OD(GPIO_Pin_3);//CRST��©
	
//	LED1 = LED2 = 0;
//	P0_MODE_IO_PU(GPIO_Pin_1);//׼˫��
//	P0_MODE_IO_PU(GPIO_Pin_2);//׼˫��
	/**********************************************************************************************/
	
	/****************************************��������ʼ��****************************************/
//	P20 = P22 = 1;
//	P2_MODE_IO_PU(GPIO_Pin_2);//׼˫��
//	P2_MODE_IO_PU(GPIO_Pin_0);//׼˫��
//	P2_MODE_IN_HIZ(GPIO_Pin_2);//��������
//	P2_MODE_IN_HIZ(GPIO_Pin_0);//��������
	P2_PULL_UP_ENABLE(GPIO_Pin_2);//����
	P2_PULL_UP_ENABLE(GPIO_Pin_0);//����

	PWMA_ENO = 0x00;		//ʧ������PWM���
	PWMA_PS = 0x05;			//����ӳ�� PWM1P_2-P2.0 PWM2P_2-P2.2
	PWMA_PSCRH = 0x00;		//Ԥ��Ƶ�Ĵ���
	PWMA_PSCRL = 0x00;
	PWMA_CCMR1 = PWMA_CCMR2 = 0x70 + 0x1;	//�˲���32ʱ�� ͨ��ģʽ����Ϊ����
	PWMA_SMCR = 0x03;		//������ģʽ3
	PWMA_CCER1 = PWMA_CCER2 = 0x55;			//����ͨ��ʹ�ܺͼ���
	PWMA_CR1 |= 0x01;		//ʹ�ܼ�����
	/********************************************************************************************/
	
	/****************************************��ʱ����ʼ��****************************************/
	PWMB_PSCR = 24000 - 1;	//����Ԥ��Ƶ��
	PWMB_ARR = 0xFFFF;		//�Զ���װ�ؼĴ���
	PWMB_CCER1 = 0x00;		//��ʹ�����벶��
	PWMB_CCMR1 = PWMB_CCMR2 = PWMB_CCMR3 = PWMB_CCMR4 = 0x00;	//��ʹ�ò���/�Ƚ�
	PWMB_CCR5 = PWMB_CCR6 = PWMB_CCR7 = PWMB_CCR8 = 0x00;		//����/�Ƚ���ֵΪ��
	PWMB_IER = 0x00;		//��ʹ���ж�
	PWMB_CR1 |= 0x81;		//ʹ�� ARR Ԥװ�أ���ʼ��ʱ
	/********************************************************************************************/
	
	/****************************************SPI��ʼ��****************************************/
	P1_MODE_OUT_PP(GPIO_Pin_5);	//SCK����
	P1_MODE_OUT_PP(GPIO_Pin_3);	//MOSI����
	P11 = P16 = 1;				//CS��CSN����
	P1_MODE_IO_PU(GPIO_Pin_1);	//CS׼˫��
	P1_MODE_IO_PU(GPIO_Pin_6);	//CSN׼˫��
	SPCTL = 0xD0;				//����SS���� ����ģʽ ģʽ0 Ƶ��SYSclk/4
	SPSTAT = 0xC0;				//���־
	/*****************************************************************************************/
	if(CFG_G24_MODE != 0) NRF24L01_Config(0);//����ģ���ʼ��Ϊ����ģʽ
	
	/****************************************ADC��ʼ��****************************************/
	ADCTIM = 0x3F;			//����ADC�ڲ�ʱ��
	ADCCFG = 0x20 + 0xF;	//����Ҷ��� ����ADCʱ��Ϊϵͳʱ��/2/16
	ADC_CONTR = 0x80;		//ʹ��ADCģ��
	//ADCEXCFG = 0x07;		//�˼Ĵ�����ת���������ý���DMAģʽ����Ч
	DMA_ADC_STA = 0x00;		//���ж�
	DMA_ADC_CFG = 0x80;		//bit7 1:Enable Interrupt
	DMA_ADC_RXAH = (uint8_t)((uint16_t)ADC_DATA >> 8);	//ADCת�����ݴ洢��ַ
	DMA_ADC_RXAL = (uint8_t)ADC_DATA;
	DMA_ADC_CFG2 = 0x0B;	//ÿ��ͨ��ת��16��
	DMA_ADC_CHSW0 = 0x00;	//ADCͨ��ʹ�� None
	DMA_ADC_CHSW1 = 0x89;	//ADCͨ��ʹ�� ADC8(��ѹ) ADC11(����) ADC15(1.19V�ο�)
	DMA_ADC_CR = 0xC0;		//bit7 1:Enable ADC_DMA, bit6 1:Start ADC_DMA
	/*****************************************************************************************/
	
//	if(/*!(RSTFLAG & 0x04)*/(RSTFLAG & 0x10)) usbMode = !KEY;//����Ϊ�����λ

	usb_init();	//USB��ʼ��
	EA = 1;		//���жϿ���

    while(1){
		funcHandle();	//���ܴ���

		if(KeyBrd_if_send){	//�����̱�����Ҫ����
			EP1_Send_Data(KeyBrd_data, sizeof(KeyBrd_data));
			delay_ms(8);
		}
		if(Mouse_if_send){	//����걨����Ҫ����
			EP1_Send_Data(Mouse_data, sizeof(Mouse_data));
			delay_ms(8);
		}
		if(Point_if_send){	//������������Ҫ����
			EP1_Send_Data(Point_data, sizeof(Point_data));
			delay_ms(8);
		}
		if(Vol_if_send){	//��ý�屨����Ҫ����
			EP1_Send_Data(Vol_data, sizeof(Vol_data));
			delay_ms(8);
		}
		if(!(KeyBrd_if_send || Mouse_if_send || Point_if_send || Vol_if_send)) delay_ms(1);//�ޱ�����Ҫ����
    }
}












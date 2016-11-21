#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "common.h" 

//////////////////////////////////////////////////////////////////////////////////	 

//#define USART1_REC_NUM  			100  	//�����������ֽ��� 200
//extern u8 uart_byte_count;          //uart_byte_countҪС��USART_REC_LEN
////extern u8 receive_str[USART1_REC_NUM];  

//void uart1_init(u32 bound);
//void uart1SendChars(u8 *str, u16 strlen);

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);
void uart1SendChars(u8 *str, u16 strlen);

#endif



#ifndef __HS0038_H
#define __HS0038_H

#include	"common.h"
	
//////////////////////////////////////////////////////////////////////////////
extern u8  Ir_Time,IR_flag;//������ȫ�ֱ���

extern u8 Decode_OK,Recive_OK,IR_OK;//�������״̬������״̬
extern u8 Ir_Record[4];    //�����ĺ����룬�ֱ��� �ͻ��룬�ͻ��룬�����룬�����뷴��
extern u8 Ir_TimeData[33]; //33���ߵ͵�ƽ��ʱ������gfaaeg awgGww

void Ir_Decode(void);
#endif


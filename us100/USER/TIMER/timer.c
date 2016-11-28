#include "timer.h"
#include "us100.h"
#include "usart.h"	


/****************************************************************************
* Ãû    ³Æ: TIM_Cap_Init(u32 arr,u16 psc)
* ¹¦    ÄÜ £º5Â· ²¶»ñÅäÖÃ 
						TIM5 CH1~CH4¶ÔÓ¦Òý½Å£ºPA0,PA2,PA3
						TIM9 CH1~CH2¶ÔÓ¦Òý½Å: PE5,PE6
						
						TIM2 CH1~Ch4¶ÔÓ¦Òý½Å£ºPA15,PB3£¬PB10,PB11
						×¢£ºTIM5µÄCH2±»Õ¼ÓÃ£¬´ò¿ªºóÆµ·±½øÈëÖÐ¶Ï£¬¹Ê²»ÓÃ
						1#US100
						
* Èë¿Ú²ÎÊý£ºarr£º×Ô¶¯ÖØ×°Öµ(TIM2,TIM5ÊÇ32Î»µÄ!!)
						psc£ºÊ±ÖÓÔ¤·ÖÆµÊý
* ·µ»Ø²ÎÊý£ºÎÞ
* Ëµ    Ã÷£º¶¨Ê±Æ÷Òç³öÊ±¼ä¼ÆËã·½·¨:Tout=((auto_data+1)*(fractional+1))/Ft us  
* ÅäÖÃ£º1# US100 ECHO£ºPA0£¬ TRIG:PG3

****************************************************************************/
void ChannelSet(void);
int status;


TIM_CAPTURE TIM5CH1,TIM5CH2,TIM5CH3,TIM5CH4;
//TIM_CAPTURE TIM9CH1,TIM9CH2;
TIM_CAPTURE TIM2CH1,TIM2CH2,TIM2CH3,TIM2CH4;

//extern void uart1SendChars(u8 *str, u16 strlen);

TIM_ICInitTypeDef  TIM5_ICInitStructure,TIM2_ICInitStructure;



//¶¨Ê±Æ÷5Í¨µÀ1ÊäÈë²¶»ñÅäÖÃ
//arr£º×Ô¶¯ÖØ×°Öµ(TIM2,TIM5ÊÇ32Î»µÄ!!)
//psc£ºÊ±ÖÓÔ¤·ÖÆµÊý
void TIM_Cap_Init(u32 arr,u16 psc)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
/*ÅäÖÃÖÐ¶Ï*/
	//TIM5
  NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//ÇÀÕ¼ÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//×ÓÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	//¸ù¾ÝÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯VIC¼Ä´æÆ÷
	//TIM2
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//ÇÀÕ¼ÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		//×ÓÓÅÏÈ¼¶3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQÍ¨µÀÊ¹ÄÜ
	NVIC_Init(&NVIC_InitStructure);	//¸ù¾ÝÖ¸¶¨µÄ²ÎÊý³õÊ¼»¯VIC¼Ä´æÆ÷¢
//???????



	/*Ê±ÖÓÉèÖÃ*/
	//TIM5
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5,ENABLE);  	//TIMÊ±ÖÓÊ¹ÄÜ 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//Ê¹ÄÜPORTAÊ±ÖÓ	
	//TIM2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);	//Ê¹ÄÜPORTEÊ±ÖÓ	
 
	/* GPIO³õÊ¼»¯*/
	//TIM5-GPIOA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3; //GPIOA0~3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//¸´ÓÃ¹¦ÄÜ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//ËÙ¶È100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //ÍÆÍì¸´ÓÃÊä³ö
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN; //ÏÂÀ­
	GPIO_Init(GPIOA,&GPIO_InitStructure); //³õÊ¼»¯PA0

	// TIM2-GPIOB
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_11; //GPIOB 3,10,11
	GPIO_Init(GPIOB,&GPIO_InitStructure); //³õÊ¼»¯PB

//	
	/*Òý½Å¸´ÓÃÓ³Éä*/
	//PA0~3¸´ÓÃÎ»¶¨Ê±Æ÷5
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource0,GPIO_AF_TIM5); //
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource1,GPIO_AF_TIM5); //
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_TIM5); //
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM5); //
	//PE5~6¸´ÓÃÎ»¶¨Ê±Æ÷9
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource3,GPIO_AF_TIM2); //
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_TIM2); //
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_TIM2); //
	//GPIO_PinAFConfig(GPIOE,GPIO_PinSource10,GPIO_AF_TIM2); //
	
  /*TIM³õÊ¼»¯  */
	//TIM5
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //¶¨Ê±Æ÷·ÖÆµ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //ÏòÉÏ¼ÆÊýÄ£Ê½
	TIM_TimeBaseStructure.TIM_Period=arr;   //×Ô¶¯ÖØ×°ÔØÖµ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseInit(TIM5,&TIM_TimeBaseStructure);
	//TIM2
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);
	
	/*²¶»ñ²ÎÊýÉèÖÃ*/
	//³õÊ¼»¯TIM5ÊäÈë²¶»ñ²ÎÊý
//	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	Ñ¡ÔñÊäÈë¶Ë IC1Ó³Éäµ½TI1ÉÏ
//  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//ÉÏÉýÑØ²¶»ñ
//  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //Ó³Éäµ½TI1ÉÏ
//  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //ÅäÖÃÊäÈë·ÖÆµ,²»·ÖÆµ 
//  TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ÅäÖÃÊäÈëÂË²¨Æ÷ ²»ÂË²¨
//	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
//	
//	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2; //CC1S=01 	Ñ¡ÔñÊäÈë¶Ë IC1Ó³Éäµ½TI1ÉÏ
//  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//ÉÏÉýÑØ²¶»ñ
//  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //Ó³Éäµ½TI1ÉÏ
//  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //ÅäÖÃÊäÈë·ÖÆµ,²»·ÖÆµ 
//  TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ÅäÖÃÊäÈëÂË²¨Æ÷ ²»ÂË²¨
//	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
//	
//	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_3; //CC1S=01 	Ñ¡ÔñÊäÈë¶Ë IC1Ó³Éäµ½TI1ÉÏ
//  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//ÉÏÉýÑØ²¶»ñ
//  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //Ó³Éäµ½TI1ÉÏ
//  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //ÅäÖÃÊäÈë·ÖÆµ,²»·ÖÆµ 
//  TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ÅäÖÃÊäÈëÂË²¨Æ÷ ²»ÂË²¨
//	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
//	
//	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_4; //CC1S=01 	Ñ¡ÔñÊäÈë¶Ë IC1Ó³Éäµ½TI1ÉÏ
//  TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//ÉÏÉýÑØ²¶»ñ
//  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //Ó³Éäµ½TI1ÉÏ
//  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //ÅäÖÃÊäÈë·ÖÆµ,²»·ÖÆµ 
//  TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ÅäÖÃÊäÈëÂË²¨Æ÷ ²»ÂË²¨
//	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	
	//TIM5 µÄ4¸öÍ¨µÀ
	TIM5_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//ÉÏÉýÑØ²¶»ñ
  TIM5_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //Ó³Éäµ½TI1ÉÏ
  TIM5_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //ÅäÖÃÊäÈë·ÖÆµ,²»·ÖÆµ 
  TIM5_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ÅäÖÃÊäÈëÂË²¨Æ÷ ²»ÂË²¨
	
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_1; //CC1S=01 	Ñ¡ÔñÊäÈë¶Ë IC1Ó³Éäµ½TI1ÉÏ
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_2; 
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_3; 
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	TIM5_ICInitStructure.TIM_Channel = TIM_Channel_4; 
	TIM_ICInit(TIM5, &TIM5_ICInitStructure);
	//TIM2µÄ3¸öÍ¨µÀ
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//ÉÏÉýÑØ²¶»ñ
  TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //Ó³Éäµ½TI1ÉÏ
  TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //ÅäÖÃÊäÈë·ÖÆµ,²»·ÖÆµ 
  TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 ÅäÖÃÊäÈëÂË²¨Æ÷ ²»ÂË²¨
//	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; 
//	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_2; 
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_3; 
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_4; 
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);
	// ÔÊÐí²¶»ñÖÐ¶Ï
	//TIM5
	//TIM_ITConfig(TIM5,TIM_IT_Update|TIM_IT_CC1,ENABLE);//ÔÊÐí¸üÐÂÖÐ¶Ï ,ÔÊÐíCC1IE²¶»ñÖÐ¶Ï	
	//	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);//ÔÊÐí¸üÐÂÖÐ¶Ï ,ÔÊÐíCC1IE²¶»ñÖÐ¶Ï	
		TIM_ITConfig(TIM5, TIM_IT_CC1  | TIM_IT_CC3 | TIM_IT_CC4  |TIM_IT_Update,ENABLE);//ÔÊÐí¸üÐÂÖÐ¶Ï ,ÔÊÐíCC1IE²¶»ñÖÐ¶Ï	
	//	TIM_ITConfig(TIM5,TIM_IT_CC2,ENABLE);
//	TIM_ITConfig(TIM5,TIM_IT_CC3,ENABLE);
//	TIM_ITConfig(TIM5,TIM_IT_CC4,ENABLE);
	//TIM2
		TIM_ITConfig(TIM2,TIM_IT_Update | TIM_IT_CC2| TIM_IT_CC3 | TIM_IT_CC4,ENABLE);
//	TIM_ITConfig(TIM9,TIM_IT_Update|TIM_IT_CC2,ENABLE);
	
	/*Ê¹ÄÜ¶¨Ê±Æ÷*/
  TIM_Cmd(TIM5,ENABLE ); 	//Ê¹ÄÜ¶¨Ê±Æ÷5
	
	TIM_Cmd(TIM2,ENABLE );

	
}



//Í¨µÀÀàÐÍ½á¹¹Ìå³õÊ¼»¯ÅäÖÃ
void ChannelSet(void)
{
	
	TIM_CAPTURE TIMX;
	
	//
	TIMX.CAPTURE_VAL=0;
	TIMX.CAPTURE_STA=0;
	TIMX.timX = TIM5;
	TIMX.TIM_IT_CCX= TIM_IT_CC1;
	TIM5CH4 = TIM5CH3 = TIM5CH2= TIM5CH1=TIMX;
	//
	TIM5CH1.TIM_IT_CCX = TIM_IT_CC1;
	TIM5CH2.TIM_IT_CCX = TIM_IT_CC2;
	TIM5CH3.TIM_IT_CCX = TIM_IT_CC3;
	TIM5CH4.TIM_IT_CCX = TIM_IT_CC4;
	//
	TIMX.timX = TIM2;
	TIM2CH4=TIM2CH3=TIM2CH2=TIM2CH1=TIMX;
	TIM2CH1.TIM_IT_CCX = TIM_IT_CC1;
	TIM2CH2.TIM_IT_CCX = TIM_IT_CC2; 
	TIM2CH3.TIM_IT_CCX = TIM_IT_CC3;
	TIM2CH4.TIM_IT_CCX = TIM_IT_CC4; 
	

}

//²¶»ñ×´Ì¬
//[7]:0,Ã»ÓÐ³É¹¦µÄ²¶»ñ;1,³É¹¦²¶»ñµ½Ò»´Î.
//[6]:0,»¹Ã»²¶»ñµ½¸ßµçÆ½;1,ÒÑ¾­²¶»ñµ½¸ßµçÆ½ÁË.
//[5:0]:²¶»ñ¸ßµçÆ½ºóÒç³öµÄ´ÎÊý(¶ÔÓÚ32Î»¶¨Ê±Æ÷À´Ëµ,1us¼ÆÊýÆ÷¼Ó1,Òç³öÊ±¼ä:4294Ãë)
//u8  TIM5CH1_CAPTURE_STA=0;	//ÊäÈë²¶»ñ×´Ì¬		    				
//u32	TIM5CH1_CAPTURE_VAL;	//ÊäÈë²¶»ñÖµ(TIM2/TIM5ÊÇ32Î»)


//µ¥¸öÍ¨µÀµÄ×´²¶»ñÊÂ¼þ´¦Àí
void SingleChannelHandler(TIM_CAPTURE * tch )
{
	

	if((tch->CAPTURE_STA&0X80)==0)//»¹Î´³É¹¦²¶»ñ	
	{
		if(TIM_GetITStatus(tch->timX, TIM_IT_Update) != RESET)//Òç³ö
		{	     
			if(tch->CAPTURE_STA&0X40)//ÒÑ¾­²¶»ñµ½¸ßµçÆ½ÁË
			{
				if((tch->CAPTURE_STA&0X3F)==0X3F)//¸ßµçÆ½Ì«³¤ÁË
				{
					tch->CAPTURE_STA|=0X80;		//±ê¼Ç³É¹¦²¶»ñÁËÒ»´Î
					tch->CAPTURE_VAL=0XFFFFFFFF;
				}else tch->CAPTURE_VAL++;
			}	 
		}
		if(TIM_GetITStatus(tch->timX, tch->TIM_IT_CCX) != RESET)//²¶»ñ1·¢Éú²¶»ñÊÂ¼þ
		{	
			if(tch->CAPTURE_STA&0X40)		//ÒÑ¾­²¶»ñµ½¸ßµçÆ½£¬±¾´Î²¶»ñµ½ÏÂ½µÑØ		
			{	  			
				tch->CAPTURE_STA|=0X80;		//±ê¼Ç³É¹¦²¶»ñµ½Ò»´Î¸ßµçÆ½Âö¿í
			  //tch->CAPTURE_VAL=TIM_GetCapture1(tch->timX);//»ñÈ¡µ±Ç°µÄ²¶»ñÖµ.
				
				switch( tch->TIM_IT_CCX ) // ¸ù¾Ý²»Í¬Í¨µÀÑ¡Ôñº¯Êý
				{
					case TIM_IT_CC1:						
						tch->CAPTURE_VAL=TIM_GetCapture1(tch->timX);//»ñÈ¡µ±Ç°µÄ²¶»ñÖµ.
						TIM_OC1PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 ÉèÖÃÎªÉÏÉýÑØ²¶»ñ
						break;
					case TIM_IT_CC2:
						tch->CAPTURE_VAL=TIM_GetCapture2(tch->timX);//»ñÈ¡µ±Ç°µÄ²¶»ñÖµ.
						TIM_OC2PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 ÉèÖÃÎªÉÏÉýÑØ²¶»ñ
						break;
					case TIM_IT_CC3:
						tch->CAPTURE_VAL=TIM_GetCapture3(tch->timX);//»ñÈ¡µ±Ç°µÄ²¶»ñÖµ.
						TIM_OC3PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 ÉèÖÃÎªÉÏÉýÑØ²¶»ñ
						break;
					case TIM_IT_CC4:
						tch->CAPTURE_VAL=TIM_GetCapture4(tch->timX);//»ñÈ¡µ±Ç°µÄ²¶»ñÖµ.
						TIM_OC4PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 ÉèÖÃÎªÉÏÉýÑØ²¶»ñ
						break;
					default:
						break;
				}
				//printf(" tch->TIM_IT_CCX = %d ",tch->TIM_IT_CCX); //´òÓ¡×ÜµÄ¸ßµãÆ½Ê±¼ä
	 			//TIM_OC1PolarityConfig(tch->timX,TIM_ICPolarity_Rising); //CC1P=0 ÉèÖÃÎªÉÏÉýÑØ²¶»ñ

			}else  								//»¹Î´¿ªÊ¼,µÚÒ»´Î²¶»ñÉÏÉýÑØ
			{
				tch->CAPTURE_STA=0;			//Çå¿Õ
				tch->CAPTURE_VAL=0;
				tch->CAPTURE_STA|= 0X40;		//±ê¼Ç²¶»ñµ½ÁËÉÏÉýÑØ
				TIM_Cmd(tch->timX,DISABLE ); 	//¹Ø±Õ¶¨Ê±Æ÷
	 			TIM_SetCounter(tch->timX,0);
				
//				printf(" tch->CAPTURE_STA = %d ",tch->CAPTURE_STA); //´òÓ¡×ÜµÄ¸ßµãÆ½Ê±¼ä
	 			//TIM_OC1PolarityConfig(tch->timX,TIM_ICPolarity_Falling);		//CC1P=1 ÉèÖÃÎªÏÂ½µÑØ²¶»ñ
				switch( tch->TIM_IT_CCX )
				{
					case TIM_IT_CC1:
						TIM_OC1PolarityConfig(tch->timX,TIM_ICPolarity_Falling); //CC1P=0 ÉèÖÃÎªÉÏÉýÑØ²¶»ñ
						break;
					case TIM_IT_CC2:
						TIM_OC2PolarityConfig(tch->timX,TIM_ICPolarity_Falling); //CC1P=0 ÉèÖÃÎªÉÏÉýÑØ²¶»ñ
						break;
					case TIM_IT_CC3:
						TIM_OC3PolarityConfig(tch->timX,TIM_ICPolarity_Falling); //CC1P=0 ÉèÖÃÎªÉÏÉýÑØ²¶»ñ
						break;
					case TIM_IT_CC4:
						TIM_OC4PolarityConfig(tch->timX,TIM_ICPolarity_Falling); //CC1P=0 ÉèÖÃÎªÉÏÉýÑØ²¶»ñ
						break;
					default:
						break;
				}
			
				TIM_Cmd(tch->timX,ENABLE ); 	//Ê¹ÄÜ¶¨Ê±Æ÷s
			}		    
		}			     	    					   
 	}
	TIM_ClearITPendingBit(tch->timX, tch->TIM_IT_CCX | TIM_IT_Update); //Çå³ýÖÐ¶Ï±êÖ¾Î»
}



//¶¨Ê±Æ÷5ÖÐ¶Ï·þÎñ³ÌÐò	 
void TIM5_IRQHandler(void)
{ 


//	status = TIM_GetITStatus(TIM5, TIM_IT_CC1 );
//	printf(" TIM_GetITStatus1 = %d ",status); //´òÓ¡×ÜµÄ¸ßµãÆ½Ê±¼ä
//	status = TIM_GetITStatus(TIM5, TIM_IT_CC2 );
//	printf(" TIM_GetITStatus2 = %d ",status); //´òÓ¡×ÜµÄ¸ßµãÆ½Ê±¼ä
//	status = TIM_GetITStatus(TIM5, TIM_IT_CC3 );
//	printf(" TIM_GetITStatus3 = %d ",status); //´òÓ¡×ÜµÄ¸ßµãÆ½Ê±¼ä
//	status = TIM_GetITStatus(TIM5, TIM_IT_CC4 );
//	printf(" TIM_GetITStatus4 = %d ",status); //´òÓ¡×ÜµÄ¸ßµãÆ½Ê±¼ä
//	status = TIM_GetITStatus(TIM5, TIM_IT_Update );
//	printf(" TIM_GetITStatus5 = %d ",status); //´òÓ¡×ÜµÄ¸ßµãÆ½Ê±¼ä
//////	
//	
//	
	TIM_ClearITPendingBit(TIM5, TIM_IT_Update); //Çå³ýÖÐ¶Ï±êÖ¾Î»
	if(TIM_GetITStatus(TIM5, TIM_IT_CC1) != RESET) 
   {								  
     
		 /* Clear TIM3 Capture compare interrupt pending bit */
     
		  SingleChannelHandler( &TIM5CH1);
		 
	 }
	 	if(TIM_GetITStatus(TIM5, TIM_IT_CC3) !=RESET) 
   {								  
     
		  SingleChannelHandler( &TIM5CH3);
		 
	 }
	 	if(TIM_GetITStatus(TIM5, TIM_IT_CC4) !=RESET) 
   {								  
     
		  SingleChannelHandler( &TIM5CH4);
		 
	 }

}

void TIM2_IRQHandler(void)
{
	//	status = TIM_GetITStatus(TIM5, TIM_IT_CC1 );
//	printf(" TIM_GetITStatus1 = %d ",status); //´òÓ¡×ÜµÄ¸ßµãÆ½Ê±¼ä
//	status = TIM_GetITStatus(TIM2, TIM_IT_CC2 );
//	printf(" TIM2_GetITStatus2 = %d ",status); //´òÓ¡×ÜµÄ¸ßµãÆ½Ê±¼ä
//	status = TIM_GetITStatus(TIM2, TIM_IT_CC3 );
//	printf(" TIM2_GetITStatus3 = %d ",status); //´òÓ¡×ÜµÄ¸ßµãÆ½Ê±¼ä
//	status = TIM_GetITStatus(TIM2, TIM_IT_CC4 );
//	printf(" TIM2_GetITStatus4 = %d ",status); //´òÓ¡×ÜµÄ¸ßµãÆ½Ê±¼ä
//	status = TIM_GetITStatus(TIM2, TIM_IT_Update );
//	printf(" TIM2_GetITStatus5 = %d ",status); //´òÓ¡×ÜµÄ¸ßµãÆ½Ê±¼ä
	
	 TIM_ClearITPendingBit(TIM2, TIM_IT_Update); //Çå³ýÖÐ¶Ï±êÖ¾Î»
	if(TIM_GetITStatus(TIM2, TIM_IT_CC2) != RESET) 
   {								  
     
	     
		  SingleChannelHandler( &TIM2CH2);
		 
	 }
	 	if(TIM_GetITStatus(TIM2, TIM_IT_CC3) !=RESET) 
   {								  
     
		  SingleChannelHandler( &TIM2CH3);
		 
	 }
	 	if(TIM_GetITStatus(TIM2, TIM_IT_CC4) !=RESET) 
   {								  
     
		  SingleChannelHandler( &TIM2CH4);
		 
	 }

	
}

//void TIM1_BRK_TIM9_IRQHandler(void)
//{
//	
//	 SingleChannelHandler(TIM9CH1);
//	 SingleChannelHandler(TIM9CH2);
//	
//}

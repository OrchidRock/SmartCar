#include "SCCB.h"
#include "../systick_delay.h"
#include "../tm4c123gh6pm.h"

/*
-----------------------------------------------
   ����: ��ʼ��ģ��SCCB�ӿ�
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/


void SCCB_SID_GPIO_INPUT(void){
	volatile unsigned long delay;
        
    SYSCTL_RCGC2_R |= 0x00000002; // activate port B
    GPIO_PORTB_DIR_R &= ~0x20; // PB5 in
    GPIO_PORTB_DEN_R |= 0x24; // enable digital I/O on PB5
}
void SCCB_SID_GPIO_OUTPUT(){
	volatile unsigned long delay;
        
    SYSCTL_RCGC2_R |= 0x00000002; // activate port B
    GPIO_PORTB_DIR_R |= 0x20; // PB5 Out
    GPIO_PORTB_DEN_R |= 0x24; // enable digital I/O on PB5
    
}
/*
 * 
 * */
void SCCB_GPIO_Config(void){
	volatile unsigned long delay;
        
    SYSCTL_RCGC2_R |= 0x00000002; // activate port B
    delay = SYSCTL_RCGC2_R;
    
    GPIO_PORTB_CR_R |= 0x24; // allow change for PB5, PB2
    GPIO_PORTB_AMSEL_R &= ~0x24;
    GPIO_PORTB_DIR_R |= 0x24; // PB5 Out
    GPIO_PORTB_AFSEL_R &= ~0x24;
    GPIO_PORTB_PUR_R |= 0x24;
    GPIO_PORTB_DEN_R |= 0x24; // enable digital I/O on PB5
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFF0FF0FF)+ 
            0x00000000;
}

/*
-----------------------------------------------
   ����: start����,SCCB����ʼ�ź�
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void startSCCB(void)
{
    SCCB_SID_H();     //�����߸ߵ�ƽ
    systick_delay_us(500);

    SCCB_SIC_H();	   //��ʱ���߸ߵ�ʱ���������ɸ�����
    systick_delay_us(500);
 
    SCCB_SID_L();
    systick_delay_us(500);

    SCCB_SIC_L();	 //�����߻ָ��͵�ƽ��������������Ҫ
    systick_delay_us(500);

}
/*
-----------------------------------------------
   ����: stop����,SCCB��ֹͣ�ź�
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void stopSCCB(void)
{
    SCCB_SID_L();
    systick_delay_us(500);
 
    SCCB_SIC_H();	
    systick_delay_us(500);
  

    SCCB_SID_H();	
    systick_delay_us(500);
   
}

/*
-----------------------------------------------
   ����: noAck,����������ȡ�е����һ����������
   ����: ��
 ����ֵ: ��
-----------------------------------------------
*/
void noAck(void)
{
	
	SCCB_SID_H();	
	systick_delay_us(500);
	
	SCCB_SIC_H();	
	systick_delay_us(500);
	
	SCCB_SIC_L();	
	systick_delay_us(500);
	
	SCCB_SID_L();	
	systick_delay_us(500);

}

/*
-----------------------------------------------
   ����: д��һ���ֽڵ����ݵ�SCCB
   ����: д������
 ����ֵ: ���ͳɹ�����1������ʧ�ܷ���0
-----------------------------------------------
*/
unsigned char SCCBwriteByte(unsigned char m_data)
{
	unsigned char j,tem;

	for(j=0;j<8;j++) //ѭ��8�η�������
	{
		if((m_data<<j)&0x80)
		{
			SCCB_SID_H();	
		}
		else
		{
			SCCB_SID_L();	
		}
		systick_delay_us(500);
		SCCB_SIC_H();	
		systick_delay_us(500);
		SCCB_SIC_L();	
		systick_delay_us(500);

	}
	systick_delay_us(100);
	SCCB_SID_IN;/*����SDAΪ����*/
	systick_delay_us(500);
	SCCB_SIC_H();	
	systick_delay_us(100);
	if(SCCB_SID_STATE){tem=0;}   //SDA=1����ʧ�ܣ�����0}
	else {tem=1;}   //SDA=0���ͳɹ�������1
	SCCB_SIC_L();	
	systick_delay_us(500);	
        SCCB_SID_OUT;/*����SDAΪ���*/

	return (tem);  
}

/*
-----------------------------------------------
   ����: һ���ֽ����ݶ�ȡ���ҷ���
   ����: ��
 ����ֵ: ��ȡ��������
-----------------------------------------------
*/
unsigned char SCCBreadByte(void)
{
	unsigned char read,j;
	read=0x00;
	
	SCCB_SID_IN;/*����SDAΪ����*/
	systick_delay_us(500);
	for(j=8;j>0;j--) //ѭ��8�ν�������
	{		     
		systick_delay_us(500);
		SCCB_SIC_H();
		systick_delay_us(500);
		read=read<<1;
		if(SCCB_SID_STATE) 
		{
			read=read+1;
		}
		SCCB_SIC_L();
		systick_delay_us(500);
	}	
	return(read);
}

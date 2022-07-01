#include "SCCB.h"
#include "../systick_delay.h"
#include "../tm4c123gh6pm.h"

/*
-----------------------------------------------
   功能: 初始化模拟SCCB接口
   参数: 无
 返回值: 无
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
   功能: start命令,SCCB的起始信号
   参数: 无
 返回值: 无
-----------------------------------------------
*/
void startSCCB(void)
{
    SCCB_SID_H();     //数据线高电平
    systick_delay_us(500);

    SCCB_SIC_H();	   //在时钟线高的时候数据线由高至低
    systick_delay_us(500);
 
    SCCB_SID_L();
    systick_delay_us(500);

    SCCB_SIC_L();	 //数据线恢复低电平，单操作函数必要
    systick_delay_us(500);

}
/*
-----------------------------------------------
   功能: stop命令,SCCB的停止信号
   参数: 无
 返回值: 无
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
   功能: noAck,用于连续读取中的最后一个结束周期
   参数: 无
 返回值: 无
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
   功能: 写入一个字节的数据到SCCB
   参数: 写入数据
 返回值: 发送成功返回1，发送失败返回0
-----------------------------------------------
*/
unsigned char SCCBwriteByte(unsigned char m_data)
{
	unsigned char j,tem;

	for(j=0;j<8;j++) //循环8次发送数据
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
	SCCB_SID_IN;/*设置SDA为输入*/
	systick_delay_us(500);
	SCCB_SIC_H();	
	systick_delay_us(100);
	if(SCCB_SID_STATE){tem=0;}   //SDA=1发送失败，返回0}
	else {tem=1;}   //SDA=0发送成功，返回1
	SCCB_SIC_L();	
	systick_delay_us(500);	
        SCCB_SID_OUT;/*设置SDA为输出*/

	return (tem);  
}

/*
-----------------------------------------------
   功能: 一个字节数据读取并且返回
   参数: 无
 返回值: 读取到的数据
-----------------------------------------------
*/
unsigned char SCCBreadByte(void)
{
	unsigned char read,j;
	read=0x00;
	
	SCCB_SID_IN;/*设置SDA为输入*/
	systick_delay_us(500);
	for(j=8;j>0;j--) //循环8次接收数据
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

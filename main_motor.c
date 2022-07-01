// This is a project that tay to implements the smart car.
// 2016-12-1
// 2022-5-20

#include "tm4c123gh6pm.h"
#include "ir.h"
#include "PLL.h"

#define CONTROLLER_VALUE_COUNT 9
#define SLOWDOWN_INDEX 0
#define RUN_INDEX 1
#define SPEEDUP_INDEX 2
#define TURNLEFT_INDEX 3
#define TURNRIGHT_INDEX 4
#define STOP_INDEX 5
#define REVERSAL_INDEX 6
#define AUTO_CONTROL 7
#define NO_AUTO_CONTROL 8

#define ENABLE_AUTO_CONTROL() (GPIO_PORTA_IM_R |= 0x70)
#define DISABLE_AUTO_CONTROL() (GPIO_PORTA_IM_R &= ~0x70)

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);

void RGB_LED_Init(void); // Init the pin PF1,PF2,PF3

void Motor_Port_Init(void); // Init IN1,IN2,IN3,IN4 pin;ENA(PE4),ENB(PE5)
void Motor_Front_Init(void); // Init the front motor.(control turn left/right)
void Motor_Back_Init(void); // Init the back motor.(control speed and go forward or retreat)

void IR_Sensors_Init(void);
void DelayTimer_Init(void); // Timer1A;
void DelayTimer_Wait(unsigned long); 

void Motor_Back_Stop(void);
void Motor_Back_Run(void);
void Motor_Back_SlowDown(int);
void Motor_Back_SpeedUp(int);
void Motor_Back_Reversal(void);

void Turn_Left(void);
void Turn_Right(void);
void Turn_Stop(void);
/* ------------------------------------------------- */

static unsigned long MY_controler_value[]={
0,0,0,0,0,0,0,0,0
};

//void Switch_LED_Init(void);

unsigned long back_H, back_L;
unsigned long front_H, front_L;
int is_reversal = 1;
int init_controller_value_index = 0;
int turning = 0; // 0: no turn 1: turn_left 2:Turn right
int reversal = 0; 
int auto_control_mode = 1;


// ***** 3. Main function section *****
int main(void){
	PLL_Init(); // 80MHZ
    RGB_LED_Init();
    Motor_Port_Init();
	Motor_Front_Init();
	Motor_Back_Init();

	//IRremote_Init();
	//IR_Sensors_Init();
    EnableInterrupts();
    
    while(1){

         
		
		// waiting for IR single
		//WaitForInterrupt();
		
		//unsigned long data_value;
		//unsigned long count = 1<<15;
		//Turn_Stop();
		//unsigned long count2 = 1<<12;
		
        while(IRrecived()){
			unsigned long data_value;
			unsigned long count = 1<<15;
			data_value = getIRresult();
			
			//if(init_controller_value_index < CONTROLLER_VALUE_COUNT && data_value == REPEAT)
				//GPIO_PORTF_DATA_R |= 0x02; // PF1 blue
			if(init_controller_value_index < CONTROLLER_VALUE_COUNT){
				  if(data_value != REPEAT){
						MY_controler_value[init_controller_value_index++] = data_value;
						GPIO_PORTF_DATA_R |= 0x04; // PF2
					}
			}else{
				
				if(data_value == MY_controler_value[SLOWDOWN_INDEX]){
					Motor_Back_SlowDown(1);
					turning = 0;
			
				}else if(data_value == MY_controler_value[RUN_INDEX]){
					Motor_Back_Run();
					turning = 0;
				}
				else if(data_value == MY_controler_value[SPEEDUP_INDEX]){
					turning = 0;
					Motor_Back_SpeedUp(1);
				}else if(data_value == MY_controler_value[TURNLEFT_INDEX]){
					if(turning == 1){
						Turn_Stop();
						turning = 0;
					}else{
						turning = 1;
						Turn_Left();
					}
					//GPIO_PORTF_DATA_R |= 0x02; // PF1 RED
				}else if(data_value == MY_controler_value[TURNRIGHT_INDEX]){
					if(turning == 2){
						Turn_Stop();
						turning = 0;
					}else{
						turning = 2;
						Turn_Right();
					}
					//GPIO_PORTF_DATA_R |= 0x04; // PF2
				}else if(data_value == MY_controler_value[STOP_INDEX]){
					turning = 0;
					Motor_Back_Stop();
					Turn_Stop();
				}else if(data_value == MY_controler_value[REVERSAL_INDEX]){
					turning = 0;
					Motor_Back_Reversal();
				}else if(data_value == MY_controler_value[AUTO_CONTROL]){
					ENABLE_AUTO_CONTROL();
					Motor_Back_Stop();
					Motor_Back_Run();
					
				}else if(data_value == MY_controler_value[NO_AUTO_CONTROL]){
				 // quit auto-controler mode
						DISABLE_AUTO_CONTROL();
						Motor_Back_Stop();
						Turn_Stop();
				}else{
					;
				}
			}
			while(count){count --;}
			GPIO_PORTF_DATA_R &= ~0x0e;
			resumeIRremote();
			//break;
		}
  }
}

// ***** 4. Subroutines Section *****
void RGB_LED_Init(){
    volatile unsigned long delay;
	// port PF1
	SYSCTL_RCGC2_R |= 0x00000020; // activate port F
	delay = SYSCTL_RCGC2_R;
    GPIO_PORTF_CR_R |= 0x0e; // allow change for PF1,2,3
	GPIO_PORTF_AMSEL_R &= ~0x0e;
	GPIO_PORTF_DIR_R |= 0x0e; // PF1,2,3 out
	GPIO_PORTF_AFSEL_R &= ~0x0e;
	GPIO_PORTF_DEN_R |= 0x0e; // enable digital I/O on PF1,2,3
	GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFF000F)+
												 0x00000000;		
    while(delay--){};
}


void Motor_Port_Init(void){
    volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x00000008; // activate port D
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTD_DIR_R |= 0x0f; // PD0,1,2,3 output
	GPIO_PORTD_AFSEL_R &= ~0x0f;
	//GPIO_PORTD_DR8R_R |= 0x0f;
	GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF0000)+
												 0x00000000;
	GPIO_PORTD_AMSEL_R &= ~0x0f ; // disable analog function
	GPIO_PORTD_DEN_R |= 0x0f; // enable digital I/O on PD0,1,2,3,4
	GPIO_PORTD_DATA_R &= ~0x0f; // make PD0,1,2,3,4 low
    
    while(delay--){};
	//SysTick_Period_Init(L);

	SYSCTL_RCGC2_R |= 0x00000010; // activate port E
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTE_DIR_R |= 0x20;    // PE5 output
    GPIO_PORTE_AFSEL_R &= ~0x20;
	//GPIO_PORTE_DR8R_R |= 0x20;
	GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R & 0xFF0FFFFF)
										+ 0x00000000;
	GPIO_PORTE_AMSEL_R &= ~0x20;
	GPIO_PORTE_DEN_R |= 0x20; // enable digital I/O on PE5
	GPIO_PORTE_DATA_R |= 0x20; // disable
    while(delay--){};

}
// PE5 // Timer0A
void Motor_Front_Init(){
	front_L = 500;
	front_H = 80000 - front_L;
    // Init Timer0A;
	SYSCTL_RCGCTIMER_R |= 0x01; // activeate timer0
    TIMER0_CTL_R &= ~0x00000001;
    TIMER0_CFG_R = 0x00000000;
    TIMER0_TAMR_R = 0x00000002;// periodic mode
    TIMER0_TAILR_R =front_L - 1;
    TIMER0_TAPR_R = 0x0;
    TIMER0_ICR_R = 0x00000001;
    TIMER0_IMR_R = 0x00000001;
    NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | 0x40000000;
    // priority 2
    NVIC_EN0_R |= 1<<19; // IRQ 19
    //TIMER0_CTL_R = 0x00000001;

    GPIO_PORTD_DATA_R &= ~0x0c; // make PD2(IN3),PD3(IN4) low

}// PE4 // SysTick
void Motor_Back_Init(){
	back_L = 72000;
	back_H = 80000 - back_L;

	NVIC_ST_CTRL_R = 0;		 // disable SysTick during setup
	NVIC_ST_RELOAD_R = back_L - 1;
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF) | 0x40000000; // priority 2
	//NVIC_ST_CTRL_R = 0x07; // enable SysTick with core clock and interrupts
	GPIO_PORTD_DATA_R &= ~0x03; // make PD0(IN1),PD1(IN2) low
}
void IR_Sensors_Init(){
    volatile unsigned long delay;
	// Init PA4,PA5,PA6 in
	SYSCTL_RCGC2_R |= 0x00000001; // activate port A
	delay = SYSCTL_RCGC2_R;
	GPIO_PORTA_CR_R |= 0xf0; // allow change for PA4,PA5,PA6,PA7
	GPIO_PORTA_AMSEL_R &= ~0xf0;
	GPIO_PORTA_DIR_R &= ~0xf0; // PA4,5,6,7 in
	GPIO_PORTA_AFSEL_R &= ~0xf0;
	GPIO_PORTA_DEN_R |= 0xf0; // enable digital I/O on PA4,5,6,7
	GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R&0x0000FFFF)+ 
    
													0x00000000;
    GPIO_PORTA_PUR_R |= 0xf0;
    
	// Conf Interrupt
	GPIO_PORTA_IS_R &= ~0xf0; // PA4,5,6,7 is edge-sensitive
	GPIO_PORTA_IBE_R |= 0xf0; // PA4,5,6,7 is both edge
	//GPIO_PORTA_IEV_R &=~0x70; // PA4,5,6,7 falling edge event
	GPIO_PORTA_ICR_R = 0xf0; // clear flag 4
	GPIO_PORTA_IM_R |= 0xf0; // arm interrupt on PA4,6,7
	NVIC_PRI0_R = (NVIC_PRI0_R & 0xFFFFFF00) | 0x00000020;
	// priority 1
	NVIC_EN0_R |= 1; // enable IRQ 0
	
    while(delay--){};
	
    DelayTimer_Init();
	
}
// Timer 1A
void DelayTimer_Init(){
	SYSCTL_RCGCTIMER_R |= 0x02; // activeate timer1A
    TIMER1_CTL_R &= ~0x00000001;
    TIMER1_CFG_R = 0x00000000;
    TIMER1_TAMR_R = 0x00000002;// periodic mode
    TIMER1_TAILR_R = 1;
    TIMER1_TAPR_R = 0x0;
    TIMER1_ICR_R = 0x00000001;
    TIMER1_IMR_R = 0x00000001;
    NVIC_PRI5_R = (NVIC_PRI5_R & 0xFFFF00FF) | 0x00006000;
    // priority 3
    NVIC_EN0_R |= 1<<21; // IRQ 21
    //TIMER1_CTL_R = 0x00000001;
}
void DelayTimer_Wait(unsigned long delay){
	auto_control_mode = 0 ;
	TIMER1_TAILR_R = delay -1;
	TIMER1_CTL_R |= 0x00000001;
}

void Motor_Back_Stop(){
	NVIC_ST_CTRL_R = 0;		 // disable SysTick
	reversal = 0;
	GPIO_PORTD_DATA_R &= ~0x03; // make PD0(IN1),PD1(IN2) low
}
void Motor_Back_Run(){
	reversal = 0;
	NVIC_ST_CTRL_R = 0x07; // e`nable SysTick with core clock and interrupts
	GPIO_PORTD_DATA_R |= 0x01; // make PD0(IN1) high
	GPIO_PORTD_DATA_R &= ~0x02; // make PD1(IN2) low
}
void Motor_Back_SlowDown(int n){
	if(NVIC_ST_CTRL_R != 0x0){
		while((n--)>0){
		 if(back_L>8000) back_L = back_L - 8000; //slow down
		}
		 back_H = 80000 - back_L;
	}
}
void Motor_Back_SpeedUp(int n){
	if(NVIC_ST_CTRL_R != 0x0){
		 while((n--)>0){
			if(back_L<72000) back_L = back_L + 8000; // speed up
		 }
		 back_H = 80000 - back_L;
	}
}
void Motor_Back_Reversal(){
	reversal = 1;
	NVIC_ST_CTRL_R = 0x07; // enable SysTick with core clock and interrupts
	GPIO_PORTD_DATA_R &= ~0x01; // IN1 low
	GPIO_PORTD_DATA_R |= 0x02; // IN2 high
}

void Turn_Left(){
    GPIO_PORTD_DATA_R &= ~0x04; // PD2(IN3)
    GPIO_PORTD_DATA_R |= 0x08; // PD3 (IN4)
    TIMER0_CTL_R |= 0x00000001;
		turning =1;
}
void Turn_Right(){
    //TIMER0_CTL_R |= 0x00000001;
    GPIO_PORTD_DATA_R &= ~0x08; // PD3(IN4)
    GPIO_PORTD_DATA_R |= 0x04; // PD2 (IN3)
    TIMER0_CTL_R |= 0x00000001;
		turning =2;
}
void Turn_Stop(){
	TIMER0_CTL_R &= ~0x00000001;
	GPIO_PORTD_DATA_R &= ~0x0c; // make PD2(IN3),PD3(IN4) low
	turning = 0;
}

/*
void Switch_LED_Init(void){
	SYSCTL_RCGC2_R |= 0x00000020; // activate port F
	GPIO_PORTF_LOCK_R = 0x4C4F434B;
	GPIO_PORTF_CR_R |= 0x13; // allow change for PF4,PF0
	GPIO_PORTF_AMSEL_R &= ~0x13;
	GPIO_PORTF_DIR_R &= ~0x11; // PF4,PF0 in
	GPIO_PORTF_DIR_R |= 0x02; // PF1 out
	GPIO_PORTF_AFSEL_R &= ~0x13;
	GPIO_PORTF_DEN_R |= 0x13; // enable digital I/O on PF4 PF0
	GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFF0FF00)+
												 0x00000000;
	//GPIO_PORTF_PUL_R

	GPIO_PORTF_PUR_R |= 0x11; // enable weak pull-up
	GPIO_PORTF_IS_R &= ~0x11; // PF4,0 is edge-sensitive
	GPIO_PORTF_IBE_R &= ~0x11; // PF4,0 is not both edges
	GPIO_PORTF_IEV_R &= ~0x11; // PF4,0 falling edge event
	GPIO_PORTF_ICR_R = 0x11; // clear flag4
	GPIO_PORTF_IM_R |= 0x11; // arm interrupt on PF4,0
	NVIC_PRI7_R = (NVIC_PRI7_R & 0xFF00FFFF) | 0x00400000; // priority 2
  NVIC_EN0_R |= 0x40000000; // enable interrupt 30 in NVIC
}
*/
// PWD output to PD0,PD1
void SysTick_Handler(void){
    //return; 
    GPIO_PORTE_DATA_R ^= 0x00000020;
	NVIC_ST_RELOAD_R = back_L-1; // reload value for low phase
	/*
	unsigned long bitmap = 0x01 << reversal;
    if(GPIO_PORTD_DATA_R & bitmap){ // toggle ENA
		GPIO_PORTD_DATA_R &= ~bitmap;
        GPIO_PORTE_DATA_R 
		NVIC_ST_RELOAD_R = back_L-1; // reload value for low phase
	}else{
		GPIO_PORTD_DATA_R |= bitmap;
			NVIC_ST_RELOAD_R = back_H-1; // reload value for low phase
	}*/
}
// PWD output to PE5(ENB)
void Timer0A_Handler(){
   TIMER0_ICR_R = 0x00000001; // acknowledge
   if(GPIO_PORTE_DATA_R & 0x20){ // toggle ENB
		GPIO_PORTE_DATA_R &= ~0x20;
		TIMER0_TAILR_R = front_L-1; // reload value for low phase
	}else{
		GPIO_PORTE_DATA_R |= 0x20;
    TIMER0_TAILR_R = front_H-1; // reload value for low phase
	}
}
void Timer1A_Handler(){
	TIMER1_ICR_R = 0x00000001; // acknowledge
	auto_control_mode = 1;
	TIMER1_CTL_R &= ~0x01;
}
void GPIOPortA_Handler(void){
	int value = 0;
    

	if(GPIO_PORTA_RIS_R & 0x10){ // PA4
		GPIO_PORTA_ICR_R = 0x10;
    } 
	if(GPIO_PORTA_RIS_R & 0x20){ // PA5
		GPIO_PORTA_ICR_R = 0x20;
    }    
	if(GPIO_PORTA_RIS_R & 0x40){ // PA6
		GPIO_PORTA_ICR_R = 0x40;
    }    
	if(GPIO_PORTA_RIS_R & 0x80){ // PA7
		GPIO_PORTA_ICR_R = 0x80;
    }    
	
	//if(!auto_control_mode) return;
	
	// delay
	//count -- ;
	//while(count>0) return;
    

	value = (GPIO_PORTA_DATA_R & 0x10)>>4; //  PA4(FRONT), 
	value |= (GPIO_PORTA_DATA_R & 0x20)>>4; // PA5(RIGHT),
	value |= (GPIO_PORTA_DATA_R & 0x40)>>4; // PA6(LEFT)
	
    switch(value){
		case 0: // all have obstacle
			Turn_Stop();
			Motor_Back_Reversal(); // only retreat 
			break;
		case 1: // right and left have obstacle
			//Motor_Back_Reversal();
			Turn_Stop();
			Motor_Back_SpeedUp(2);
			break;
		case 2: // front and left have obstacle
			// nothing
			Motor_Back_Reversal();
			DelayTimer_Wait(80000000*2); // 2 s; 
			//while()
			break;
		case 3: // only left has obstacle
			Turn_Right();
			//Motor_Back_Run();
            Motor_Back_Reversal();
			break;
		case 4: // front and right have obstacle 
			Motor_Back_Reversal();
			DelayTimer_Wait(80000000*2); // 2 s; 
			break;
		case 5: // only right has obstacle 
			Turn_Left();
			Motor_Back_Run();
			break;
		case 6: // only front has obstacle
			Motor_Back_Reversal();
			Motor_Back_SpeedUp(2);
		
			Turn_Right();
			DelayTimer_Wait(80000000*2); // 2 s; 
			//Motor_Back_Run();
			break;
		case 7: // all none 
			if(is_reversal){
				if(turning == 1) Turn_Right();
				else if(turning == 2) Turn_Left();
			}
			//Turn_Stop();
			//Motor_Back_Run();
			break;
	}
}

/*
//L range 8000,16000,24000,32000,40000,48000,56000,64000,72000
// power  10%, 20%,  30%,  40%,  50%,   60%,  70%, 80%,  90%
void GPIOPortF_Handler(void){
	if(GPIO_PORTF_RIS_R & 0x01){  //SW2 touch
		 GPIO_PORTF_ICR_R = 0x01; // acknowledge
		 if(L>8000) L = L - 8000; //slow down
	}
	if(GPIO_PORTF_RIS_R & 0x10){ // Sw1 touch
		GPIO_PORTF_ICR_R = 0x10; // acknowledge
		 if(L<72000) L = L + 8000; // speed up
	}
	H = 80000 - L;
}
*/

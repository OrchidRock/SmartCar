#include "uart1.h"
#include "tm4c123gh6pm.h"
/*
 * RX : PB0 
 * TX : PB1
 * */

void UART1_Init(void){ 

        SYSCTL_RCGC1_R |= SYSCTL_RCGC1_UART1; // activate UART1
        SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // activate port B
        
        while((SYSCTL_PRGPIO_R&0x02) == 0){};
        
        UART1_CTL_R &= ~UART_CTL_UARTEN;      // disable UART
        UART1_IBRD_R = 43;                    // IBRD = int(80,000,000 / (16 * 115200)) = int(43.402778)
        UART1_FBRD_R = 26;                    // FBRD = round(0.402778 * 64) = 26
        // 8 bit word length (no parity bits, one stop bit, FIFOs)
        UART1_LCRH_R = (UART_LCRH_WLEN_8|UART_LCRH_FEN);
        UART1_CTL_R |= UART_CTL_UARTEN;       // enable UART
        GPIO_PORTB_AFSEL_R |= 0x03;           // enable alt funct on PB1,PB0
        GPIO_PORTB_DEN_R |= 0x03;             // enable digital I/O on PB1,PB0
        // configure PB1,PB0 as UART1
        GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R&0xFFFFFF00)+0x00000011;
        GPIO_PORTB_AMSEL_R &= ~0x03;          // disable analog functionality on PB1,PB0
}

unsigned char UART1_InChar(void){
        // as part of Lab 11, modify this program to use UART1 instead of UART1
        while((UART1_FR_R&UART_FR_RXFE) != 0);
        return((unsigned char)(UART1_DR_R&0xFF));
}

//------------UART_InCharNonBlocking------------
// Get oldest serial port input and return immediately
// if there is no data.
// Input: none
// Output: ASCII code for key typed or 0 if no character
unsigned char UART1_InCharNonBlocking(void){
        // as part of Lab 11, modify this program to use UART1 instead of UART1
        if((UART1_FR_R&UART_FR_RXFE) == 0){
                return((unsigned char)(UART1_DR_R&0xFF));
        } else{
                return 0;
        }
}

//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART1_OutChar(unsigned char data){
        // as part of Lab 11, modify this program to use UART1 instead of UART1
        while((UART1_FR_R&UART_FR_TXFF) != 0);
        UART1_DR_R = data;
}

//------------UART_InUDec------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 32-bit unsigned number
//     valid range is 0 to 4294967295 (2^32-1)
// Input: none
// Output: 32-bit unsigned number
// If you enter a number above 4294967295, it will return an incorrect value
// Backspace will remove last digit typed
unsigned long UART_InUDec(void){
        unsigned long number=0, length=0;
        char character;
        character = UART1_InChar();
        while(character != CR){ // accepts until <enter> is typed
                // The next line checks that the input is a digit, 0-9.
                // If the character is not 0-9, it is ignored and not echoed
                if((character>='0') && (character<='9')) {
                        number = 10*number+(character-'0');   // this line overflows if above 4294967295
                        length++;
                        UART1_OutChar(character);
                }
                // If the input is a backspace, then the return number is
                // changed and a backspace is outputted to the screen
                else if((character==BS) && length){
                        number /= 10;
                        length--;
                        UART1_OutChar(character);
                }
                character = UART1_InChar();
        }
        return number;
}
//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART_OutString(unsigned char buffer[]){
        // as part of Lab 11 implement this function

        int s=0;
        while(buffer[s]!='\0')
        {
                while((UART1_FR_R&UART_FR_TXFF) != 0);

                UART1_DR_R = buffer[s];
                s++;

        }


}

unsigned char String[10];
//-----------------------UART_ConvertUDec-----------------------
// Converts a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: store the conversion in global variable String[10]
// Fixed format 4 digits, one space after, null termination
// Examples
//    4 to "   4 "  
//   31 to "  31 " 
//  102 to " 102 " 
// 2210 to "2210 "
//10000 to "**** "  any value larger than 9999 converted to "**** "
void UART1_ConvertUDec(unsigned long n){
        // as part of Lab 11 implement this function
        char k;
        int s=3;
        int i;
        memset(String,'\0',sizeof(String));
        //bzero(String,sizeof(String));
        String[4]=32;
        if(n<10000){
                do{
                        k=48+n%10;
                        n=n/10;
                        String[s]=k;
                        s--;
                }while(n>0);
                while(s>=0){
                        String[s]=32;
                        s--;
                }
        } else{
                String[0]=32;
                for(i=0;i<4;i++)
                        String[i]=42;

        }
}

//-----------------------UART_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Fixed format 4 digits, one space after, null termination
void UART1_OutUDec(unsigned long n){
        UART1_ConvertUDec(n);     // convert using your function
        UART1_OutString(String);  // output using your function
}


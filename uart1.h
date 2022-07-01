#ifndef __UART1__NODEMCU__
#define __UART1__NODEMCU__

#define CR   0x0D
#define LF   0x0A
#define BS   0x08
#define ESC  0x1B
#define SP   0x20
#define DEL  0x7F


void UART1_Init(void);

//------------UART_InChar------------
// Wait for new serial port input
// Input: none
// Output: ASCII code for key typed
unsigned char UART1_InChar(void);

//------------UART_InCharNonBlocking------------
// Get oldest serial port input and return immediately
// if there is no data.
// Input: none
// Output: ASCII code for key typed or 0 if no character
unsigned char UART1_InCharNonBlocking(void);

//------------UART_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART1_OutChar(unsigned char data);

//------------UART_InUDec------------
// InUDec accepts ASCII input in unsigned decimal format
//     and converts to a 32-bit unsigned number
//     valid range is 0 to 4294967295 (2^32-1)
// Input: none
// Output: 32-bit unsigned number
// If you enter a number above 4294967295, it will return an incorrect value
// Backspace will remove last digit typed
unsigned long UART1_InUDec(void);

//------------UART_OutString------------
// Output String (NULL termination)
// Input: pointer to a NULL-terminated string to be transferred
// Output: none
void UART1_OutString(unsigned char buffer[]);


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
void UART1_ConvertUDec(unsigned long n);

//-----------------------UART_OutUDec-----------------------
// Output a 32-bit number in unsigned decimal format
// Input: 32-bit number to be transferred
// Output: none
// Fixed format 4 digits, one space after, null termination
void UART1_OutUDec(unsigned long n);


#endif

#include "2450addr.h"
#include "option.h"
#include <stdarg.h>

// Function Declaration
void Uart_Init(int baud);
void Uart_Printf(char *fmt,...);
void Uart_Send_String(char *pt);
void Uart_Send_Byte(int data);
char Uart_Get_Char();

void Uart_Init(int baud)
{
	int pclk;
	pclk = PCLK;
	
	// PORT GPIO initial
	rGPHCON &= ~(0xf<<4);
	rGPHCON |= (0xa<<4);	

	
	rUFCON1 = 0x0;
	rUMCON1 = 0x0;
	
	/* TODO : Line Control(Normal mode, No parity, One stop bit, 8bit Word length */
	rULCON1 = 0x3;

	/* TODO : Transmit & Receive Mode is polling mode  */
	rUCON1  = (1<<2)|(1);

	/* TODO : Baud rate 설정  */		
	rUBRDIV1= ((unsigned int)(PCLK/16./baud+0.5)-1 );
}

void Uart_Printf(char *fmt,...)
{
    va_list ap;
    char string[256];

    va_start(ap,fmt);
    vsprintf(string,fmt,ap);
    Uart_Send_String(string);
    va_end(ap);		
}

void Uart_Send_String(char *pt)
{
	while(*pt)
	{
		/* TODO : 문자 하나씩 증가시키면서 문자열 출력  */
		/*YOUR CODE HERE*/
		Uart_Send_Byte(*pt++);			
	}	
}

void Uart_Send_Byte(int data)
{
	if(data=='\n')
	{
		while(!(rUTRSTAT1 & 0x2));
		WrUTXH1('\r');
	}
	/* TODO : UTRSTAT1의 값을 확인하여 TX 버퍼가 비어있으면 문자 출력   */	
	/*YOUR CODE HERE*/
	while(!(rUTRSTAT1 & 0x2));	
	WrUTXH1(data);
}

char Uart_Get_Char()
{
	/* TODO : UTRSTAT1의 값을 확인하여 문자열 입력   */	
	/*YOUR CODE HERE*/
	while(!(rUTRSTAT1 & 0x1));	
	return RdURXH1();
}
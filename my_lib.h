#ifndef _MY_LIB_H_
#define _MY_LIB_H_

//Uart.c 
extern void Uart_Init(int baud);
extern void Uart_Printf(char *fmt,...);
extern void Uart_Send_String(char *pt);
extern void Uart_Send_Byte(int data);
extern char Uart_Get_Char(void);

// Lcd.c
extern void Lcd_Port_Init(void);
extern void Lcd_Init(void);
extern void NonPal_Lcd_Init(void);
extern void Palette_Init(void);
extern void Put_Pixel(unsigned long x, unsigned long y, unsigned long pal_addr);
extern void NonPal_Put_Pixel(int x, int y, int color);
extern void Lcd_Draw_BMP(int x, int y, const unsigned char *fp);

#endif

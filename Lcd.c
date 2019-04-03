/*
 * =====================================================================
 * NAME         : Lcd.c
 *
 * Descriptions : Main routine for S3C2450
 *
 * IDE          : GCC-4.1.0
 *
 * Modification
 *	  
 * =====================================================================
 */
#include <stdarg.h>
#include "2450addr.h"
#include "my_lib.h"
#include "option.h"
#include "./font/ENG8X16.H"


#define LCD_SIZE_X (480)
#define LCD_SIZE_Y (272)
#define CLKVAL  	13
#define HOZVAL 	   (LCD_SIZE_X-1)
#define LINEVAL    (LCD_SIZE_Y-1)

#define VBPD (10)	
#define VFPD (1)	
#define VSPW (1)	
#define HBPD (43)	
#define HFPD (1)	
#define HSPW (1)	

#define  NonPal_Fb   ((volatile unsigned short(*)[480]) FRAME_BUFFER)
#define  Fb   ((volatile unsigned long(*)[80]) FRAME_BUFFER)
//unsigned short int (* NonPal_Fb)[480]; 
//unsigned long (* Fb)[80];

#define MAKE_BUFFER(BUFFER) {\
	int xx=0, yy=0;\
	unsigned int tmp;\
	unsigned char tmpR, tmpG, tmpB;\
	bfType=*(unsigned short *)(fp+0);\
	bfSize=*(unsigned short *)(fp+2);\
	tmp=*(unsigned short *)(fp+4);\
	bfSize=(tmp<<16)+bfSize;\
	bfOffbits=*(unsigned short *)(fp+10);\
	biWidth=*(unsigned short *)(fp+18);\
	biHeight=*(unsigned short *)(fp+22);\
	biWidth2=(bfSize-bfOffbits)/biHeight;\
	for(yy=0;yy<biHeight;yy++)\
	{\
		for(xx=0;xx<biWidth;xx++)\
		{\
			tmpB=*(unsigned char *)(fp+bfOffbits+(biHeight-yy-1)*biWidth*3+xx*3+0);\
			tmpG=*(unsigned char *)(fp+bfOffbits+(biHeight-yy-1)*biWidth*3+xx*3+1);\
			tmpR=*(unsigned char *)(fp+bfOffbits+(biHeight-yy-1)*biWidth*3+xx*3+2);\
			tmpR>>=3;\
			tmpG>>=3;\
			tmpB>>=3;\
			if(xx<biWidth2)\
			{\
				(BUFFER)[y+yy][x+xx] = (unsigned short)((tmpR<<10)+(tmpG<<5)+(tmpB<<0));\
			}\
		}\
	}\
}

#define CLEAR_COOKIE {\
	int i,j;\
	for ( i = 176 ; i < 220 ; i ++)\
	{\
		for( j = 20; j < 76 ; j ++ )\
			NonPal_Put_Pixel(j, i ,fullscreen_buffer[i][j]);\
	}\
}

#define COPY(A,B) for(loop=0;loop<32;loop++) *(B+loop)=*(A+loop);


unsigned int fullscreen_buffer_1[272][480];
unsigned int fullscreen_buffer_2[272][480];
unsigned int fullscreen_buffer_3[272][480];
unsigned int background_buffer[272][480];
unsigned int char_buffer[44][44];
unsigned int Buffer_lim[272][480];
unsigned int Buffer_x[24][40];
unsigned int Buffer_o[24][40];
unsigned int Buffer_explain[120][200];
unsigned int Buffer_option[120][200];


unsigned int TIMER_buffer[272][480];


//Function Declaration
void Lcd_Port_Init(void);
void NonPal_Lcd_Init(void);
void NonPal_Put_Pixel(int x, int y, int color);
void Lcd_Draw_BMP(int x, int y, const unsigned char *fp);

static unsigned short bfType;
static unsigned int bfSize;
static unsigned int bfOffbits;
static unsigned int biWidth, biWidth2;
static unsigned int biHeight;

void Lcd_Port_Init(void)
{
	
	rGPCUDP = 0xffffffff; 
	rGPCCON &= ~(0xffffffff);
	rGPCCON |= 0xaaaa02aa; //GPC5,6,7 = output
	
	rGPDUDP = 0xffffffff; 
	rGPDCON &= ~(0xffffffff);
	rGPDCON |= 0xaaaaaaaa;
	
	/* GPG12 is setted as LCD_Display ON/OFF */
	//rGPGCON=rGPGCON&(~(3<<24))|(1<<24); //GPG12=OUTPUT
	rGPGCON &= ~(3<<24);
	rGPGCON |= (1<<24); //GPG12=OUTPUT

	rGPGDAT|=(1<<12);
	
	/* GPB0 backlight Enable */
	//rGPBCON=rGPBCON&(~(3<<0))|(1<<0);
	rGPBCON &= ~(3<<0);
	rGPBCON |= (1<<0);

	rGPBDAT|= (1<<0);
	
	
}

void NonPal_Lcd_Init()
{		
	rVIDW00ADD0B0 = FRAME_BUFFER;
	rVIDW00ADD1B0 = 0;
	rVIDW00ADD2B0 = (0<<13)|((LCD_SIZE_X*4*2)&0x1fff);

	rVIDW00ADD1B0 = 0+(LCD_SIZE_X*LCD_SIZE_Y);
	
	/* TO DO : setting for LCD control 
	* RGB I/F,PNRMODE(BGR),VCLK=9MHz,VCLK=Enable,CLKSEL=HCLK,CLKDIR=Divided,ENVID=disable
	*/
	rVIDCON0=(0x0<<22)+(0x1<<13)+((CLKVAL)<<6)+(1<<5)+(1<<4)+(0<<2);
	rVIDCON1= (1<<6)+(1<<5);
	
	rVIDTCON0=((VBPD)<<16)+((VFPD)<<8)+(VSPW);
	rVIDTCON1=((HBPD)<<16)+((HFPD)<<8)+(HSPW);
	rVIDTCON2=(LINEVAL<<11)+(HOZVAL);
	
	rVIDOSD0A		= 	(((0)&0x7FF)<<11) | (((0)&0x7FF)<<0);
	rVIDOSD0B	 	= 	(((LCD_SIZE_X-1)&0x7FF)<<11) | (((LCD_SIZE_Y-1)&0x7FF)<<0);

	rVIDOSD1A 		= 	(((0)&0x7FF)<<11) | (((0)&0x7FF)<<0);
	rVIDOSD1B	 	= 	(((LCD_SIZE_X-1)&0x7FF)<<11) | (((LCD_SIZE_Y-1)&0x7FF)<<0);
	/* TO DO 
	* Half swap Enable, 5:5:5:I format, Window0 ON
	*/
	rWINCON0=(0x1<<16)+(0x7<<2)+(1<<0); 
	
	/* TO DO : ENVID Enable for video output and LCD control signal */
	rVIDCON0 |= (3<<0);	
}

void NonPal_Put_Pixel(int x, int y, int color)
{
	/* TO DO : Fill FRAMEBUFFER with color value  */
	NonPal_Fb[y][x] = (unsigned short int)color;	
	
}

void Lcd_Draw_BMP(int x, int y, const unsigned char *fp)
{
     int xx=0, yy=0;	
     unsigned int tmp;
     unsigned char tmpR, tmpG, tmpB;
	
     bfType=*(unsigned short *)(fp+0);
     bfSize=*(unsigned short *)(fp+2);
     tmp=*(unsigned short *)(fp+4);
     bfSize=(tmp<<16)+bfSize;
     bfOffbits=*(unsigned short *)(fp+10);
     biWidth=*(unsigned short *)(fp+18);    
     biHeight=*(unsigned short *)(fp+22);    
     biWidth2=(bfSize-bfOffbits)/biHeight;	
     for(yy=0;yy<biHeight;yy++)
     {
         for(xx=0;xx<biWidth;xx++)
         {
             tmpB=*(unsigned char *)(fp+bfOffbits+(biHeight-yy-1)*biWidth*3+xx*3+0);
             tmpG=*(unsigned char *)(fp+bfOffbits+(biHeight-yy-1)*biWidth*3+xx*3+1);
             tmpR=*(unsigned char *)(fp+bfOffbits+(biHeight-yy-1)*biWidth*3+xx*3+2);
             tmpR>>=3;
             tmpG>>=3;
             tmpB>>=3;
             
             if(xx<biWidth2) NonPal_Put_Pixel(x+xx,y+yy,(tmpR<<10)+(tmpG<<5)+(tmpB<<0));
         } 
     }
    
}

void Lcd_Puts(int x, int y, int color, unsigned int (* buffer)[480], char *str)
{
    unsigned data;
  	unsigned offset,loop;
	unsigned char xs,ys;
	unsigned char temp[32];
	unsigned char bitmask[]={128,64,32,16,8,4,2,1};   
	while(*str)
	{
		data=*str++;
		offset=(unsigned)(data*16);
		COPY(eng8x16+offset,temp);
		
		for(ys=0;ys<16;ys++)
		{
			for(xs=0;xs<8;xs++)
			{
				if(temp[ys]&bitmask[xs])
				{
					NonPal_Put_Pixel(x+xs,y+ys,color);
				} 
				else
				{
					NonPal_Put_Pixel(x+xs,y+ys,buffer[y+ys][x+xs]);
				} 
			}
		}
		x+=8;
	}
} 


void Lcd_Printf(int x, int y, int color, unsigned int (* buffer)[480],char *fmt,...)
{
	va_list ap;
	char string[256];

	va_start(ap,fmt);
	vsprintf(string,fmt,ap);
	Lcd_Puts(x, y, color, buffer, string);
	va_end(ap);
}

void Lcd_Puts_big(int x, int y, int color, unsigned int (* buffer)[480], char *str)
{
    unsigned data;
   	unsigned offset,loop;
	unsigned char xs,ys;
	unsigned char temp[32];
	unsigned char bitmask[]={128,64,32,16,8,4,2,1};     
	
    while(*str)
    {
		data=*str++;
        offset=(unsigned)(data*16);
		COPY(eng8x16+offset,temp);
		
		for(ys=0;ys<16;ys++)
		{
			for(xs=0;xs<8;xs++)
			{
				if(temp[ys]&bitmask[xs])
				{
					NonPal_Put_Pixel(x+2*xs,y+2*ys+1,color);
					NonPal_Put_Pixel(x+2*xs+1,y+2*ys,color);
					NonPal_Put_Pixel(x+2*xs,y+2*ys,color);
					NonPal_Put_Pixel(x+2*xs+1,y+2*ys+1,color);
				}
				else
				{
					NonPal_Put_Pixel(x+2*xs,y+2*ys+1,buffer[y+ys][x+xs]);
					NonPal_Put_Pixel(x+2*xs+1,y+2*ys,buffer[y+ys][x+xs]);
					NonPal_Put_Pixel(x+2*xs,y+2*ys,buffer[y+ys][x+xs]);
					NonPal_Put_Pixel(x+2*xs+1,y+2*ys+1,buffer[y+ys][x+xs]);
				}	   	
			} 
		}
		x+=16;
	}	
} 


void Lcd_Buffer_back(int x, int y, const unsigned char *fp)
{
	MAKE_BUFFER(background_buffer)
}

void Lcd_Buffer_screen1(int x, int y, const unsigned char *fp)
{
	MAKE_BUFFER(fullscreen_buffer_1)
}
void Lcd_Buffer_screen2(int x, int y, const unsigned char *fp)
{
	MAKE_BUFFER(fullscreen_buffer_2)
}
void Lcd_Buffer_screen3(int x, int y, const unsigned char *fp)
{
	MAKE_BUFFER(fullscreen_buffer_3)
}
void Lcd_Buffer_char(int x, int y, const unsigned char *fp)
{
	MAKE_BUFFER(char_buffer)
}

void Lcd_Buffer_lim(int x, int y, const unsigned char *fp)
{
	MAKE_BUFFER(Buffer_lim)
}


void Lcd_Buffer_o(int x, int y, const unsigned char *fp)
{
	MAKE_BUFFER(Buffer_o)
}


void Lcd_Buffer_x(int x, int y, const unsigned char *fp)
{
	MAKE_BUFFER(Buffer_x)
}


void Lcd_Buffer_explain(int x, int y, const unsigned char *fp)
{
	MAKE_BUFFER(Buffer_explain)
}


void Lcd_Buffer_option(int x, int y, const unsigned char *fp)
{
	MAKE_BUFFER(Buffer_option)
}






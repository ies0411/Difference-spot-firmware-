
#include "2450addr.h"
#include "option.h"
#include "my_lib.h"

#define RUN_MINUTE 0
#define RUN_SECOND 0

#define BLACK	0x0000
#define WHITE	0xFFFF
#define BLUE		0x001F
#define GREEN	0x03E0
#define RED		0x7C00

extern unsigned int TIMER_buffer[272][480];
extern volatile  int ADC_x, ADC_y;

int Limit_min = 2;
int min;
int sec;
int temp;
int d_min;
int d_sec;


void Timer1_ISR(void) __attribute__((interrupt ("IRQ")));
void Timer1_ISR()
{	
	 rSRCPND1|=(0x1<<11);
	 rINTPND1|=(0x1<<11);
	
	rRTCCON = 0x1;
		min=rBCDMIN;
		sec=rBCDSEC;
		d_min =  (min & 0xf) + 10 * ((min>> 4) & 0x7) ;
		d_sec =  (sec & 0xf) + 10 * ((sec >> 4) & 0x7) ;

		if(temp!=sec){
			temp=sec;
			//Uart_Printf("%x : %x \n",min,sec);
			Lcd_Printf(200,10,WHITE, TIMER_buffer,"%02d:%02d",Limit_min-1-d_min,59-d_sec);
		}
	
}



void Rtc_Init(void){
	rRTCCON |= (0x1);
	rRTCCON |= (5<<5);

	rBCDMIN &= ~(0x7f);
	rBCDMIN |= RUN_MINUTE;

	rBCDSEC &= ~(0x7f);
	rBCDSEC |= RUN_SECOND;
		
	rRTCCON = 0x0;
	pISR_TIMER1=(unsigned int)Timer1_ISR;	
}


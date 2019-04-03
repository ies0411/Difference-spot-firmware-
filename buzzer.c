#include "2450addr.h"
#include "option.h"
//#include "macro.h"

// for GPH10

#define BASE10	10000

#define TONE_BEEP		1000
#define DURATION_5SEC	5000
#define DURATION_1SEC	1000
#define DURATION_05SEC	 500

#define C1      523     // Do
#define C1_     554
#define D1      587     // Re
#define D1_     622
#define E1      659     // Mi
#define F1      699     // Pa
#define F1_     740
#define G1      784     // Sol
#define G1_     831
#define A1      880     // La
#define A1_     932
#define B1      988     // Si
#define C2      C1*2    // Do
#define C2_     C1_*2
#define D2      D1*2    // Re
#define D2_     D1_*2
#define E2      E1*2    // Mi
#define F2      F1*2    // Pa
#define F2_     F1_*2
#define G2      G1*2    // Sol
#define G2_     G1_*2
#define A2      A1*2    // La
#define A2_     A1_*2
#define B2      B1*2    // Si

static int Buzzer_flag;

void Timer3_ISR(void) __attribute__((interrupt ("IRQ")));
void Timer3_ISR()
{	
	 rSRCPND1|=(0x1<<13);
	 rINTPND1|=(0x1<<13);
	 	Uart_Printf("timer3");
		int temp;
		if(Buzzer_flag==0){
			rGPBDAT &= ~(0x1<<1);
			Buzzer_flag=1;
		}
		else{
			rGPBDAT |= (0x1<<1);
			Buzzer_flag=0;
		}
}

void Buzzer_Init(void)
{
	// Buzzer = GPB1
	rGPBDAT |= (0x1<<1);
	rGPBCON &= ~(0x3 << 2);
	rGPBCON |= 0x1<<2;
}

void Buzzer_timer(void){
	pISR_TIMER3=(unsigned int)Timer3_ISR;	
	rINTMSK1 &= ~(0x1<<13);
	Timer_Delay_3(20);

}

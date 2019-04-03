#include "2450addr.h"
#include "my_lib.h"
#include "option.h"

int menu_select ;

void Key_IRQ_Port_Init(void)
{

	/* TO DO :  GPFCON[6:2] = Interrupt mode (10) */ 
	//rGPFDAT |= (0x1f<<2);
	rGPFCON &= ~(0x3ff<<4);
	rGPFCON |= (0x2AA<<4);
	
	
	/* KEYOUT[0:1] = GPGCON[0],GPFCON[7]  : Output mode
	 *GPGCON[0],GPFCON[7]  : initial data is all 0 for falling interrupt
	 */
	rGPGDAT &= ~0x1;
	rGPFDAT	&= ~(0x1<<7);
	
	rGPGCON &= ~(0x3);
	rGPGCON |= 0x1;	
	rGPFCON &= ~(0x3<<14);
	rGPFCON |= 0x1<<14;
	
	/* TO DO :  EXINT0[26:8] = falling edge  */	
	rEXTINT0 &= ~((0x7<<8) | (0x7<<12) | (0x7<<16) | (0x7<<20) | (0x7<<24));
	rEXTINT0 |= ((0x2<<8) | (0x2<<12) |(0x2<<16) | (0x2<<20) | (0x2<<24));
}

int EINT2_(void) __attribute__((interrupt ("IRQ")));
int EINT2_(void){
	rSRCPND1|=(1<<2);
	rINTPND1|=(1<<2);
	if((rGPGDAT&0x1)==1)
	{
		Uart_Printf("button1\n");
		menu_select=1;
		int i;
		for(i=0;i<0xffffa;i++);
	}
	else
	{
		Uart_Printf("button6\n");
			menu_select=6;
	}
	
	
	
}



void EINT3_(void) __attribute__((interrupt ("IRQ")));
void EINT3_(void){
	 rSRCPND1|=(0x1<<3);
	 rINTPND1|=(0x1<<3);
	if((rGPGDAT&0x1)==1)
	{
		Uart_Printf("button2\n");
		menu_select=2;
		int i;
		for(i=0;i<0xffffa;i++);
	}
	else
	{
		Uart_Printf("button7\n");
			menu_select=7;
	}
	
}


void EINT4_7(void) __attribute__((interrupt ("IRQ")));
void EINT4_7(void){
	 rSRCPND1|=(0x1<<4);
	 rINTPND1|=(0x1<<4);

	if((rGPGDAT&0x1)==1){
		//Uart_Printf("%x\n",rGPFDAT);
		 if(rEINTPEND&(0x1<<4)){
		 	rEINTPEND|=(0x1<<4);
		 	Uart_Printf("button3\n");
		 	menu_select=3;
		 	int i;
			for(i=0;i<0xffffa;i++);
		 }

		 else if(rEINTPEND&(0x1<<5)){
	 			rEINTPEND|=(0x1<<5);
		 	Uart_Printf("button4\n");
		 	menu_select=4;
		 	int i;
			for(i=0;i<0xffffa;i++);
	
		 }
		 else if(rEINTPEND&(0x1<<6)){
		 		rEINTPEND|=(0x1<<6);
		 	Uart_Printf("button5\n");
		 	menu_select=5;
		 	int i;
			for(i=0;i<0xffffa;i++);
		 	
		 }
	}

	else {
			if(rEINTPEND&(0x1<<4)){
		 	rEINTPEND|=(0x1<<4);
		 	Uart_Printf("button8\n");
		 		menu_select=8;
		 }

		 else if(rEINTPEND&(0x1<<5)){
	 			rEINTPEND|=(0x1<<5);
		 	Uart_Printf("button9\n");
		 }
		 else if(rEINTPEND&(0x1<<6)){
		 		rEINTPEND|=(0x1<<6);
		 	Uart_Printf("button10\n");
		 }
	}
}


void Timer0_ISR(void) __attribute__((interrupt ("IRQ")));
void Timer0_ISR()
{	
	 rSRCPND1|=(0x1<<10);
	 rINTPND1|=(0x1<<10);
	//Uart_Printf("timer");
static int flag ;
	
		if(flag){
			flag = 0;
				rGPGDAT &=~(0x1);
				rGPFDAT |=(0x1<<7);
		}else{
			flag = 1;
			rGPGDAT |=0x1;
			rGPFDAT &=~(0x1<<7);
		}
}


int MENU(void){
	Timer_Init();
	Key_IRQ_Port_Init();
	rINTMSK1 &= ~(0x1<<10);	
	rINTMSK1 &=~(1<<2);
	rINTMSK1 &=~(1<<3);
	rINTMSK1 &=~(1<<4);
	rEINTMASK &=~(0x7<<4);
	pISR_TIMER0=(unsigned int)Timer0_ISR;
	pISR_EINT2=(unsigned int)EINT2_;
	pISR_EINT3=(unsigned int)EINT3_;
	pISR_EINT4_7=(unsigned int)EINT4_7;
	Timer_Delay_0(10);
	while(1){
		if(menu_select!=0) return menu_select;
	}

}
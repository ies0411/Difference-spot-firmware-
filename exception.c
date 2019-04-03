#include "2450addr.h"
#include "macro.h"

void Exception_Init(void);
void Undef_Handler(void);
void Pabort_Handler(void);
void Dabort_Handler(void);
void SWI_Handler(void);

void Timer2_ISR(void) __attribute__((interrupt ("IRQ")));
void Touch_ISR(void) __attribute__ ((interrupt ("IRQ")));

extern unsigned int  HandleUndef;
extern unsigned int  HandleSWI;
extern unsigned int  HandlePabort;
extern unsigned int  HandleDabort;

extern unsigned int  HandleADC;

volatile  int ADC_x, ADC_y;
volatile  int Touch_Pressed;

void Uart_Printf(const char *fmt,...);

void SWI_Handler(void)
{
	Uart_Printf("SWI exception.\n");
	for(;;);
}

void Undef_Handler(void) 
{
	Uart_Printf("Undefined instruction exception.\n");
	for(;;);
}

void Pabort_Handler(void)
{
	volatile int prefetch_addr=0;
	
	 __asm volatile(
		"mov %[retval],lr\n"
		"sub %[retval],%[retval],#4\n"
		: [retval] "=r" (prefetch_addr) : :
	);	
	Uart_Printf("Pabort exception in address 0x%08x.\n", prefetch_addr);		
	for(;;);
}

void Dabort_Handler(void)
{
	volatile int data_addr=0;

	 __asm volatile(
		"mov %[retval1],lr\n"
		"sub %[retval1],%[retval1],#8\n"
		: [retval1] "=r" (data_addr) : :
	);	
	Uart_Printf("Dabort exception in address 0x%08x.\n", data_addr);				
	for(;;);
}


//>>>>>>>>>>>>>>>ADC TOUCH>>>>>>>>>>>>
void Touch_ISR()
{
	/* ÀÎÅÍ·´Æ® Çã¿ëÇÏÁö ¾ÊÀ½ on Touch */
	rINTSUBMSK |= (0x1<<9);
	rINTMSK1 |= (0x1<<31);	
	
	/* TO DO: Pendng Clear on Touch */	
	rSUBSRCPND |= (0x1<<9);
	rSRCPND1 |= (0x1<<31);
	rINTPND1 |= (0x1<<31);

	if(rADCTSC & 0x100)
	{
		rADCTSC &= (0xff); 
		Touch_Pressed = 0;
		Uart_Send_String("Detect Stylus Up Interrupt Signal \n");
	}
	
	else
	{
			
		rADCTSC &=~(0x1<<8);				//detect stylus down
		rADCTSC &= ~(0x1<<7);				//set bit for x position measurement
		rADCTSC |= (0x1<<6);				//YM=Hi-z, YP=Hi-z
		rADCTSC |= (0x1<<5);		
		rADCTSC &= ~(0x1<<4);				//XM=VSS,XP=VDD
		rADCTSC &= ~(0x1<<3);				//pull-up EN
		rADCTSC &= ~(0x1<<2);				//Normal-ADC
		rADCTSC &= ~(0x3);
		rADCTSC |= (0x1);					//X-position = 1


		/* TO DO : ENABLE_START */		
		rADCCON |= (0x1);
		
		/* wait until End of A/D Conversion */
		while(!(rADCCON & (1<<15)));
		rADCCON &= ~(0x1);					//Stop Conversion

		/*Set ADCTSC reg for Y Conversion*/ 
		ADC_x = (rADCDAT0 & 0x3ff);			//Store X value
		
		rADCTSC |= (0x1<<7);				//YM=VSS, YP=VDD
		rADCTSC &= ~(0x1<<6);
		rADCTSC &= ~(0x1<<5);				//XM=Hi-z, XP=Hi-z
		rADCTSC |= (0x1<<4);
		/* clear and then set  ADCTSC [1:0] for Y Conversion*/
		rADCTSC &= ~(0x3);
		rADCTSC |= (0x2);
	
		rADCCON |= (0x1);				//StartConversion
		while(!(rADCCON & (1<<15)));	//wait untill End of A/D Conversion

		ADC_y = (rADCDAT1 & 0x3ff);		//Store y value
		
		Touch_Pressed = 1;
		
		//revise the ACD 	
		ADC_x=480.0*(1-((ADC_x-180.0)/(860.0-180.0)));
		ADC_y=270.0*(1-((ADC_y-350.0)/(680.0-350.0)));
		
		//delete noise
		if(ADC_x<=0||ADC_y<=0||ADC_x>480||ADC_y>270) Touch_Pressed = 0;
		
		
		rADCTSC |= (0x1<<8);
		rADCTSC |= (0x1<<7);
		rADCTSC |= (0x1<<6);
		rADCTSC &= ~(0x1<<5);
		rADCTSC |= (0x1<<4);
		rADCTSC &= ~(0x1<<3);
		rADCTSC &= ~(0x1<<2);
		rADCTSC |= (0x1<<1);
		rADCTSC |= (0x1);

		Uart_Printf("X : %d, Y: %d \n", ADC_x, ADC_y);	
			int i;
			for(i=0;i<0xff;i++);
	}
	
	/* ÀÎÅÍ·´Æ® ´Ù½Ã Çã¿ë  on Touch */
	rINTSUBMSK &= ~(0x1<<9);
	rINTMSK1 &= ~(0x1<<31);
	
}

void Exception_Init(void)
{
	HandleUndef  = (unsigned)Undef_Handler;
	HandleSWI    = (unsigned)SWI_Handler;
	HandlePabort = (unsigned)Pabort_Handler;
	HandleDabort = (unsigned)Dabort_Handler;

	HandleADC = (unsigned)Touch_ISR;
	


	// Disable Interrupt
	rINTMOD1   = 0x0;
	rINTMSK1    = BIT_ALLMSK;		
	rINTMOD2   = 0x0;
	rINTMSK2    = BIT_ALLMSK;		

	// Clear All Previous Pending Bits
	rSRCPND1 = 0xffffffff;
	rSRCPND2 = 0xffffffff;
	rINTPND1 = 0xffffffff;  
}
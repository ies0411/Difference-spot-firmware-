#include "2450addr.h"

void Touch_Init(void);



void Touch_Init(void)
{
	rADCDLY = (50000); 
     /* TO DO : prescaler enable, prescaler value=39, Analog no input, 
      * 		Normal operation mode, Disable start, No operation */
		rADCCON |= (0x1<<14);
		rADCCON |= (39<<6);
		rADCCON &= ~(0x1<<3);
		rADCCON &= ~(0x1<<2);
		rADCCON &= ~(0x1<<1);
		rADCCON &= ~(0x1);

	  
	 
	 /* TO DO :  For Waiting Interrupt Mode rADCTSC=0xd3 */
		rADCTSC &= ~(0x1<<8);
		rADCTSC |= (0x1<<7);
		rADCTSC |= (0x1<<6);
		rADCTSC &= ~(0x1<<5);
		rADCTSC |= (0x1<<4);
		rADCTSC &= ~(0x1<<3);
		rADCTSC &= ~(0x1<<2);
		rADCTSC |= (0x1<<1);
		rADCTSC |= (0x1);

}


/*
 * =====================================================================
 * NAME         : IM EUNSOO
 *
 * Descriptions : Main routine for S3C2450
 *
 * IDE          : GCC-4.1.0
 *
 * Modification
 *	  
 * =====================================================================
 */
#include "2450addr.h"
#include "my_lib.h"
#include "option.h"

//image
#include "./Images/background.h"
#include "./Images/limit_time.h"
#include "./Images/Explain.h"
#include "./Images/PIC0.h"
#include "./Images/PIC1.h"
#include "./Images/PIC2.h"
#include "./Images/RUN.H"
#include "./Images/lim.h"

#include "./Images/o.h"
#include "./Images/x.h"

#include <stdlib.h>

//LCD image
#define BLACK	0x0000
#define WHITE	0xFFFF
#define BLUE	0x001F
#define GREEN	 0x03E0
#define RED 0x7C00

#define  NonPal_Fb   ((volatile unsigned short(*)[480]) FRAME_BUFFER)
#define  Fb   ((volatile unsigned long(*)[80]) FRAME_BUFFER)

//ranking num
extern int Lanking_num[3];
extern int Sum_num;

//time rtc
extern int Limit_min ;
extern int d_min;
extern int d_sec;

//LCD buffer & touch
extern volatile  int Touch_Pressed;
extern unsigned int TIMER_buffer[272][480];
extern unsigned int fullscreen_buffer_1[272][480];
extern unsigned int fullscreen_buffer_2[272][480];
extern unsigned int fullscreen_buffer_3[272][480];
extern unsigned int background_buffer[272][480];
extern unsigned int char_buffer[44][44];
extern unsigned int Buffer_lim[272][480];
extern unsigned int Buffer_x[24][40];
extern unsigned int Buffer_o[24][40];
extern unsigned int Buffer_explain[120][200];
extern unsigned int Buffer_option[120][200];

extern int menu_select ;


void Init_LED(void){
	rGPGCON &=~(0xff<<8);
	rGPGCON |=(0x55<<8);
	rGPGDAT |= (0xf<<4);
}

void Option(void){
	int i,j;
	int x=0,y=0;
	menu_select=0;
	
	for(i=80;i<200;i++){
		for(j=150;j<350;j++){
			NonPal_Put_Pixel(j, i ,Buffer_option[y][x]);
			x++;
		}
			x = 0;
			y++;
	}
	//Lcd_Draw_BMP(150,80,limit_time);
	int select=MENU();
	if(select==3) Limit_min=1;
		else if(select==4) Limit_min=2;
			else if(select==5) Limit_min=3;
}

void Explain_fuction(void){
	//Lcd_Draw_BMP(150, 80, Explain);
	Touch_Pressed=0;
	int i,j;
	int x=0,y=0;
	for(i=80;i<200;i++){
		for(j=150;j<350;j++){
			NonPal_Put_Pixel(j, i ,Buffer_explain[y][x]);
			x++;
		}
			x = 0;
			y++;
	}
	while(!Touch_Pressed);

}


void Main(void) {
	int i,j;
	Lcd_Port_Init();
	NonPal_Lcd_Init();

	for(i=0;i<480;i++){
		for(j=0;j<272;j++){
			NonPal_Put_Pixel(i, j, BLACK);
		}
	}

	Lcd_Buffer_back(0,0,background);
	Lcd_Buffer_char(0,0,run);
	Lcd_Buffer_lim(0,0,lim);
	Lcd_Buffer_screen1(0,0,PIC0);
	Lcd_Buffer_screen2(0,0,PIC1);
	Lcd_Buffer_screen3(0,0,PIC2);
	Lcd_Buffer_o(0,0,o);
 	Lcd_Buffer_x(0,0,x);
 	Lcd_Buffer_explain(0,0,Explain);
	Lcd_Buffer_option(0,0,limit_time);

while(1){
	Buzzer_Init();
	Touch_Init();
	Init_LED();
	
	rINTMSK1 &= ~(0x1<<31); 
	rINTSUBMSK &= ~(0x1<<9);
	int game1, game2 ,game3;
		
	for(i=0;i<480;i++){
		for(j=0;j<272;j++){
			NonPal_Put_Pixel(i, j, background_buffer[j][i]);
		}
	}

	int start;

	start=MENU();
		if(start==1){
				game1=GAME1_();
				Sum_num+=d_min*60+d_sec;
				if(game1>0){
					game2=GAME2_();
					Sum_num+=d_min*60+d_sec;
					if(game2>0){
						game3=GAME3_();
						Sum_num+=d_min*60+d_sec;
						if(game3>0){
							Sum_num+=d_min*60+d_sec; End_Sucess();Who_fuction();
							while(!Touch_Pressed);
							menu_select=0;
						}
						else {
							Sum_num+=d_min*60+d_sec; End_fail();Who_fuction();
							while(!Touch_Pressed);
							menu_select=0;
						}	
					}
					else {
						End_fail();Who_fuction();while(!Touch_Pressed);
						menu_select=0;
					}
				}
				else {
					End_fail();Who_fuction();while(!Touch_Pressed);
					menu_select=0;
				}
		}
		else if(start==2){
			Option();
			menu_select=0;
		}
		else if(start==3){
			Explain_fuction();
			menu_select=0;
		}
	}
}

#include "2450addr.h"
#include "my_lib.h"
#include "option.h"

//image


#define BLACK	0x0000
#define WHITE	0xFFFF
#define BLUE		0x001F
#define GREEN	0x03E0
#define RED		0x7C00

//image buffer
#define  NonPal_Fb   ((volatile unsigned short(*)[480]) FRAME_BUFFER)
#define  Fb   ((volatile unsigned long(*)[80]) FRAME_BUFFER)

//touch 
extern volatile  int ADC_x, ADC_y;
extern volatile  int Touch_Pressed;

//rtc
extern int min;
extern int sec;
extern int temp;
extern int Limit_min ;
extern int d_min;
extern int d_sec;

//image buffer
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

//raking
int Lanking_num[3];
int Sum_num;

static int LED_flag;

void Timer2_ISR(void) __attribute__((interrupt ("IRQ")));
void Timer2_ISR()
{	
	 rSRCPND1|=(0x1<<12);
	 rINTPND1|=(0x1<<12);
	 Uart_Printf("timer2");
	 int i;
	 if(LED_flag==0){
	 	rGPGDAT &=~(0xff<<4);
	 	LED_flag=1;
	 	}
	 else {
	 	rGPGDAT |= (0xff<<4);
	 	LED_flag=0;
	 	}
}

void End_fail(void){
	Rtc_Init();
	Timer_Init();
	Lcd_Printf(200,10,WHITE, TIMER_buffer,"%02d:%02d",0,0);
	Lcd_Puts_big(180,50,WHITE, TIMER_buffer,"GAME OVER");
	Lcd_Printf(200,90,WHITE, TIMER_buffer,"TIME : %d sec",Sum_num);
}


void End_Sucess(void){
	Rtc_Init();
	Timer_Init();
	int i,j;
	Lcd_Printf(200,10,WHITE, TIMER_buffer,"%02d:%02d",0,0);
	for(i=0;i<3;i++){
		if(Lanking_num[i]==0||Lanking_num[i]>Sum_num) {
			for(j=2;j>i;j--){
				Lanking_num[j]=Lanking_num[j-1];
			}
		}
		break;
	}
	
	Lcd_Puts_big(180,50,WHITE, TIMER_buffer,"SUCESS");
	Lcd_Printf(200,90,WHITE, TIMER_buffer,"TIME : %d sec",Sum_num);

	for(i=0;i<3;i++){
		Lcd_Printf(200,110+j,WHITE, TIMER_buffer,"[%d] %d",i+1,Lanking_num[i]);
		j+=30;
		i++;
	}
}

int GAME1_(void){
	
	rINTMSK1 &= ~(0x1<<11);	
	rINTMSK1 &= ~(0x1<<12);	
	pISR_TIMER2=(unsigned int)Timer2_ISR;	

	Rtc_Init();
	int i,j,x,y;
	for(i=0;i<480;i++){
		for(j=0;j<272;j++){
			NonPal_Put_Pixel(i, j, fullscreen_buffer_1[j][i]);
		}
	}
	
	Timer_Delay_1(10);
	int cnt_true = 0;
	int cnt_false = 0;
	int flag=0;
	int flag1=0,flag2=0;
	Uart_Printf("start\n");

	Touch_Pressed=0;
	while(1){
		x=0;y=0;
		Lcd_Printf(420,10,WHITE, TIMER_buffer,"C : %d/1",cnt_true);
		Lcd_Printf(420,30,WHITE, TIMER_buffer,"W : %d/5",cnt_false);
		while(!Touch_Pressed){
			if(cnt_false==5||min==Limit_min) return -1;
			if(Limit_min-d_min==1&&d_sec==30&&flag1==0){
				Timer_Delay_2(500);
				flag1=1;	
			} 
			if(Limit_min-d_min==1&&d_sec==50&&flag2==0){
				Buzzer_timer();
				flag2=1;	
			} 
			if(cnt_true==1) return 1;
		}
		if((80<ADC_x && ADC_x<160)&&(80<ADC_y &&ADC_y<160)&&(flag!=1)) {
			flag=1;
			for ( i = ADC_y-12 ; i < ADC_y+12 ; i ++){
				for( j = ADC_x-20; j < ADC_x+20 ; j ++ ){
					NonPal_Put_Pixel(j, i ,Buffer_o[y][x] == 0x0000 ?fullscreen_buffer_1[i][j] : Buffer_o[y][x]);
					x++;
					}
					x = 0;
					y++;
				}
			Uart_Printf("true\n");
			++cnt_true;
			//'o' image
		}
		else {
				//'x' image
			for ( i = ADC_y-12 ; i < ADC_y+12 ; i ++){
				for( j = ADC_x-20; j < ADC_x+20 ; j ++ ){
					NonPal_Put_Pixel(j, i ,Buffer_x[y][x] == 0x0000 ?fullscreen_buffer_1[i][j] : Buffer_x[y][x]);
					x++;
					}
					x = 0;
					y++;
				}
			Uart_Printf("true\n");
				Uart_Printf("false\n");
				++cnt_false;
		}
		for(i=0;i<0xffffa;i++);
	
	}
}



int GAME2_(void){
	rGPGDAT |= (0xff<<4);
	LED_flag=0;
	rTCON &= ~(0x1<<8);
	rTCON |= (0x1<<8);
	rINTMSK1 |= (0x1<<12);	
	rINTMSK1 |= (0x1<<13);	

	int i,j,x,y;
	x=0;y=0;
	Rtc_Init();
	for(i=0;i<480;i++){
		for(j=0;j<272;j++){
			NonPal_Put_Pixel(i, j, fullscreen_buffer_2[j][i]);
		}
	}
	
	Timer_Delay_1(10);
	int cnt_true = 0;
	int cnt_false = 0;
	int check[4]={0,};
	int flag=0,flag1=0;
	Uart_Printf("start\n");
	
	while(1){
		x=0;y=0;
		Lcd_Printf(420,10,WHITE, TIMER_buffer,"C : %d/3",cnt_true);
		Lcd_Printf(420,30,WHITE, TIMER_buffer,"W : %d/5",cnt_false);
		while(!Touch_Pressed){
			if(cnt_false==5||min==Limit_min) return -1;
			if(cnt_true==3) return 1;
			if(Limit_min-d_min==1&&d_sec==30&&flag==0){
				rINTMSK1 &= ~(0x1<<12);	Timer_Delay_2(500);
				flag=1;	
			}
			if(Limit_min-d_min==1&&d_sec==50&&flag1==0){
				rINTMSK1 &= ~(0x1<<13);	Buzzer_timer();
				flag1=1;	
			} 
		}

	if((20<ADC_x&&ADC_x<60)&&(10<ADC_y&&ADC_y<30)&&(check[0]!=1)) {
		for ( i = ADC_y-12 ; i < ADC_y+12 ; i ++){
				for( j = ADC_x-20; j < ADC_x+20 ; j ++ ){
					NonPal_Put_Pixel(j, i ,Buffer_o[y][x] == 0x0000 ?fullscreen_buffer_2[i][j] : Buffer_o[y][x]);
					x++;
					}
					x = 0;
					y++;
				}
			Uart_Printf("true\n");
			++cnt_true;
			check[0]=1;
			//'o' image
		}
		else if((20<ADC_x&&ADC_x<70)&&(40<ADC_y&&ADC_y<120)&&(check[1]!=1)){
		for ( i = ADC_y-12 ; i < ADC_y+12 ; i ++){
				for( j = ADC_x-20; j < ADC_x+20 ; j ++ ){
					NonPal_Put_Pixel(j, i ,Buffer_o[y][x] == 0x0000 ?fullscreen_buffer_2[i][j] : Buffer_o[y][x]);
					x++;
					}
					x = 0;
					y++;
				}
			Uart_Printf("true\n");
			++cnt_true;
			check[1]=1;
			//'o' image
		}
		else if((55<ADC_x&&ADC_x<90)&&(100<ADC_y&&ADC_y<160)&&(check[2]!=1)){
		for ( i = ADC_y-12 ; i < ADC_y+12 ; i ++){
				for( j = ADC_x-20; j < ADC_x+20 ; j ++ ){
					NonPal_Put_Pixel(j, i ,Buffer_o[y][x] == 0x0000 ?fullscreen_buffer_2[i][j] : Buffer_o[y][x]);
					x++;
					}
					x = 0;
					y++;
				}
			Uart_Printf("true\n");
			++cnt_true;
			check[2]=1;
			
			//'o' image
		}
		else if((50<ADC_x&&ADC_x<150)&&(165<ADC_y&&ADC_y<210)&&(check[3]!=1)){
			for ( i = ADC_y-12 ; i < ADC_y+12 ; i ++){
				for( j = ADC_x-20; j < ADC_x+20 ; j ++ ){
					NonPal_Put_Pixel(j, i ,Buffer_o[y][x] == 0x0000 ?fullscreen_buffer_2[i][j] : Buffer_o[y][x]);
					x++;
					}
					x = 0;
					y++;
				}
			Uart_Printf("true\n");
			++cnt_true;
			check[3]=1;
		
			//'o' image
		}
		else {
			for ( i = ADC_y-12 ; i < ADC_y+12 ; i ++){
				for( j = ADC_x-20; j < ADC_x+20 ; j ++ ){
					NonPal_Put_Pixel(j, i ,Buffer_x[y][x] == 0x0000 ?fullscreen_buffer_2[i][j] : Buffer_x[y][x]);
					x++;
					}
					x = 0;
					y++;
				}
				Uart_Printf("false\n");
				++cnt_false;
		}
		
		for(i=0;i<0xffffa;i++);
	}
}


int GAME3_(void){
	rGPGDAT |= (0xff<<4);
	 LED_flag=0;
	rTCON &= ~(0x1<<8);
	rTCON |= (0x1<<8);

	int i,j,x,y;
	x=0;y=0;
	rINTMSK1 |= (0x1<<12);	
	rINTMSK1 |= (0x1<<13);	
	Rtc_Init();
	for(i=0;i<480;i++){
		for(j=0;j<272;j++){
			NonPal_Put_Pixel(i, j, fullscreen_buffer_3[j][i]);
		}
	}
	
	Timer_Delay_1(10);
	int cnt_true = 0;
	int cnt_false = 0;
	int check[3]={0,};
	int flag=0,flag1=0;

	Uart_Printf("start\n");
	while(1){
		x=0;y=0;
		Lcd_Printf(420,10,WHITE, TIMER_buffer,"C : %d/2",cnt_true);
		Lcd_Printf(420,30,WHITE, TIMER_buffer,"W : %d/5",cnt_false);
		while(!Touch_Pressed){
			if(cnt_false==5||min==Limit_min) return -1;
			if(cnt_true==1) return 1;
			if(Limit_min-d_min==1&&d_sec==30&&flag==0){
				rINTMSK1 &= ~(0x1<<12);	Timer_Delay_2(500);
				flag=1;
			}
			if(Limit_min-d_min==1&&d_sec==50&&flag1==0){
				rINTMSK1 &= ~(0x1<<13);	Buzzer_timer();
				flag1=1;	
			} 
		}

		if((2<ADC_x&&ADC_x<70)&&(120<ADC_y&&ADC_y<200)&&(check[0]!=1)) {
			for ( i = ADC_y-12 ; i < ADC_y+12 ; i ++){
				for( j = ADC_x-20; j < ADC_x+20 ; j ++ ){
					NonPal_Put_Pixel(j, i ,Buffer_o[y][x] == 0x0000 ?fullscreen_buffer_3[i][j] : Buffer_o[y][x]);
					x++;
					}
					x = 0;
					y++;
				}
			Uart_Printf("true\n");
			++cnt_true;
			check[0]=1;
			//'o' image
		}
		else if((40<ADC_x&&ADC_x<110)&&(200<ADC_y&&ADC_y<270)&&(check[1]!=1)){
			for ( i = ADC_y-12 ; i < ADC_y+12 ; i ++){
				for( j = ADC_x-20; j < ADC_x+20 ; j ++ ){
					NonPal_Put_Pixel(j, i ,Buffer_o[y][x] == 0x0000 ?fullscreen_buffer_3[i][j] : Buffer_o[y][x]);
					x++;
					}
					x = 0;
					y++;
				}
			Uart_Printf("true\n");
			++cnt_true;
			check[1]=1;		
		}
		else {
				//'x' image
			for ( i = ADC_y-12 ; i < ADC_y+12 ; i ++){
				for( j = ADC_x-20; j < ADC_x+20 ; j ++ ){
					NonPal_Put_Pixel(j, i ,Buffer_x[y][x] == 0x0000 ?fullscreen_buffer_3[i][j] : Buffer_x[y][x]);
					x++;
					}
					x = 0;
					y++;
				}
				Uart_Printf("false\n");
				++cnt_false;
		}
		for(i=0;i<0xffffa;i++);
	}
}
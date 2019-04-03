#include "2450addr.h"
#include "my_lib.h"
#include "option.h"

extern unsigned int char_buffer[44][44];
extern unsigned int Buffer_lim[272][480];

void Who_fuction(void){
	int x = 0;
	int y = 0;
	int k=0;
	int z=0;
	int i,j;

while(z<271){

	while(k<440){
	for ( i = 0+z ; i < 44+z ; i ++)
	{
		for( j = 0+k; j < 44+k ; j ++ )
		{
			NonPal_Put_Pixel(j, i ,char_buffer[y][x]==0x0000 ? Buffer_lim[i][j] : char_buffer[y][x]);
			//NonPal_Put_Pixel(j+1, i ,char_buffer[y][x]);

			x++;
		}
		x = 0;
		y++;
	}
	
	for ( i = 0+z ; i < 44+z ; i ++)
	{
		for( j = 0+k; j < 15+k ; j ++ )
		{
			NonPal_Put_Pixel(j, i ,Buffer_lim[i][j]);
			
		}
		
	}
	k+=10;
	x=0;y=0;
	}
	for ( i = 0+z ; i < 44+z ; i ++)
	{
		for( j = 0+k; j < 40+k ; j ++ )
		{
			NonPal_Put_Pixel(j, i ,Buffer_lim[i][j]);
			
		}
		
	}
	z+=40;
	k=0;
  }

}

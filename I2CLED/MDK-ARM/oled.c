/**
	*@file oled.c
	*@author xuwithbean(Jiaxu Xiao)
	*@date 2023.3.24
	*@berif this is a drive program to drive SSD1036
  */
#include "oled.h"
#include "i2c.h"
#include "oledfont.h"          
/**
  *@brief the first order to drive the screen
	*@param CMD_Data refer to the order array used to drive the screen
					|		order	|													founction												|																introduction																|
					|0x00-0x0F|set lower column start address for page addressing mode 	|start line register reset to 0000b																					|
					|0x10-0x1F|set higher column start address for page addressing mode	|start line register reset to 0000b																					|
					|0x20			|set register address mode																|when A[1:0]=0x00b horizontal address mode																	|
					|					|																													|						 0x01b vertical address mode																		|
					|					|																													|						 0x10b page address mode																				|
					|0x21			|set column address																				|A[6:0] -> start ;B[6:0] ->  end																						|
					|0x22			|set page address																					|A[2:0]start range:0-7d and B[2:0]end range:0-7d(7d is reset)								|
					|0x40-0x7F|set display start line																		|0x40 refer to 0,if it's x,RAM line x reflect to COM0												|
					|0x81			|set contrast control for BANK0														|set screen's contrast control(0x00-0xFF)																		|
					|0xA0|0xA1|set segment re-map																				|charge the reference relationship between column adress and segment drives |
					|0xA4|0xA5|entire display on																				|if A5 has been used,use A4 to recover																			|
					|0xA6|0xA7|set normal or opposite display mode											|when normal mode used,data 1 in RAM means the small LED light							|
					|0xA8			|set the multiplexing rate																|default 63 can change to any num in [16,63],COMx will be normal mode				|
					|0xAE|0xAF|set the screen to be on or off														|AF let sleep mode to be normal mode,AE let normal mode to be sleep mode		|
					|0xC0=0xC8|set com output scan direction														|if screen is normal mood,use this command will let picture turn 90 degrees	|
					|0xD3			|set display offset																				|it will move the picture																										|
					|0xD5			|set display clock divide ration/oscillator frequency			|set clock division value A[3:0] in [1,16]																	|
					|					|																													|		 oscillator frequency value A[7:4],defult is 1000b											|
					|0xD9			|set pre-charge period																		|period is time of 1 DCLK,reset value is 2 DCLK															|
					|0xDA			|set COM Pins hardware configuration											|DA A[4]=0 is column pin's configuration,scanf direction is from COM0-63		|
					|					|																													|DA A[5]=0,disable left/right reflection;A[5]=1,enable left/right reflection|
					|					|																													|DA A[4]=1 is another column pin's configuration														|
					|0xDB			|set VCOMH deselect level																	|change VCOMH's output																											|
					|0xE3			|empty command																						|without funtction																													|
					|0x26|0x27|horizontal scroll setup																	|the charater will roll in horizontal direction															|
					|0x29|0x2A|continuos vertical and horizontal scroll setup						|B[2:0]C[2:0]D[2:0]set vertical roll and E[5]set vertical offset(0 is horiz)|
					|0x2E			|disable scroll																						|RAM must be reset when use 2E																							|
					|0x2F			|enable scroll																						|musr be used after 26|27|29|2A																							|
  */
uint8_t CMD_Data[]={
0xAE, 0x00, 0x10, 0x40, 0xB0, 0x81, 0xFF, 0xA1, 0xA6, 0xA8, 0x3F,				
0xC8, 0xD3, 0x00, 0xD5, 0x80, 0xD8, 0x05, 0xD9, 0xF1, 0xDA, 0x12,	
0xD8, 0x30, 0x8D, 0x14, 0xAF};
/**
	*@brief used to firstly let screen on
	*@param none
	*@retval none
  */
void WriteCmd(void)
{
	uint8_t i = 0;
	for(i=0; i<27; i++)
	{
		HAL_I2C_Mem_Write(&OLED_I2C ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,CMD_Data+i,1,0x100);
	}
}
/**
	*@brief set command to the screen
	*@param all data in CAM_Data can be param used in OLED_WR_CMD
	*@retval none
  */
void OLED_WR_CMD(uint8_t cmd)
{
	HAL_I2C_Mem_Write(&OLED_I2C ,0x78,0x00,I2C_MEMADD_SIZE_8BIT,&cmd,1,0x100);
}
/**
	*@brief write date into the screen
	*@param a Byte can be a param
	*@retval none
	*/
void OLED_WR_DATA(uint8_t data)
{
	HAL_I2C_Mem_Write(&OLED_I2C ,0x78,0x40,I2C_MEMADD_SIZE_8BIT,&data,1,0x100);
}
/**
	*@brief initialize the screen
	*@param none
	*@retval none
	*/
void OLED_Init(void)
{ 	
	HAL_Delay(200);
 
	WriteCmd();
}
/**
	*@brief clear all the light led on the screen
	*@param none
	*@retval none
	*/
void OLED_Clear(void)
{
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_CMD(0xb0+i);
		OLED_WR_CMD (0x00); 
		OLED_WR_CMD (0x10); 
		for(n=0;n<128;n++)
			OLED_WR_DATA(0);
	} 
}
/**
	*@brief let the screen be able to display what it accepted
	*@param none
	*@retval none
	*/   
void OLED_Display_On(void)
{
	OLED_WR_CMD(0X8D);  //SET DCDC
	OLED_WR_CMD(0X14);  //DCDC ON
	OLED_WR_CMD(0XAF);  //DISPLAY ON
}
/**
	*@brief let the screen be not able to display what it accepted
	*@param none
	*@retval none
	*/     
void OLED_Display_Off(void)
{
	OLED_WR_CMD(0X8D);  //SET DCDC
	OLED_WR_CMD(0X10);  //DCDC OFF
	OLED_WR_CMD(0XAE);  //DISPLAY OFF
}
/**
	*@brief point to the RAM refer to the special little LED
	*@param x is the horizon position of the point,y is the vertical position of the point
	*@retval none
	*/ 
void OLED_Set_Pos(uint8_t x, uint8_t y) 
{ 	
	OLED_WR_CMD(0xb0+y);
	OLED_WR_CMD(((x&0xf0)>>4)|0x10);
	OLED_WR_CMD(x&0x0f);
} 
 /**
	*@brief let the screen to be light
	*@param none
	*@retval none
	*/ 
void OLED_On(void)  
{  
	uint8_t i,n;		    
	for(i=0;i<8;i++)  
	{  
		OLED_WR_CMD(0xb0+i);    
		OLED_WR_CMD(0x00);      
		OLED_WR_CMD(0x10);         
		for(n=0;n<128;n++)
			OLED_WR_DATA(1); 
	} 
}
/**
	*@brief oled_pow is like pow(m,n) in math.h
	*@param m is the root and n is the power
	*@retval m**n
	*/
unsigned int oled_pow(uint8_t m,uint8_t n)
{
	unsigned int result=1;	 
	while(n--)result*=m;    
	return result;
}
/**
	*@brief	this founction is used to display two numbers 
	*@param x,y is the start position to display the numbers,len is the number of nums,num is what to display(range is [0-4294967295]),size2 is the size of the nums
	*@retval none
	*/ 		  
void OLED_ShowNum(uint8_t x,uint8_t y,unsigned int num,uint8_t len,uint8_t size2)
{         	
	uint8_t t,temp;
	uint8_t enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/oled_pow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				OLED_ShowChar(x+(size2/2)*t,y,' ',size2);
				continue;
			}else enshow=1; 
		}
	 	OLED_ShowChar(x+(size2/2)*t,y,temp+'0',size2); 
	}
} 
/**
	*@brief print a charater at the special position,include some charaters
	*@param x,y is the start position to display,chr is the charater to display,size is the size of the charater
	*@retval none
	*/
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t Char_Size)
{      	
	unsigned char c=0,i=0;	
		c=chr-' ';//get the value after offset		
		if(x>128-1){x=0;y=y+2;}
		if(Char_Size ==16)
			{
			OLED_Set_Pos(x,y);	
			for(i=0;i<8;i++)
			OLED_WR_DATA(F8X16[c*16+i]);
			OLED_Set_Pos(x,y+1);
			for(i=0;i<8;i++)
			OLED_WR_DATA(F8X16[c*16+i+8]);
			}
			else {	
				OLED_Set_Pos(x,y);
				for(i=0;i<6;i++)
				OLED_WR_DATA(F6x8[c][i]);
				
			}
}
/**
	*@brief used to display a array of charaters
	*@param x,y is the start position to display,chr is the array of charaters,Char_Size is the size of the charaters
	*@retval none
	*/
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t Char_Size)
{
	unsigned char j=0;
	while (chr[j]!='\0')
	{		OLED_ShowChar(x,y,chr[j],Char_Size);
			x+=8;
		if(x>120){x=0;y+=2;}
			j++;
	}
}
/**
	*@brief used to display the Chinese charater(must used pctolcd to generate the hex value of the Chinese Charater)
	*@param x,y is the start position of the Chinese charater,no is the number of the charater in the hzk array
	*@retval none
	*/
void OLED_ShowCHinese(uint8_t x,uint8_t y,uint8_t no)
{      			    
	uint8_t t,adder=0;
	OLED_Set_Pos(x,y);	
    for(t=0;t<16;t++)
		{
				OLED_WR_DATA(Hzk[2*no][t]);
				adder+=1;
     }	
		OLED_Set_Pos(x,y+1);	
    for(t=0;t<16;t++)
			{	
				OLED_WR_DATA(Hzk[2*no+1][t]);
				adder+=1;
      }					
}
/**
	*@brief used to display the pictures,the hex velue of the picture must use image2LCD to generate
	*@param x0,y0 is the start position to ptint the picture,x is the lenth of the picture,y*8 is the highth of the picture,bmp[] is the hex value of the picture
	*@retval none
	*/
void OLED_DrawBMP(unsigned char x0,unsigned char y0,unsigned char x1,unsigned char y1,unsigned char BMP[])
{
	unsigned int j=0;
	unsigned char x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
		{
			OLED_WR_DATA(BMP[j++]);
		}
	}
}


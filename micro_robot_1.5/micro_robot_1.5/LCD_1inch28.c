/*****************************************************************************
* | File      	:   LCD_1inch28.c
* | Author      :   Waveshare team
* | Function    :   Hardware underlying interface
* | Info        :
*                Used to shield the underlying layers of each master
*                and enhance portability
*----------------
* |	This version:   V1.0
* | Date        :   2020-12-06
* | Info        :   Basic version
*
******************************************************************************/
#include "LCD_1inch28.h"
#include "DEV_Config.h"

#include <stdlib.h>		//itoa()
#include <stdio.h>
#include <gpio.h>

LCD_1IN28_ATTRIBUTES LCD_1IN28;


/******************************************************************************
function :	Hardware reset
parameter:
******************************************************************************/
static void LCD_1IN28_Reset(void)
{
    LCD_1IN28_RST_1;
    DEV_Delay_ms(100);
    LCD_1IN28_RST_0;
    DEV_Delay_ms(100);
    LCD_1IN28_RST_1;
    DEV_Delay_ms(100);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void LCD_1IN28_SendCommand(UBYTE Reg)
{
    LCD_1IN28_DC_0;
	//printf("LCD_1IN28_DC_0\r\n");	
    LCD_1IN28_CS_0;
	//printf("LCD_1IN28_CS_0\r\n");	
    DEV_SPI_WRITE(Reg);
	//printf("DEV_SPI_WRITE\r\n");	
    // LCD_1IN28_CS_1;
	//printf("LCD_1IN28_SendCommand\r\n");	
	//while(1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_1IN28_SendData_8Bit(UBYTE Data)
{
    LCD_1IN28_DC_1;
	//printf("LCD_1IN28_DC_1\r\n");	
    LCD_1IN28_CS_0;
	//printf("LCD_1IN28_CS_0\r\n");	
    DEV_SPI_WRITE(Data);
	//printf("DEV_SPI_WRITE\r\n");	
    LCD_1IN28_CS_1;
	//printf("LCD_1IN28_CS_1\r\n");	
	//printf("LCD_1IN28_SendData_8Bit\r\n");	
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void LCD_1IN28_SendData_16Bit(UWORD Data)
{
    LCD_1IN28_DC_1;
    LCD_1IN28_CS_0;
    DEV_SPI_WRITE(Data >> 8);
    DEV_SPI_WRITE(Data);
    LCD_1IN28_CS_1;
}

/******************************************************************************
function :	Initialize the lcd register
parameter:
******************************************************************************/
static void LCD_1IN28_InitReg(void)
{
  LCD_1IN28_SendCommand(0xEF);
	LCD_1IN28_SendCommand(0xEB);
	LCD_1IN28_SendData_8Bit(0x14); 
	
  LCD_1IN28_SendCommand(0xFE);			 
	LCD_1IN28_SendCommand(0xEF); 

	LCD_1IN28_SendCommand(0xEB);	
	LCD_1IN28_SendData_8Bit(0x14); 

	LCD_1IN28_SendCommand(0x84);			
	LCD_1IN28_SendData_8Bit(0x40); 

	LCD_1IN28_SendCommand(0x85);			
	LCD_1IN28_SendData_8Bit(0xFF); 

	LCD_1IN28_SendCommand(0x86);			
	LCD_1IN28_SendData_8Bit(0xFF); 

	LCD_1IN28_SendCommand(0x87);			
	LCD_1IN28_SendData_8Bit(0xFF);

	LCD_1IN28_SendCommand(0x88);			
	LCD_1IN28_SendData_8Bit(0x0A);

	LCD_1IN28_SendCommand(0x89);			
	LCD_1IN28_SendData_8Bit(0x21); 

	LCD_1IN28_SendCommand(0x8A);			
	LCD_1IN28_SendData_8Bit(0x00); 

	LCD_1IN28_SendCommand(0x8B);			
	LCD_1IN28_SendData_8Bit(0x80); 

	LCD_1IN28_SendCommand(0x8C);			
	LCD_1IN28_SendData_8Bit(0x01); 

	LCD_1IN28_SendCommand(0x8D);			
	LCD_1IN28_SendData_8Bit(0x01); 

	LCD_1IN28_SendCommand(0x8E);			
	LCD_1IN28_SendData_8Bit(0xFF); 

	LCD_1IN28_SendCommand(0x8F);			
	LCD_1IN28_SendData_8Bit(0xFF); 


	LCD_1IN28_SendCommand(0xB6);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x20);

	LCD_1IN28_SendCommand(0x36);
	LCD_1IN28_SendData_8Bit(0x08);//Set as vertical screen

	LCD_1IN28_SendCommand(0x3A);			
	LCD_1IN28_SendData_8Bit(0x05); 


	LCD_1IN28_SendCommand(0x90);			
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x08); 

	LCD_1IN28_SendCommand(0xBD);			
	LCD_1IN28_SendData_8Bit(0x06);
	
	LCD_1IN28_SendCommand(0xBC);			
	LCD_1IN28_SendData_8Bit(0x00);	

	LCD_1IN28_SendCommand(0xFF);			
	LCD_1IN28_SendData_8Bit(0x60);
	LCD_1IN28_SendData_8Bit(0x01);
	LCD_1IN28_SendData_8Bit(0x04);

	LCD_1IN28_SendCommand(0xC3);			
	LCD_1IN28_SendData_8Bit(0x13);
	LCD_1IN28_SendCommand(0xC4);			
	LCD_1IN28_SendData_8Bit(0x13);

	LCD_1IN28_SendCommand(0xC9);			
	LCD_1IN28_SendData_8Bit(0x22);

	LCD_1IN28_SendCommand(0xBE);			
	LCD_1IN28_SendData_8Bit(0x11); 

	LCD_1IN28_SendCommand(0xE1);			
	LCD_1IN28_SendData_8Bit(0x10);
	LCD_1IN28_SendData_8Bit(0x0E);

	LCD_1IN28_SendCommand(0xDF);			
	LCD_1IN28_SendData_8Bit(0x21);
	LCD_1IN28_SendData_8Bit(0x0c);
	LCD_1IN28_SendData_8Bit(0x02);

	LCD_1IN28_SendCommand(0xF0);   
	LCD_1IN28_SendData_8Bit(0x45);
	LCD_1IN28_SendData_8Bit(0x09);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x26);
 	LCD_1IN28_SendData_8Bit(0x2A);

 	LCD_1IN28_SendCommand(0xF1);    
 	LCD_1IN28_SendData_8Bit(0x43);
 	LCD_1IN28_SendData_8Bit(0x70);
 	LCD_1IN28_SendData_8Bit(0x72);
 	LCD_1IN28_SendData_8Bit(0x36);
 	LCD_1IN28_SendData_8Bit(0x37);  
 	LCD_1IN28_SendData_8Bit(0x6F);


 	LCD_1IN28_SendCommand(0xF2);   
 	LCD_1IN28_SendData_8Bit(0x45);
 	LCD_1IN28_SendData_8Bit(0x09);
 	LCD_1IN28_SendData_8Bit(0x08);
 	LCD_1IN28_SendData_8Bit(0x08);
 	LCD_1IN28_SendData_8Bit(0x26);
 	LCD_1IN28_SendData_8Bit(0x2A);

 	LCD_1IN28_SendCommand(0xF3);   
 	LCD_1IN28_SendData_8Bit(0x43);
 	LCD_1IN28_SendData_8Bit(0x70);
 	LCD_1IN28_SendData_8Bit(0x72);
 	LCD_1IN28_SendData_8Bit(0x36);
 	LCD_1IN28_SendData_8Bit(0x37); 
 	LCD_1IN28_SendData_8Bit(0x6F);

	LCD_1IN28_SendCommand(0xED);	
	LCD_1IN28_SendData_8Bit(0x1B); 
	LCD_1IN28_SendData_8Bit(0x0B); 

	LCD_1IN28_SendCommand(0xAE);			
	LCD_1IN28_SendData_8Bit(0x77);
	
	LCD_1IN28_SendCommand(0xCD);			
	LCD_1IN28_SendData_8Bit(0x63);		


	LCD_1IN28_SendCommand(0x70);			
	LCD_1IN28_SendData_8Bit(0x07);
	LCD_1IN28_SendData_8Bit(0x07);
	LCD_1IN28_SendData_8Bit(0x04);
	LCD_1IN28_SendData_8Bit(0x0E); 
	LCD_1IN28_SendData_8Bit(0x0F); 
	LCD_1IN28_SendData_8Bit(0x09);
	LCD_1IN28_SendData_8Bit(0x07);
	LCD_1IN28_SendData_8Bit(0x08);
	LCD_1IN28_SendData_8Bit(0x03);

	LCD_1IN28_SendCommand(0xE8);			
	LCD_1IN28_SendData_8Bit(0x34);

	LCD_1IN28_SendCommand(0x62);			
	LCD_1IN28_SendData_8Bit(0x18);
	LCD_1IN28_SendData_8Bit(0x0D);
	LCD_1IN28_SendData_8Bit(0x71);
	LCD_1IN28_SendData_8Bit(0xED);
	LCD_1IN28_SendData_8Bit(0x70); 
	LCD_1IN28_SendData_8Bit(0x70);
	LCD_1IN28_SendData_8Bit(0x18);
	LCD_1IN28_SendData_8Bit(0x0F);
	LCD_1IN28_SendData_8Bit(0x71);
	LCD_1IN28_SendData_8Bit(0xEF);
	LCD_1IN28_SendData_8Bit(0x70); 
	LCD_1IN28_SendData_8Bit(0x70);

	LCD_1IN28_SendCommand(0x63);			
	LCD_1IN28_SendData_8Bit(0x18);
	LCD_1IN28_SendData_8Bit(0x11);
	LCD_1IN28_SendData_8Bit(0x71);
	LCD_1IN28_SendData_8Bit(0xF1);
	LCD_1IN28_SendData_8Bit(0x70); 
	LCD_1IN28_SendData_8Bit(0x70);
	LCD_1IN28_SendData_8Bit(0x18);
	LCD_1IN28_SendData_8Bit(0x13);
	LCD_1IN28_SendData_8Bit(0x71);
	LCD_1IN28_SendData_8Bit(0xF3);
	LCD_1IN28_SendData_8Bit(0x70); 
	LCD_1IN28_SendData_8Bit(0x70);

	LCD_1IN28_SendCommand(0x64);			
	LCD_1IN28_SendData_8Bit(0x28);
	LCD_1IN28_SendData_8Bit(0x29);
	LCD_1IN28_SendData_8Bit(0xF1);
	LCD_1IN28_SendData_8Bit(0x01);
	LCD_1IN28_SendData_8Bit(0xF1);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x07);

	LCD_1IN28_SendCommand(0x66);			
	LCD_1IN28_SendData_8Bit(0x3C);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0xCD);
	LCD_1IN28_SendData_8Bit(0x67);
	LCD_1IN28_SendData_8Bit(0x45);
	LCD_1IN28_SendData_8Bit(0x45);
	LCD_1IN28_SendData_8Bit(0x10);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x00);

	LCD_1IN28_SendCommand(0x67);			
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x3C);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x00);
	LCD_1IN28_SendData_8Bit(0x01);
	LCD_1IN28_SendData_8Bit(0x54);
	LCD_1IN28_SendData_8Bit(0x10);
	LCD_1IN28_SendData_8Bit(0x32);
	LCD_1IN28_SendData_8Bit(0x98);

	LCD_1IN28_SendCommand(0x74);			
	LCD_1IN28_SendData_8Bit(0x10);	
	LCD_1IN28_SendData_8Bit(0x85);	
	LCD_1IN28_SendData_8Bit(0x80);
	LCD_1IN28_SendData_8Bit(0x00); 
	LCD_1IN28_SendData_8Bit(0x00); 
	LCD_1IN28_SendData_8Bit(0x4E);
	LCD_1IN28_SendData_8Bit(0x00);					
	
  LCD_1IN28_SendCommand(0x98);			
	LCD_1IN28_SendData_8Bit(0x3e);
	LCD_1IN28_SendData_8Bit(0x07);

	LCD_1IN28_SendCommand(0x35);	
	LCD_1IN28_SendCommand(0x21);

	LCD_1IN28_SendCommand(0x11);
	DEV_Delay_ms(120);
	LCD_1IN28_SendCommand(0x29);
	DEV_Delay_ms(20);
}

/********************************************************************************
function:	Set the resolution and scanning method of the screen
parameter:
		Scan_dir:   Scan direction
********************************************************************************/
static void LCD_1IN28_SetAttributes(UBYTE Scan_dir)
{
    //Get the screen scan direction
    LCD_1IN28.SCAN_DIR = Scan_dir;
	//printf("1\r\n");
    UBYTE MemoryAccessReg = 0x08;
	//printf("2\r\n");
    //Get GRAM and LCD width and height
    if(Scan_dir == HORIZONTAL) {
        LCD_1IN28.HEIGHT	= LCD_1IN28_HEIGHT;
        LCD_1IN28.WIDTH   = LCD_1IN28_WIDTH;
        MemoryAccessReg = 0XC8;
    } else {
        LCD_1IN28.HEIGHT	= LCD_1IN28_WIDTH;
        LCD_1IN28.WIDTH   = LCD_1IN28_HEIGHT;
        MemoryAccessReg = 0X68;
    }
	//printf("3\r\n");
    // Set the read / write scan direction of the frame memory
    LCD_1IN28_SendCommand(0x36); //MX, MY, RGB mode
	//printf("4\r\n");
    LCD_1IN28_SendData_8Bit(MemoryAccessReg);	//0x08 set RGB
	//printf("5\r\n");
	//while(1);
	printf("LCD_1IN28_SendData_8Bit");
}

/********************************************************************************
function :	Initialize the lcd
parameter:
********************************************************************************/
void LCD_1IN28_Init(UBYTE Scan_dir)
{

    //Hardware reset
    LCD_1IN28_Reset();
	//printf("LCD_1IN28_ Reset...\r\n");

    //Set the resolution and scanning method of the screen
    LCD_1IN28_SetAttributes(Scan_dir);
	//printf("LCD_1IN28_SetAttributes...\r\n");
    
    //Set the initialization register
    LCD_1IN28_InitReg();
	//printf("LCD_1IN28_InitReg...\r\n");
	
}

/********************************************************************************
function:	Sets the start position and size of the display area
parameter:
		Xstart 	:   X direction Start coordinates
		Ystart  :   Y direction Start coordinates
		Xend    :   X direction end coordinates
		Yend    :   Y direction end coordinates
********************************************************************************/
void LCD_1IN28_SetWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend)
{
    //set the X coordinates
    LCD_1IN28_SendCommand(0x2A);
    LCD_1IN28_SendData_8Bit(0x00);
    LCD_1IN28_SendData_8Bit(Xstart);
	  LCD_1IN28_SendData_8Bit(0x00);
    LCD_1IN28_SendData_8Bit(Xend);

    //set the Y coordinates
    LCD_1IN28_SendCommand(0x2B);
    LCD_1IN28_SendData_8Bit(0x00);
	  LCD_1IN28_SendData_8Bit(Ystart);
	  LCD_1IN28_SendData_8Bit(0x00);
    LCD_1IN28_SendData_8Bit(Yend);

    LCD_1IN28_SendCommand(0X2C);
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void LCD_1IN28_Clear(UWORD Color)
{
    UWORD i,j;
    LCD_1IN28_SetWindows(0, 0, LCD_1IN28_WIDTH-1, LCD_1IN28_HEIGHT-1);

	DEV_Digital_Write(DEV_DC_PIN, 1);
	for(i = 0; i < LCD_1IN28_WIDTH; i++){
		for(j = 0; j < LCD_1IN28_HEIGHT; j++){
			DEV_SPI_WRITE(Color>>8);
			DEV_SPI_WRITE(Color);
		}
	 }
}

/******************************************************************************
function :	Sends the image buffer in RAM to displays
parameter:
******************************************************************************/
void LCD_1IN28_Display(UWORD *Image)
{
    UWORD i,j;
    LCD_1IN28_SetWindows(0, 0, LCD_1IN28_WIDTH-1, LCD_1IN28_HEIGHT-1);
    DEV_Digital_Write(DEV_DC_PIN, 1);
    for(i = 0; i < LCD_1IN28_WIDTH; i++){
      for(j = 0; j < LCD_1IN28_HEIGHT; j++){
        DEV_SPI_WRITE((*Image+i*LCD_1IN28_WIDTH+j)>>8);
        DEV_SPI_WRITE(*(Image+i*LCD_1IN28_WIDTH+j));
      }
    }
}

void LCD_1IN28_DisplayWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD *Image)
{
    // display
    UDOUBLE Addr = 0;

    UWORD i,j;
    LCD_1IN28_SetWindows(Xstart, Ystart, Xend-1 , Yend-1);
    LCD_1IN28_DC_1;
    for (i = Ystart; i < Yend - 1; i++) {
        Addr = Xstart + i * LCD_1IN28_WIDTH ;
        for(j=Xstart;j<Xend-1;j++){
          DEV_SPI_WRITE(*(Image+Addr+j)>>8);
          DEV_SPI_WRITE(*(Image+Addr+j));
        }
    }
}

/******************************************************************************
function: Draw a point
parameter	:
	    X	: 	Set the X coordinate
	    Y	:	Set the Y coordinate
	  Color :	Set the color
******************************************************************************/
void LCD_1IN28_DrawPaint(UWORD x, UWORD y, UWORD Color)
{
	LCD_1IN28_SetWindows(x,y,x,y);
	LCD_1IN28_SendData_16Bit(Color); 	    
}

/*******************************************************************************
function:
	Setting backlight
parameter	:
	  value : Range 0~1000   Duty cycle is value/1000	
*******************************************************************************/

// void LCD_1IN28_SetBackLight(UWORD Value)
// {
// 	DEV_Set_PWM(Value);
// }


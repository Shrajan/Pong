////////////////////////////////////////////////////////////////////////////
/*
 *  Game Engine for PONG.
 *  ----------------------------------------
 *  File Name     : Game_Engine.c
 *  Target Device : ARM Cortex-A9 processor
 *
 *  Created on    : April 23, 2019
 *  Code Author(s): Nuo, Sanjith Chandran & Shrajan Bhandary
 *  Location 	  : University of Leeds
 *  Module 		  : ELEC5620M Embedded Microprocessor System Design
 *
 *  Description of the file:
 *  	The driver is used to control different aspects of the game such as
 *      read the player's input, manipulate the position of the paddle, 
 *		movement of the ball, players' scores and game-play of PONG.
 */
////////////////////////////////////////////////////////////////////////////

#include "Game_Engine.h"													// Invoking the main header file.
#include "../Graphics_Engine/Graphics_Engine.h"								// Importing the Graphics Engine driver library.
#include "../DE1Soc_LT24/DE1SoC_LT24.h"	 									// Importing the Leeds SoC LT24 driver controller.
#include "../HPS_Watchdog/HPS_Watchdog.h"									// Importing the Watch dog timer driver controller.
#include "../HPS_usleep/HPS_usleep.h"										// Importing the sleep function.
#include "math.h"															// Importing the mathematical library of C.
#include <time.h>															// Importing the time library so that its values can be used as seed value for generating random numbers.

/* Global variables to hold the current position of paddle 1. */
unsigned current_paddle_1_x_1;
unsigned current_paddle_1_y_1;
unsigned current_paddle_1_x_2;
unsigned current_paddle_1_y_2;

/* Global variables to hold the previous position of paddle 1. */
unsigned previous_paddle_1_x_1;
unsigned previous_paddle_1_y_1;
unsigned previous_paddle_1_x_2;
unsigned previous_paddle_1_y_2;

/* Global variables to hold the current position of paddle 2. */
unsigned current_paddle_2_x_1;
unsigned current_paddle_2_y_1;
unsigned current_paddle_2_x_2;
unsigned current_paddle_2_y_2;

/* Global variables to hold the previous position of paddle 2. */
unsigned previous_paddle_2_x_1;
unsigned previous_paddle_2_y_1;
unsigned previous_paddle_2_x_2;
unsigned previous_paddle_2_y_2;

/* Global variables to move the paddles. */
unsigned move_paddle_1_x = PADDLE_1_START_X_1;
unsigned move_paddle_2_x = PADDLE_2_START_X_1;

/* Move ball variables. */
#define PI 3.14

signed int quadrant = 1;
signed int inherent_delay = 5000;

unsigned int x1 = 120 ;
unsigned int y1 = 160;

unsigned int x2 = 120;
unsigned int y2 = 160;

unsigned int angle = 60;

unsigned short colour;
unsigned short fillColour;

unsigned int GPIO_value;

/* Base address of the seven segment displays. */
volatile char *HEX_1  = (char *) 0xFF200020;
volatile char *HEX_2  = (char *) 0xFF200021;
volatile char *HEX_3  = (char *) 0xFF200022;
volatile char *HEX_4  = (char *) 0xFF200023;
volatile char *HEX_5  = (char *) 0xFF200030;
volatile char *HEX_6  = (char *) 0xFF200031;

/* Flags for the points. */
bool ball_touch_paddle = true;

/* Variables to store players' scores. */
int player_1_score = 0;
int player_2_score = 0;

/* Function to initialize PADDLE 1 with two coordinates: top-left (x1,y1) and bottom_right (x2,y2) with a boundary colour. */
signed int Paddle_1_Initialize ( void )
{
	/* Declaring the local variables required for drawing a line */
	signed int Game_status = 0;												// Status variable to check for errors.
	
	current_paddle_1_x_1 = PADDLE_1_START_X_1;
	current_paddle_1_y_1 = PADDLE_1_START_Y_1;
	current_paddle_1_x_2 = PADDLE_1_START_X_2;
	current_paddle_1_y_2 = PADDLE_1_START_Y_2;
	
	Game_status = Graphics_drawBox ( current_paddle_1_x_1, current_paddle_1_y_1, current_paddle_1_x_2, current_paddle_1_y_2, LT24_WHITE, false, LT24_WHITE ); // Draw the paddle.
	if ( Game_status != GAME_SUCCESS) return Game_status;					// If there is an error, report it.
	
	/* The current value of the paddle becomes previous value in the next iteration. */
	previous_paddle_1_x_1 = current_paddle_1_x_1;
	previous_paddle_1_y_1 = current_paddle_1_y_1;
	previous_paddle_1_x_2 = current_paddle_1_x_2;
	previous_paddle_1_y_2 = current_paddle_1_y_2;
	
	return GAME_SUCCESS;													// Return the completion of the function.
}

/* Function to initialize PADDLE 2 with two coordinates: top-left (x1,y1) and bottom_right (x2,y2) with a boundary colour. */
signed int Paddle_2_Initialize ( void )
{
	/* Declaring the local variables required for drawing a line */
	signed int Game_status = 0;												// Status variable to check for errors.
	
	current_paddle_2_x_1 = PADDLE_2_START_X_1;
	current_paddle_2_y_1 = PADDLE_2_START_Y_1;
	current_paddle_2_x_2 = PADDLE_2_START_X_2;
	current_paddle_2_y_2 = PADDLE_2_START_Y_2;
	
	Game_status = Graphics_drawBox ( current_paddle_2_x_1, current_paddle_2_y_1, current_paddle_2_x_2, current_paddle_2_y_2, LT24_WHITE, false, LT24_WHITE ); // Draw the paddle.
	if ( Game_status != GAME_SUCCESS) return Game_status;					// If there is an error, report it.
	
	/* The current value of the paddle becomes previous value in the next iteration. */
	previous_paddle_2_x_1 = current_paddle_2_x_1;
	previous_paddle_2_y_1 = current_paddle_2_y_1;
	previous_paddle_2_x_2 = current_paddle_2_x_2;
	previous_paddle_2_y_2 = current_paddle_2_y_2;
	
	return GAME_SUCCESS;													// Return the completion of the function.
}

/* Function to draw PADDLE 1 with two coordinates: top-left (x,y) and fixed paddle dimensions with a boundary colour. */
signed int Paddle_1_Draw ( unsigned int PADDLE_1_X, unsigned int PADDLE_1_Y )
{
	/* Declaring the local variables required for drawing a line */
	signed int Game_status = 0;												// Status variable to check for errors.
	
	current_paddle_1_x_1 = PADDLE_1_X;
	current_paddle_1_y_1 = PADDLE_1_Y;
	current_paddle_1_x_2 = PADDLE_1_X + PADDLE_LENGTH;
	current_paddle_1_y_2 = PADDLE_1_Y + PADDLE_WIDTH ;
	
	Game_status = Graphics_drawBox ( previous_paddle_1_x_1, previous_paddle_1_y_1, previous_paddle_1_x_2, previous_paddle_1_y_2,LT24_BLACK, false, LT24_BLACK); // Erase the paddle.
	if ( Game_status != GAME_SUCCESS) return Game_status;					// If there is an error, report it.
	
	Game_status = Graphics_drawBox ( current_paddle_1_x_1, current_paddle_1_y_1, current_paddle_1_x_2, current_paddle_1_y_2, LT24_WHITE, false, LT24_WHITE ); // Draw the paddle.
	if ( Game_status != GAME_SUCCESS) return Game_status;					// If there is an error, report it.
	
	/* The current value of the paddle becomes previous value in the next iteration. */
	previous_paddle_1_x_1 = current_paddle_1_x_1;
	previous_paddle_1_y_1 = current_paddle_1_y_1;
	previous_paddle_1_x_2 = current_paddle_1_x_2;
	previous_paddle_1_y_2 = current_paddle_1_y_2;
	
	return GAME_SUCCESS;													// Return the completion of the function.
}

/* Function to draw PADDLE 2 with two coordinates: top-left (x,y) and fixed paddle dimensions with a boundary colour. */
signed int Paddle_2_Draw ( unsigned int PADDLE_2_X, unsigned int PADDLE_2_Y )
{
	/* Declaring the local variables required for drawing a line */
	signed int Game_status = 0;												// Status variable to check for errors.
	
	current_paddle_2_x_1 = PADDLE_2_X;
	current_paddle_2_y_1 = PADDLE_2_Y;
	current_paddle_2_x_2 = PADDLE_2_X + PADDLE_LENGTH;
	current_paddle_2_y_2 = PADDLE_2_Y + PADDLE_WIDTH ;
	
	Game_status = Graphics_drawBox ( previous_paddle_2_x_1, previous_paddle_2_y_1, previous_paddle_2_x_2, previous_paddle_2_y_2, LT24_BLACK, false, LT24_BLACK); // Erase the paddle.
	if ( Game_status != GAME_SUCCESS) return Game_status;					// If there is an error, report it.
	
	Game_status = Graphics_drawBox ( current_paddle_2_x_1, current_paddle_2_y_1, current_paddle_2_x_2, current_paddle_2_y_2, LT24_WHITE, false, LT24_WHITE ); // Draw the paddle.
	if ( Game_status != GAME_SUCCESS) return Game_status;					// If there is an error, report it.
	
	/* The current value of the paddle becomes previous value in the next iteration. */
	previous_paddle_2_x_1 = current_paddle_2_x_1;
	previous_paddle_2_y_1 = current_paddle_2_y_1;
	previous_paddle_2_x_2 = current_paddle_2_x_2;
	previous_paddle_2_y_2 = current_paddle_2_y_2;
	
	return GAME_SUCCESS;													// Return the completion of the function.
}

/* Function to draw the dashed line with points: (x1,y1) and (x2,y2) with a boundary colour. */
signed int Graphics_drawDash(unsigned int x1, unsigned int y1, unsigned int x2,  unsigned int y2, unsigned short colour)
{
	/* Declaring the local variables required for drawing a line */
	signed int Game_status = 0;												// Status variable to check for errors.
	signed int count;
			
	for ( count = x1+1 ; count <= x2-1 ; count+=10 )
	{
		if ( count < x2-5 )
		{
			Game_status = Graphics_drawLine(count,y1,count+5,y2,colour);
			if ( Game_status != GAME_SUCCESS) return Game_status;					// If there is an error, report it.
		}
	}
	return GAME_SUCCESS;
}

/* Function to move paddle based on the camera. */
signed int Move_Paddle_1 ( void )
{
	/* Declaring the local variables required for drawing a line */
	signed int Game_status = 0;												// Status variable to check for errors.
	
	/* GPIO Port Base Address. */
	volatile unsigned char *GPIO_ptr  = (unsigned char *) 0xFF200070;
	
	/* LED Base Address. */
	volatile unsigned int *LED_ptr  = (unsigned int *) 0xFF200000; 
	
	/* Variable to hold the value from the slave board */
	unsigned slave_board_value;  
	
	/* Setting all pin as Input. */
	*(GPIO_ptr+4)  = 0x00000001;
	
	slave_board_value = *GPIO_ptr & 0xFFF;
	
	move_paddle_1_x = ( slave_board_value * 5 ) / 4;
	
	if ( move_paddle_1_x <= MINIMUM_PADDLE_X )
	{
		move_paddle_1_x = MINIMUM_PADDLE_X;
	}
	else if ( move_paddle_1_x >= ( MAXIMUM_PADDLE_X - PADDLE_LENGTH ) )
	{
		move_paddle_1_x = MAXIMUM_PADDLE_X - PADDLE_LENGTH;
	}
   
	//Paddle_1_Draw ( current_paddle_1_x, PADDLE_Y_1 ); HPS_ResetWatchdog();
	Game_status = Paddle_1_Draw ( move_paddle_1_x, PADDLE_1_START_Y_1 ); HPS_ResetWatchdog();
	if ( Game_status != GAME_SUCCESS) return Game_status;					// If there is an error, report it.
	
	return GAME_SUCCESS;
}

/* Function to move paddle based on the buttons. */
signed int Move_Paddle_2 ( void )
{
	/* Declaring the local variables required for drawing a line */
	signed int Game_status = 0;												// Status variable to check for errors.
	
	volatile unsigned int *KEY_ptr     = (unsigned int *) 0xFF200050;
	unsigned key_value;
	
	key_value = *KEY_ptr & 0x3;

	if ( key_value == 1 )
	{
		
		if ( move_paddle_2_x >= ( MAXIMUM_PADDLE_X - PADDLE_LENGTH ) )
		{
			move_paddle_2_x = MAXIMUM_PADDLE_X - PADDLE_LENGTH;
		}
		
		else if ( move_paddle_2_x < MAXIMUM_PADDLE_X )
		{
			move_paddle_2_x = move_paddle_2_x + PADDLE_INCREMENT;

			if ( move_paddle_2_x >= ( MAXIMUM_PADDLE_X - PADDLE_LENGTH ) )
			{
				move_paddle_2_x = MAXIMUM_PADDLE_X - PADDLE_LENGTH;
			}
		}
		
		//Reset watchdog.
		HPS_ResetWatchdog();
	}
	else if ( key_value == 2 )
	{
		if ( move_paddle_2_x <= MINIMUM_PADDLE_X )
		{
			move_paddle_2_x = MINIMUM_PADDLE_X;
		}
		else if ( move_paddle_2_x > MINIMUM_PADDLE_X )
		{
			move_paddle_2_x = move_paddle_2_x - PADDLE_DECREMENT;
			
			if ( move_paddle_2_x <= MINIMUM_PADDLE_X )
			{
				move_paddle_2_x = MINIMUM_PADDLE_X;
			}
		}
		
		//Reset watchdog.
		HPS_ResetWatchdog();
	}
   
	//Paddle_1_Draw ( current_paddle_1_x, PADDLE_Y_1 ); HPS_ResetWatchdog();
	Game_status = Paddle_2_Draw ( move_paddle_2_x, PADDLE_2_START_Y_1 ); HPS_ResetWatchdog();
	if ( Game_status != GAME_SUCCESS) return Game_status;					// If there is an error, report it.
	
	return GAME_SUCCESS;
}

/* Function to draw ball on the screen. */
signed int Graphics_drawBall(unsigned int x1, unsigned int y1, unsigned int x2,  unsigned int y2, unsigned short colour)
{
	signed int i;
	signed int j;
	for(i=y1;i<=y2;i++)
			{
				for(j=x1;j<=x2;j++)
				{
					LT24_drawPixel(colour,j,i);

				}
			}
	return 0;
}

/* Function to erase ball from the screen. */
signed int Graphics_Erase(unsigned int x1, unsigned int y1, unsigned int x2,  unsigned int y2, unsigned short fillColour)
{
	signed int i;
	signed int j;
		for(i=y1;i<=y2;i++)
				{
					for(j=x1;j<=x2;j++)
					{
							LT24_drawPixel(fillColour,j,i);
					}
				}

	return 0;
}

/* Function to move the ball around the screen. */
signed int Move_Ball ( void )
{
	volatile unsigned char *AUDIO_ptr = (unsigned char *)0xFF2000C0;
	float hit_angle = angle*(PI/180);
	
	volatile unsigned int *SW_ptr = (unsigned int *)0xFF200040;
	unsigned int delay = inherent_delay - ( ( *SW_ptr & 0x3FF ) * 3.5) ;
	
	*(AUDIO_ptr+4)  = 0xFF;
	colour = LT24_WHITE;
	fillColour = 0x39E7;
	
	ball_touch_paddle = true;
	
	if (quadrant ==1)
	{
		if  (x2>12 && y2>12)
		{

			*AUDIO_ptr = 0x0;
			x2 = x2-1;
			y2 = y2 - abs(round_off(1/tan(hit_angle)));
			Graphics_drawBall(x2,y2,x2+3,y2+3,colour);
			usleep(delay);
			Graphics_Erase(x2,y2,x2+3,y2+3,fillColour);
			Graphics_drawDash(11,160,229,160,LT24_WHITE);
		}
		
		if (x2 <= 12)
		{
			*AUDIO_ptr = 0xF;
			usleep(10000);
			quadrant = 2;
		}
		else if(y2 <= 12)
		{
			if ( x2 > current_paddle_1_x_1 && x2 < current_paddle_1_x_2 )
			{
				*AUDIO_ptr = 0xF;
				usleep(10000);
				quadrant = 4;
			}
			else 
			{
				Update_Score_2();
				ball_touch_paddle = false;
			}
		}

	}

	else if (quadrant ==2)
	{
		if (x2<225 && y2>12)
		{
			*AUDIO_ptr = 0x0;
			x2 = x2+1;
			y2 = y2 - abs(round_off(1/tan(hit_angle)));
			Graphics_drawBall(x2,y2,x2+3,y2+3,colour);
			usleep(delay);
			Graphics_Erase(x2,y2,x2+3,y2+3,fillColour);
			Graphics_drawDash(11,160,229,160,LT24_WHITE);
		}

		if (x2 >= 225)
		{
			quadrant = 1;
			*AUDIO_ptr = 0xF;
			usleep(10000);
		}
		else if(y2 <= 12)
		{
			if ( x2 > current_paddle_1_x_1 && x2 < current_paddle_1_x_2 )
			{
				*AUDIO_ptr = 0xF;
				usleep(10000);
				quadrant = 3;
			}
			else 
			{
				Update_Score_2();
				ball_touch_paddle = false;
			}
		}
	}
		
	//else if (angle>180 && angle<270)
	else if (quadrant ==3)
	{
		if (x2<225 && y2<305)
		{
			*AUDIO_ptr = 0x0;
			x2 = x2+1;
			y2 = y2 + abs(round_off(1/tan(hit_angle)));
			Graphics_drawBall(x2,y2,x2+3,y2+3,colour);
			usleep(delay);
			Graphics_Erase(x2,y2,x2+3,y2+3,fillColour);
			Graphics_drawDash(11,160,229,160,LT24_WHITE);
		}

		if (x2 >= 225)
		{

			quadrant = 4;
			*AUDIO_ptr = 0xF;
			usleep(10000);
		}
		else if(y2 >= 305)
		{
			if ( x2 > current_paddle_2_x_1 && x2 < current_paddle_2_x_2 )
			{
				*AUDIO_ptr = 0xF;
				usleep(10000);
				quadrant = 2;
			}
			else 
			{
				Update_Score_1();
				ball_touch_paddle = false;
			};
		}
	}
		//else 	if (angle>270 && angle<360)
	else if (quadrant == 4)
	{
		if (x2>12 && y2<305)
		{
			*AUDIO_ptr = 0x0;
			x2 = x2-1;
			y2 = y2 + abs(round_off(1/tan(hit_angle)));
			Graphics_drawBall(x2,y2,x2+3,y2+3,colour);
			usleep(delay);
			Graphics_Erase(x2,y2,x2+3,y2+3,fillColour);
			Graphics_drawDash(11,160,229,160,LT24_WHITE);
		}

		if (x2 <= 12)
		{
			quadrant = 3;
			*AUDIO_ptr = 0xF;
			usleep(10000);
		}
		else if(y2 >= 305)
		{
			if ( x2 > current_paddle_2_x_1 && x2 < current_paddle_2_x_2 )
			{
				*AUDIO_ptr = 0xF;
				usleep(10000);
				quadrant = 1;
			}
			else 
			{
				Update_Score_1();
				ball_touch_paddle = false;
			};
		}
	}

	HPS_ResetWatchdog();
	
	if ( ball_touch_paddle == true )
	{
		x1=x2;
		y1=y2;
	}
}

void Update_Score_1 ( void )
{
	Graphics_drawBox ( 115, 75, 125, 85, LT24_GREEN, false, LT24_GREEN ); // Draw the box.
	Graphics_drawBox ( 115, 235, 125, 245, LT24_RED, false, LT24_RED ); // Draw the box.
	
	player_1_score = player_1_score + 1;
	
	Display_Seven_Segment(1,player_1_score);
	Display_Seven_Segment(2,16);
	Display_Seven_Segment(3,16);
	Display_Seven_Segment(4,16);
	Display_Seven_Segment(5,16);
	Display_Seven_Segment(6,player_2_score);
	
	if ( player_1_score >= 10  )
	{
		Graphics_drawBox ( previous_paddle_1_x_1, previous_paddle_1_y_1, previous_paddle_1_x_2, previous_paddle_1_y_2, LT24_BLACK, false, LT24_BLACK); // Erase paddle 1.
		Graphics_drawBox ( previous_paddle_2_x_1, previous_paddle_2_y_1, previous_paddle_2_x_2, previous_paddle_2_y_2, LT24_BLACK, false, LT24_BLACK); // Erase paddle 2.
		
		while (1)
		{
			Graphics_drawBox(10,10,230,310,LT24_BLACK,false,LT24_YELLOW); ResetWDT();
			Graphics_drawLetter(85,215,115,235,7,LT24_BLACK); ResetWDT();
			Graphics_drawLetter(85,190,115,210,8,LT24_BLACK); ResetWDT();
			Graphics_drawLetter(85,165,115,185,9,LT24_BLACK); ResetWDT();
			Graphics_drawLetter(85,140,115,160,9,LT24_BLACK); ResetWDT();
			Graphics_drawLetter(85,115,115,135,10,LT24_BLACK); ResetWDT();
			Graphics_drawLetter(85,90,115,110,4,LT24_BLACK); ResetWDT();
			Graphics_drawLetter(125,165,155,185,6,LT24_BLACK); ResetWDT();
			
			//P1
			Graphics_drawLetter(125,140,155,160,11,LT24_BLACK); ResetWDT();
		}
	}
	
	usleep(3000000);
	
	Graphics_drawBox ( 115, 75, 125, 85, GAME_BACKGROUND_GREY , false, GAME_BACKGROUND_GREY  ); // Erase the box.
	Graphics_drawBox ( 115, 235, 125, 245, GAME_BACKGROUND_GREY , false, GAME_BACKGROUND_GREY  ); // Draw the box.
	
	x1 = 120;
	y1 = 160;
	x2 = 120;
	y2 = 160;
	
	srand(time(0));
	
	quadrant = ((abs( rand())) % 4 ) + 1;
	angle = ((abs( rand())) % 40 )+ 20;
}

void Update_Score_2 ( void )
{
	Graphics_drawBox ( 115, 75, 125, 85, LT24_RED, false, LT24_RED ); // Draw the box.
	Graphics_drawBox ( 115, 235, 125, 245, LT24_GREEN, false, LT24_GREEN ); // Draw the box.
	
	player_2_score = player_2_score + 1;
	
	Display_Seven_Segment(1,player_1_score);
	Display_Seven_Segment(2,16);
	Display_Seven_Segment(3,16);
	Display_Seven_Segment(4,16);
	Display_Seven_Segment(5,16);
	Display_Seven_Segment(6,player_2_score);
	
	if ( player_2_score >= 10 )
	{
		Graphics_drawBox ( previous_paddle_1_x_1, previous_paddle_1_y_1, previous_paddle_1_x_2, previous_paddle_1_y_2, LT24_BLACK, false, LT24_BLACK); // Erase paddle 1.
		Graphics_drawBox ( previous_paddle_2_x_1, previous_paddle_2_y_1, previous_paddle_2_x_2, previous_paddle_2_y_2, LT24_BLACK, false, LT24_BLACK); // Erase paddle 2.

		while (1)
		{
			Graphics_drawBox(10,10,230,310,LT24_BLACK,false,LT24_YELLOW); ResetWDT();
			Graphics_drawLetter(85,215,115,235,7,LT24_BLACK); ResetWDT();
			Graphics_drawLetter(85,190,115,210,8,LT24_BLACK); ResetWDT();
			Graphics_drawLetter(85,165,115,185,9,LT24_BLACK); ResetWDT();
			Graphics_drawLetter(85,140,115,160,9,LT24_BLACK); ResetWDT();
			Graphics_drawLetter(85,115,115,135,10,LT24_BLACK); ResetWDT();
			Graphics_drawLetter(85,90,115,110,4,LT24_BLACK); ResetWDT();
			Graphics_drawLetter(125,165,155,185,6,LT24_BLACK); ResetWDT();

			//P2
			Graphics_drawLetter(125,140,155,160,12,LT24_BLACK); ResetWDT();
		}
	}
	
	usleep(3000000);
	
	Graphics_drawBox ( 115, 75, 125, 85, GAME_BACKGROUND_GREY , false, GAME_BACKGROUND_GREY  ); // Erase the box.
	Graphics_drawBox ( 115, 235, 125, 245, GAME_BACKGROUND_GREY , false, GAME_BACKGROUND_GREY  ); // Draw the box.
	
	x1 = 120;
	y1 = 160;
	x2 = 120;
	y2 = 160;
	
	srand(time(0));
	
	quadrant = ((abs( rand())) % 4 ) + 1;
	angle = ((abs( rand())) % 40 )+ 20;
	
}

/* Round off function that converts float value to integer. */
unsigned int round_off(float num)
{
	int n = (int)num;
	
	if((num-n)>0.5)
	{
		return n+1;
	}
	else {
		return n;
	}
}

/* Seven Segment displays. */
void Display_Seven_Segment(int DISPLAY_NUMBER, int HEX_NUMBER)
{
	if ( DISPLAY_NUMBER == 1 )
	{
		switch (HEX_NUMBER)
		{
			case 0: *HEX_1 = 63;break;
			case 1: *HEX_1 = 6;break;
			case 2: *HEX_1 = 91;break;
			case 3: *HEX_1 = 79;break;
			case 4: *HEX_1 = 102;break;
			case 5: *HEX_1 = 109;break;
			case 6: *HEX_1 = 125;break;
			case 7: *HEX_1 = 07;break;
			case 8: *HEX_1 = 127;break;
			case 9: *HEX_1 = 103;break;
			case 10: *HEX_1 = 119;break;
			case 11: *HEX_1 = 124;break;
			case 12: *HEX_1 = 57;break;
			case 13: *HEX_1 = 94;break;
			case 14: *HEX_1 = 121;break;
			case 15: *HEX_1 = 113;break;

		}
	}

	else if ( DISPLAY_NUMBER == 2 )
	{

		switch (HEX_NUMBER)
		{
			case 0: *HEX_2 = 63;break;
			case 1: *HEX_2 = 6;break;
			case 2: *HEX_2 = 91;break;
			case 3: *HEX_2 = 79;break;
			case 4: *HEX_2 = 102;break;
			case 5: *HEX_2 = 109;break;
			case 6: *HEX_2 = 125;break;
			case 7: *HEX_2 = 07;break;
			case 8: *HEX_2 = 127;break;
			case 9: *HEX_2 = 103;break;
			case 10: *HEX_2 = 119;break;
			case 11: *HEX_2 = 124;break;
			case 12: *HEX_2 = 57;break;
			case 13: *HEX_2 = 94;break;
			case 14: *HEX_2 = 121;break;
			case 15: *HEX_2 = 113;break;
			case 16: *HEX_2 = 0x6F; break;
		}
	}

	else if ( DISPLAY_NUMBER == 3 )
	{

		switch (HEX_NUMBER)
		{
			case 0: *HEX_3 = 63;break;
			case 1: *HEX_3 = 6;break;
			case 2: *HEX_3 = 91;break;
			case 3: *HEX_3 = 79;break;
			case 4: *HEX_3 = 102;break;
			case 5: *HEX_3 = 109;break;
			case 6: *HEX_3 = 125;break;
			case 7: *HEX_3 = 07;break;
			case 8: *HEX_3 = 127;break;
			case 9: *HEX_3 = 103;break;
			case 10: *HEX_3 = 119;break;
			case 11: *HEX_3 = 124;break;
			case 12: *HEX_3 = 57;break;
			case 13: *HEX_3 = 94;break;
			case 14: *HEX_3 = 121;break;
			case 15: *HEX_3 = 113;break;
			case 16: *HEX_3 = 0x54; break;
		}
	}

	else if ( DISPLAY_NUMBER == 4 )
	{

		switch (HEX_NUMBER)
		{
			case 0: *HEX_4 = 63;break;
			case 1: *HEX_4 = 6;break;
			case 2: *HEX_4 = 91;break;
			case 3: *HEX_4 = 79;break;
			case 4: *HEX_4 = 102;break;
			case 5: *HEX_4 = 109;break;
			case 6: *HEX_4 = 125;break;
			case 7: *HEX_4 = 07;break;
			case 8: *HEX_4 = 127;break;
			case 9: *HEX_4 = 103;break;
			case 10: *HEX_4 = 119;break;
			case 11: *HEX_4 = 124;break;
			case 12: *HEX_4 = 57;break;
			case 13: *HEX_4 = 94;break;
			case 14: *HEX_4 = 121;break;
			case 15: *HEX_4 = 113;break;
			case 16: *HEX_4 =0x5C; break;
		}
	}

	else if ( DISPLAY_NUMBER == 5 )
	{

		switch (HEX_NUMBER)
		{
			case 0: *HEX_5 = 63;break;
			case 1: *HEX_5 = 6;break;
			case 2: *HEX_5 = 91;break;
			case 3: *HEX_5 = 79;break;
			case 4: *HEX_5 = 102;break;
			case 5: *HEX_5 = 109;break;
			case 6: *HEX_5 = 125;break;
			case 7: *HEX_5 = 07;break;
			case 8: *HEX_5 = 127;break;
			case 9: *HEX_5 = 103;break;
			case 10: *HEX_5 = 119;break;
			case 11: *HEX_5 = 124;break;
			case 12: *HEX_5 = 57;break;
			case 13: *HEX_5 = 94;break;
			case 14: *HEX_5 = 121;break;
			case 15: *HEX_5 = 113;break;
			case 16: *HEX_5 = 0x73; break;

		}
	}

	else if ( DISPLAY_NUMBER == 6 )
	{

		switch (HEX_NUMBER)
		{
			case 0: *HEX_6 = 63;break;
			case 1: *HEX_6 = 6;break;
			case 2: *HEX_6 = 91;break;
			case 3: *HEX_6 = 79;break;
			case 4: *HEX_6 = 102;break;
			case 5: *HEX_6 = 109;break;
			case 6: *HEX_6 = 125;break;
			case 7: *HEX_6 = 07;break;
			case 8: *HEX_6 = 127;break;
			case 9: *HEX_6 = 103;break;
			case 10: *HEX_6 = 119;break;
			case 11: *HEX_6 = 124;break;
			case 12: *HEX_6 = 57;break;
			case 13: *HEX_6 = 94;break;
			case 14: *HEX_6 = 121;break;
			case 15: *HEX_6 = 113;break;
		}
	}
}
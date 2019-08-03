////////////////////////////////////////////////////////////////////////////
/*
 *  Program for the master board of PONG.
 *  ----------------------------------------
 *  File Name     : main.c
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

#include "Game_Engine/Game_Engine.h"										// Invoking the main header file.
#include "Graphics_Engine/Graphics_Engine.h"								// Importing the graphics engine library for drawing geometrical shapes. 
#include "DE1SoC_LT24/DE1SoC_LT24.h"										// Importing the LCD library for interfacing with the LT24 terasic LCD.
#include "HPS_Watchdog/HPS_Watchdog.h"										// Importing the watchdog timer library to ensure the program doesn't stay in an infinite loop.
#include "HPS_usleep/HPS_usleep.h"											// Importing the sleep library to create instances of time delay.
#include <stdlib.h>															// Importing the standard library functions to perform necessary actions.
#include <time.h>															// Importing the time library so that its values can be used as seed value for generating random numbers.

void exitOnFail(signed int status, signed int successStatus)				// Exit on fail sub-routine is used to ensure that the processor doesn't malfunction.
{
    if (status != successStatus)
    {
        exit((int)status); 													// Add breakpoint here to catch failure
    }
}

/* Main Function.*/
int main(void)
{	
	/* GPIO Port Base Address. */
	volatile unsigned char *GPIO_ptr  = (unsigned char *) 0xFF200070;		// The GPIO of the master board is connected to the slave board for board-to-board communication.
	
	/* Variable to hold the value from the slave board */
	unsigned int slave_board_data = 0;										// This information holds the value of the touch screen and the paddle 1 control values from the slave board. 
	
	/* Setting all pin as Input. */
	*(GPIO_ptr+4)  = 0x000000;												// The master board only receives values from the slave board.
	
    /* Initialise the LCD Display and exit if not successful. */
    exitOnFail( LT24_initialise(0xFF200060,0xFF200080),  LT24_SUCCESS);

	HPS_ResetWatchdog();													// Reset the watch dog timer so that it doesn't run out and restarts the processor.
			
	/* Drawing the gaming area. White Border on two sides and grey sides on two sides. Grey Fill. */
	Graphics_drawBox(10,10,230,310,LT24_WHITE,false,0x39E7); HPS_ResetWatchdog();
	Graphics_drawLine(10,10,230,10,0x39E7); HPS_ResetWatchdog();
	Graphics_drawLine(10,310,230,310,0x39E7); HPS_ResetWatchdog();

	/* Line. Drawing a Dashed Net between the two players. */
	Graphics_drawDash(11,160,229,160,LT24_WHITE); HPS_ResetWatchdog();
	
	/* Initialize the ball with its starting positions. */
    Ball_Initialize(); HPS_ResetWatchdog();
	
    /* Initialize the two paddles with their starting positions. */
    Paddle_1_Initialize(); HPS_ResetWatchdog();
    Paddle_2_Initialize(); HPS_ResetWatchdog();
	
	/* The seven-segment display is used to display player scores and the word Pong. */
	Display_Seven_Segment(1,0);
	Display_Seven_Segment(2,16);
	Display_Seven_Segment(3,16);
	Display_Seven_Segment(4,16);
	Display_Seven_Segment(5,16);
	Display_Seven_Segment(6,0);
	HPS_ResetWatchdog();
	
	/* Infinite Loop. */
	while (1)
	{	
		slave_board_data = *GPIO_ptr & 0xFFFFFFF0;							// Keep storing the data from the slave board as it can stop and start the game.
	
		while ( slave_board_data == 0 ) 									// If the slave board data is 0, then the game should be paused. 
		{
			slave_board_data = *GPIO_ptr & 0xFFFFFFF0;						// Keep checking for new data from the slave board that can change from stop mode to start mode and vice-versa.
			HPS_ResetWatchdog();
		}
		
		Move_Ball();														// If start mode, then keep moving the ball around the gaming area.
		Move_Paddle_1();													// Move paddle 1 continuously in the start mode based on the user input.
		Move_Paddle_2();													// Move paddle 2 continuously in the start mode based on the user input.
		HPS_ResetWatchdog();												// Reset the watch dog timer.
	} 
}

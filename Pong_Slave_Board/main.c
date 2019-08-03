////////////////////////////////////////////////////////////////////////////
/*
 *  Program for the slave board of PONG.
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
 *      read the player's input using the camera and touch screen. 
 */
////////////////////////////////////////////////////////////////////////////

#include "Graphics_Engine/Graphics_Engine.h"								// Importing the graphics engine library for drawing geometrical shapes. 
#include "DE1SoC_LT24/DE1SoC_LT24.h"										// Importing the LCD library for interfacing with the LT24 terasic LCD.
#include "HPS_Watchdog/HPS_Watchdog.h"										// Importing the watchdog timer library to ensure the program doesn't stay in an infinite loop.
#include "HPS_usleep/HPS_usleep.h"											// Importing the sleep library to create instances of time delay.
#include <stdlib.h>															// Importing the standard library functions to perform necessary actions.
#include <time.h>															// Importing the time library so that its values can be used as seed value for generating random numbers.
#include "pong.h"															// Importing the pong library that has the bit map of the image to be displayed.

void exitOnFail(signed int status, signed int successStatus)				// Exit on fail sub-routine is used to ensure that the processor doesn't malfunction.
{
    if (status != successStatus)
    {
        exit((int)status); 													// Add breakpoint here to catch failure
    }
}


/* Main Function. */
int main(void)
{	
	/* The timer of the ARM processor is used to the detect the PWM signals from the camera module. */
	/* ARM A9 Private Timer Load. */
	volatile unsigned int *private_timer_load = (unsigned int *) 0xFFFEC600;	
	
	/* ARM A9 Private Timer Value. */
    volatile unsigned int *private_timer_value = (unsigned int *) 0xFFFEC604;
    
	/* ARM A9 Private Timer Control. */
    volatile unsigned int *private_timer_control = (unsigned int *) 0xFFFEC608;
    
	/* ARM A9 Private Timer Interrupt. */
    volatile unsigned int *private_timer_interrupt = (unsigned int *) 0xFFFEC60C;
	
	//////////////////////////////////////////////////////////////////////////////
	
	/* The touch screen is used to start or stop the game. */
	/* Touch screen Port Base Address. */
	volatile unsigned int *TOUCH_SCREEN_ptr  = (unsigned int *) 0xFF200060; 
	
	/* The GPIO port is used to communicate between the master board and slave board. */
	/* GPIO Port Base Address. */
	volatile unsigned char *GPIO_ptr  = (unsigned char *) 0xFF200070; 
	
	/* The LEDs on the slave pong board are diplay the PWM signal from the camera and are used for debugging purpose. */
	/* LED Base Address. */
	volatile unsigned int *LED_ptr  = (unsigned int *) 0xFF200000; 
	
	/* Variables to detect rising and falling edge of the PWM signal. */
	/* The value of the GPIO input. */
	unsigned int current_GPIO_value = 0;
	unsigned int previous_GPIO_value = 0;
	
	/* Variables to determine the ON time of the PWM signal. */
	/* Hold Initial Value, Final Value */
    unsigned int initial_count = 0;
	unsigned int final_count = 0;
	
	/* Variable to hold the value of the touch screen. */
	/* Touch screen value. */
	unsigned int touch_screen_value = 0;
	
	/* Start or Stop mode of the game that is determined by the touch screen. 
		mode = 0 => stop mode and mode = 0 => start mode*/
	int mode = 0;
  	
	/* Initialise the LCD Display and exit if not successful. */
    exitOnFail( LT24_initialise(0xFF200060,0xFF200080),  LT24_SUCCESS); HPS_ResetWatchdog();
	exitOnFail( LT24_copyFrameBuffer(pong,10,160,220,150), LT24_SUCCESS); HPS_ResetWatchdog();
	
	//////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////  DRAW THE MAIN SCREEN OF THE GAME //////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	
	Graphics_drawBox(10,10,230,310,LT24_WHITE,true,0x39E7); ResetWDT();

	//START
	Graphics_drawBox(10,10,120,160,LT24_WHITE,false,LT24_GREEN); ResetWDT();
	//STOP
	Graphics_drawBox(120,10,230,160,LT24_WHITE,false,LT24_RED); ResetWDT();

	//S=1
	Graphics_drawLetter(50,125,80,145,1,LT24_BLACK); ResetWDT();
	//T=2
	Graphics_drawLetter(50,100,80,120,2,LT24_BLACK); ResetWDT();
	//A=3
	Graphics_drawLetter(50,75,80,95,3,LT24_BLACK); ResetWDT();
	//R=4
	Graphics_drawLetter(50,50,80,70,4,LT24_BLACK); ResetWDT();
	//T=2
	Graphics_drawLetter(50,25,80,45,2,LT24_BLACK); ResetWDT();

	//S=1
	Graphics_drawLetter(150,115,180,135,1,LT24_WHITE); ResetWDT();
	//T=2
	Graphics_drawLetter(150,90,180,110,2,LT24_WHITE); ResetWDT();
	//O=5
	Graphics_drawLetter(150,65,180,85,5,LT24_WHITE); ResetWDT();
	//P=6
	Graphics_drawLetter(150,40,180,60,6,LT24_WHITE); ResetWDT();
	
	//////////////////////////////////////////////////////////////////////////////
	
	/* Loading the timer load with a value. */
	*private_timer_load = 100000000;
	
	/* Setting the first pin as Input and the remaining as output. */
	*(GPIO_ptr+4)  = 0xFFFFFFFE;
    
	/* Checking if the touch screen has been tapped. */
	touch_screen_value = *TOUCH_SCREEN_ptr & 0x20000000;
	
	/* Don't start the game until the touch screen has been tapped. */
	while ( touch_screen_value != 0 )
	{
		touch_screen_value = *TOUCH_SCREEN_ptr & 0x20000000;						// Keep monitoring the touch screen for taps.
		*GPIO_ptr = 0;																// Inform the current mode to the master board.
		HPS_ResetWatchdog();														// Reset the watch dog timer periodically.
	}
	
	/* Mode = start. */
	mode = mode ^ 0x1; 																// Change mode to start.

	*GPIO_ptr = 0xFF;																// Inform the master board that the mode is start game.
	usleep(5000);																	// Do nothing for 5 ms so that the master board has received the data.
	
    /* Main Run Loop. */
    while (1) 
	{
		touch_screen_value = *TOUCH_SCREEN_ptr & 0x20000000;						// Keep monitoring the touch screen for taps.
		if ( touch_screen_value == 0 )												// Change modes after each tap of the touch screen.
		{
			/* Invert the mode. */
			mode = mode ^ 0x1;														// Change modes to stop or start.
			usleep(50000);															
		}	
		
		/* In start mode the slave pong board must communicate with the camera module and retrieve the location of the paddle. */
		if ( mode == 1 )
		{
			current_GPIO_value = *GPIO_ptr & 0x1;									// Measure the signal from the camera module.
			
			if ( current_GPIO_value == 1 && previous_GPIO_value == 0 )				// Check for rising edge of the PWM signal.
			{
				initial_count = *private_timer_value;								
				*private_timer_control = (0 << 8) | (0 << 2) | (1 << 1) | (1 << 0);	// Start the timer so that the pulse width can be measured.

				while ( current_GPIO_value == 1 )									
				{
					current_GPIO_value = *GPIO_ptr & 0x1;

					HPS_ResetWatchdog();

					if ( current_GPIO_value == 0 )
					{
						*private_timer_control = (0 << 8) | (0 << 2) | (1 << 1) | (0 << 0);
						final_count = initial_count - *private_timer_value;
						*GPIO_ptr = final_count / 100;								// The pulse width conveying the location of the paddle is sent to the master board as 8-bit parallel signal.
						*LED_ptr = final_count / 100;
						HPS_ResetWatchdog();
					}
					
					previous_GPIO_value = current_GPIO_value;
					
				}
			}
			
			previous_GPIO_value = current_GPIO_value;
		}
		
		else if ( mode == 0 )														// If the mode is stop notify the master pong board about the same.
		{
			*GPIO_ptr = 0x00000000;
		}
		
		/* Finally reset the watchdog. */
        HPS_ResetWatchdog();
    } 
}

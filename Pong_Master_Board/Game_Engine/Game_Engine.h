////////////////////////////////////////////////////////////////////////////
/*
 *  Game Engine for PONG.
 *  ----------------------------------------
 *  File Name     : Game_Engine.h
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
 *		movement of the ball, players' scores and game-play of PONG. This 
 *		driver imports other libraries to access hardware resources and 
 *		communicate with them.
 */
////////////////////////////////////////////////////////////////////////////

#ifndef GAME_ENGINE_H_
#define GAME_ENGINE_H_

/* Importing the required libraries. */
/* Boolean variable type "bool" and "true"/"false" constants. */
#include <stdbool.h> 														

/* Error Codes*/
#define GAME_SUCCESS       0

/* Creating a function that returns maximum and minimum of two numbers. */
#define MAX(number_1, number_2) (((number_1) > (number_2)) ? (number_1) : (number_2))
#define MIN(number_1, number_2) (((number_1) < (number_2)) ? (number_1) : (number_2))

//////////////////////////////////////// INITIAL CONSTANT POSITIONS //////////////////////////////////////

/* A paddle is a rectangle with length 20 pixels and width 5 width. Therefore, it has a right top coordinate and left-bottom coordinate. */

/* Constant paddle dimensions. */
#define PADDLE_LENGTH 40
#define PADDLE_WIDTH  2

#define PADDLE_1_CENTER_X	119
#define PADDLE_2_CENTER_X	119

#define MINIMUM_PADDLE_X 11
#define MAXIMUM_PADDLE_X 229

#define PADDLE_INCREMENT 1
#define PADDLE_DECREMENT 1

/* Paddle 1 (x1,y1) point. */
#define PADDLE_1_START_X_1	PADDLE_1_CENTER_X - ( PADDLE_LENGTH / 2 )
#define PADDLE_1_START_Y_1	7

/* Paddle 1 (x2,y2) point. */
#define PADDLE_1_START_X_2	PADDLE_1_CENTER_X + ( PADDLE_LENGTH / 2 )
#define PADDLE_1_START_Y_2	9

/* Paddle 2 (x1,y1) point. */
#define PADDLE_2_START_X_1	PADDLE_2_CENTER_X - ( PADDLE_LENGTH / 2 )
#define PADDLE_2_START_Y_1	311

/* Paddle 2 (x2,y2) point. */
#define PADDLE_2_START_X_2	PADDLE_2_CENTER_X + ( PADDLE_LENGTH / 2 )
#define PADDLE_2_START_Y_2	313

/* The colour of the background. */
#define GAME_BACKGROUND_GREY (0x39E7) 

//////////////////////////////////////// DECLARATION OF FUNCTIONS  ///////////////////////////////////////

/* Function to initialize PADDLE 1 with two coordinates: top-left (x1,y1) and bottom_right (x2,y2) with a boundary colour. */
signed int Paddle_1_Initialize ( void );

/* Function to initialize PADDLE 2 with two coordinates: top-left (x1,y1) and bottom_right (x2,y2) with a boundary colour. */
signed int Paddle_2_Initialize ( void );

/* Function to draw PADDLE 1 with two coordinates: top-left (x,y) and fixed paddle dimensions with a boundary colour. */
signed int Paddle_1_Draw ( unsigned int PADDLE_1_X, unsigned int PADDLE_1_Y );

/* Function to draw PADDLE 2 with two coordinates: top-left (x,y) and fixed paddle dimensions with a boundary colour. */
signed int Paddle_2_Draw ( unsigned int PADDLE_2_X, unsigned int PADDLE_2_Y );

/* Function to draw the dashed line with points: (x1,y1) and (x2,y2) with a boundary colour. */
signed int Graphics_drawDash(unsigned int x1, unsigned int y1, unsigned int x2,  unsigned int y2, unsigned short colour);

/* Function to move paddle 1 based on the buttons. */
signed int Move_Paddle_1 ( void );

/* Function to move paddle 2 based on the buttons. */
signed int Move_Paddle_2 ( void );

/* Function to draw ball on the screen. */
signed int Graphics_drawBall(unsigned int x1, unsigned int y1, unsigned int x2,  unsigned int y2, unsigned short colour);

/* Function to erase ball from the screen. */
signed int Graphics_Erase(unsigned int x1, unsigned int y1, unsigned int x2,  unsigned int y2, unsigned short fillColour);

/* Function to move the ball around the screen. */
signed int Move_Ball ( void );

/* Round off function. */
unsigned int round_off(float num);

/* Seven Segment displays. */
void Display_Seven_Segment(int DISPLAY_NUMBER, int HEX_NUMBER);

/* Function to update scores of players. */
void Update_Score_1 ( void );
void Update_Score_2 ( void );

#endif /* GAME_ENGINE_H_ */

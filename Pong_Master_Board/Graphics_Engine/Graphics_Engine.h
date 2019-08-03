////////////////////////////////////////////////////////////////////////////
/*
 *  Driver for LT24 Display Graphics Engine
 *  ----------------------------------------
 *  File Name     : Graphics_Engine.h
 *  Target Device : ARM Cortex-A9 processor
 *
 *  Created on    : March 15, 2019
 *  Code Author   : S. Bhandary
 *  Location 	  : University of Leeds
 *  Module 		  : ELEC5620M Embedded Microprocessor System Design
 *
 *  Description of the file:
 *  	The driver is used to draw different geometrical shapes on the LT24
 *  	display. This driver imports other libraries to access hardware
 *  	resources and communicate with them.
 */
////////////////////////////////////////////////////////////////////////////

#ifndef GRAPHICS_ENGINE_H_
#define GRAPHICS_ENGINE_H_

/* Importing the required libraries. */
/* Boolean variable type "bool" and "true"/"false" constants. */
#include <stdbool.h> 														

/* Error Codes*/
#define GE_SUCCESS       0

/* Creating a function that returns maximum and minimum of two numbers. */
#define MAX(number_1, number_2) (((number_1) > (number_2)) ? (number_1) : (number_2))
#define MIN(number_1, number_2) (((number_1) < (number_2)) ? (number_1) : (number_2))

//////////////////////////////////////// DECLARATION OF FUNCTIONS  ///////////////////////////////////////

/* Function to draw a box with two coordinates: top-left (x1,y1) and bottom_right (x2,y2) with a boundary colour. 
   The function has a boolean argument that decides whether the box should be filled or not with the colour as that of the boundary.*/
signed int Graphics_drawBox ( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned short colour, bool noFill, unsigned short fillColour );

/* Function to draw a circle with two centre: (x,y) and radius with a boundary colour. 
   The function has a boolean argument that decides whether the circle should be filled or not with with a particular colour mentioned.*/
signed int Graphics_drawCircle ( unsigned int x, unsigned int y, unsigned int r, unsigned short colour, bool noFill, unsigned short fillColour );

/* Graphic function to draw a straight line. Returns 0 if successful .*/
signed int Graphics_drawLine ( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned short colour );

/* Function to draw a triangle with three vertices: (x1,y1), (x2,y2) and (x3,y3) with a boundary colour. 
   The function has a boolean argument that decides whether the triangle should be filled or not with with a particular colour mentioned.*/
signed int Graphics_drawTriangle( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int x3, unsigned int y3, unsigned short colour, bool noFill, unsigned short fillColour );

/* Function to initialize the LCD. */
void Graphics_initialise( unsigned int lcd_pio_base, unsigned int lcd_hw_base );

/* Graphic function to determine whether a coordinate lies inside the triangle to fill triangle.*/
bool Graphics_Engine_Point_Test ( unsigned int point_x, unsigned int point_y, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int x3, unsigned int y3);

/* Function to swap axes i.e., swap x and y points */
unsigned int* Graphics_Engine_Swap_Vertices ( unsigned int point_x, unsigned int point_y );

/* Graphic function to draw letters on the LCD screen. */
signed int Graphics_drawLetter(unsigned int x1, unsigned int y1, unsigned int x2,  unsigned int y2, unsigned int score, unsigned short colour);

#endif /* GRAPHICS_ENGINE_H_ */

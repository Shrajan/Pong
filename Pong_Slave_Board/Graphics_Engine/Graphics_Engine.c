////////////////////////////////////////////////////////////////////////////
/*
 *  Driver for LT24 Display Graphics Engine
 *  ----------------------------------------
 *  File Name     : Graphics_Engine.c
 *  Target Device : ARM Cortex-A9 processor
 *
 *  Created on    : March 15, 2019
 *  Code Author   : S. Bhandary
 *  Location 	  : University of Leeds
 *  Module 		  : ELEC5620M Embedded Microprocessor System Design
 *
 *  Description of the file:
 *  	The driver is used to draw different geometrical shapes on the LT24
 *  	display. This file imports other libraries to access hardware
 *  	resources and communicate with them. The file contains the source
 *  	file of all the functions defined in the header file.
 */
////////////////////////////////////////////////////////////////////////////

#include "Graphics_Engine.h"												// Invoking the main header file.
#include "../DE1Soc_LT24/DE1SoC_LT24.h"										// Importing the Leeds SoC LT24 driver controller.
#include "math.h"															// Importing the mathematical library of C.

/* Graphics initialisation.  */
void Graphics_initialise( unsigned int lcd_pio_base, unsigned int lcd_hw_base )
{
	if ( LT24_isInitialised() == false )									// Check if LT24 has been initialised.
	{
		LT24_initialise	( lcd_pio_base, lcd_hw_base );						// If it has nor been initialised, then call LT24_initialise function.
	}
}

/* Graphic function to draw a straight line. Returns 0 if successful .*/
signed int Graphics_drawLine ( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned short colour )
{
	/* Declaring the local variables required for drawing a line */
	signed int GE_status = 0;												// Status variable to check for errors.
	
	bool is_line_steep = (abs (  y2 - y1 )) > (abs (  x2 - x1 ));			// Boolean to determine line is steep or not.
	unsigned int *swapped_points;											// Array Pointer to hold swapped coordinates.
	signed int delta_x;														// To hold the difference between x2 and x1.
	signed int delta_y;														// To hold the difference between y2 and y1.
	signed int increment_error;												// To hold the error.
	unsigned int current_x;													// The x - point from where the pixel has to be drawn.
	unsigned int current_y;													// The y - point from where the pixel has to be drawn.
	signed int step_y = 0;													// Determines line should go upwards or downwards.
	
	if ( y1 == y2 )															// Checking if the line is horizontal.
	{	
		current_y = y1;														// Since y does not change, it could either y1 or y2.
		if ( x1 > x2 )														// Checking if the line is vertically decreasing.
		{
			for ( current_x = x1; current_x >= x2; current_x -- )			// Draw a line from (x2,y2) to (x1,y1)
			{
				GE_status = LT24_drawPixel ( colour, current_x, current_y);	// Draw single pixel.
				if ( GE_status != GE_SUCCESS) return GE_status;				// If there is an error, report it.
			}
		}
		else 																// Otherwise the line is vertically increasing.
		{
			for ( current_x = x2; current_x >= x1; current_x -- )			// Draw a line from (x1,y1) to (x2,y2)
			{
				GE_status = LT24_drawPixel ( colour, current_x, current_y);	// Draw single pixel.
				if ( GE_status != GE_SUCCESS) return GE_status;				// If there is an error, report it.
			}
		}
	}
	
	else if ( x1 == x2 )													// Checking if the line is vertical.
	{	
		current_x = x1;														// Since x does not change, it could either x1 or x2.
		if ( y1 > y2 )														// Checking if the line is horizontal decreasing.
		{
			for ( current_y = y1; current_y >= y2; current_y -- )			// Draw a line from (x2,y2) to (x1,y1)
			{
				GE_status = LT24_drawPixel ( colour, current_x, current_y);	// Draw single pixel.
				if ( GE_status != GE_SUCCESS) return GE_status;				// If there is an error, report it.
			}
		}
		else 																// Otherwise the line is horizontal increasing.
		{
			for ( current_y = y2; current_y >= y1; current_y -- )			// Draw a line from (x1,y1) to (x2,y2)
			{
				GE_status = LT24_drawPixel ( colour, current_x, current_y);	// Draw single pixel.
				if ( GE_status != GE_SUCCESS) return GE_status;				// If there is an error, report it.
			}
		}
	}
	
	
	else 																	// Implementing Bresenham's algorithm 
	{
		if ( is_line_steep )												// Checking if the line is steep.
		{
			swapped_points = Graphics_Engine_Swap_Vertices ( x1, y1 );		// If line is steep, then swap the points.
			x1 = swapped_points [0];
			y1 = swapped_points [1];
			
			swapped_points = Graphics_Engine_Swap_Vertices ( x2, y2 );
			x2 = swapped_points [0];
			y2 = swapped_points [1];
		}
		
		if ( x1 > x2 )														// If the line is rising, then swap the coordinates.
		{
			swapped_points = Graphics_Engine_Swap_Vertices ( x1, x2 );
			x1 = swapped_points [0];
			x2 = swapped_points [1];
			
			swapped_points = Graphics_Engine_Swap_Vertices ( y1, y2 );
			y1 = swapped_points [0];
			y2 = swapped_points [1];
		}
		
		delta_x = x2 - x1;													// Calculate the difference between x2 and x1.  
		delta_y = abs (y2 - y1);											// Calculate the difference between y2 and y1.
		
		increment_error = - ( delta_x / 2);									// Calculate the error.
		
		current_y = y1;														// Assign the initial y coordinate.
		
		if ( y1 < y2 )														// Checking if the line is rising.
		{
			step_y = 1;														// If the line is rising, slope is 1.
		}
		else if ( y1 > y2 )													// Checking if the line is falling.
		{
			step_y = -1;													// If the line is falling, slop is -1.
		}
		
		for ( current_x = x1; current_x <= x2 ; current_x ++ )				// Iterate through all the points.
		{
			if ( is_line_steep ) 
			{
				GE_status = LT24_drawPixel ( colour, current_y,current_x );	// Draw single pixel.
				if ( GE_status != GE_SUCCESS) return GE_status;				// If there is an error, report it.
			}
			
			else
			{
				GE_status = LT24_drawPixel ( colour, current_x, current_y);	// Draw single pixel.
				if ( GE_status != GE_SUCCESS) return GE_status;				// If there is an error, report it.
			}
			
			increment_error = increment_error + delta_y;					// Recalculate the error.
			
			if ( increment_error >= 0 )										// Check whether the error is greater than 0.
			{
				current_y = current_y + step_y;								// Calculate values for y.
				increment_error = increment_error - delta_x;				// Recalculate the error.
			}
		}
	}
	
	return GE_SUCCESS;
}

/* Function to draw a box with two coordinates: top-left (x1,y1) and bottom_right (x2,y2) with a boundary colour. 
   The function has a boolean argument that decides whether the box should be filled or not with the colour as that of the boundary.*/
signed int Graphics_drawBox ( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned short colour, bool noFill, unsigned short fillColour )
{
	/* Declaring the local variables required for drawing a line */
	signed int GE_status = 0;												// Status variable to check for errors.
	
	unsigned int current_x;													// The x - point from where the pixel has to be drawn.
	unsigned int current_y;													// The y - point from where the pixel has to be drawn.	
	
	GE_status = Graphics_drawLine(x1,y1,x2,y1,colour);						// Draw a line from (x1,y1) to (x2,y1)
	if ( GE_status != GE_SUCCESS) return GE_status;							// If there is an error, report it.
	
	GE_status = Graphics_drawLine(x1,y1,x1,y2,colour);						// Draw a line from (x1,y1) to (x1,y2)
	if ( GE_status != GE_SUCCESS) return GE_status;							// If there is an error, report it.
	
	GE_status = Graphics_drawLine(x2,y1,x2,y2,colour);						// Draw a line from (x2,y1) to (x2,y2)
	if ( GE_status != GE_SUCCESS) return GE_status;							// If there is an error, report it.
	
	GE_status = Graphics_drawLine(x1,y2,x2,y2,colour);						// Draw a line from (x1,y2) to (x2,y2)
	if ( GE_status != GE_SUCCESS) return GE_status;							// If there is an error, report it.
	
	if ( noFill == false ) 
	{
		for ( current_x = x2-1; current_x >= x1+1; current_x -- )			// Iterate through each row.
		{
			for ( current_y = y2-1; current_y >= y1+1; current_y -- )		// Fill each column of the every row.
			{
				GE_status = LT24_drawPixel(fillColour,current_x,current_y); // Draw single pixel.
				if ( GE_status != GE_SUCCESS) return GE_status;				// If there is an error, report it.
			}
		}
	}
	
	return GE_SUCCESS;
}

/* Function to draw a circle with two centre: (x,y) and radius with a boundary colour. 
   The function has a boolean argument that decides whether the circle should be filled or not with with a particular colour mentioned.*/
signed int Graphics_drawCircle ( unsigned int x, unsigned int y, unsigned int r, unsigned short colour, bool noFill, unsigned short fillColour )
{
	/* Declaring the local variables required for drawing a line */
	signed int GE_status = 0;												// Status variable to check for errors.
	
	int current_x = r-1;													// The x - point from where the pixel has to be drawn.
    int current_y = 0;														// The y - point from where the pixel has to be drawn.
    int delta_x = 1;														// To hold the difference between x2 and x1.
    int delta_y = 1;														// To hold the difference between y2 and y1.
    int increment_error = delta_x - (r << 1);								// To hold the error.
	int fill_radius;														// Radius of concentric circles.
	
	if ( noFill == false )
	{		
		/*
			The following code generates concentric circles starting with radius = (r-1) pixels until radius = 1 pixel.
			However, this creates a Moire pattern with multiple discontinued pixels. Uncomment the following for-loop to check this.*/
		/*
		for ( fill_radius = (r-1) ; fill_radius >= 1 ; fill_radius -- )
		{
			GE_status = Graphics_drawCircle(x, y, fill_radius, fillColour, true , 0 ); 	// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;								// If there is an error, report it.
		}
		*/
		
		while (current_x >= current_y)
		{
			/* Octant #1 */
			GE_status = Graphics_drawLine( x, y+current_y, x+current_x, y+current_y, fillColour);	// Draw a line from (x,y) to the circumference of the circle.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			GE_status = LT24_drawPixel( colour, x + current_x, y + current_y); 						// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			/* Octant #2 */
			GE_status = Graphics_drawLine( x, y + current_x, x+current_y, y+current_x, fillColour);	// Draw a line from (x,y) to the circumference of the circle.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.

			GE_status = LT24_drawPixel( colour, x + current_y, y + current_x);						// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			/* Octant #3 */
			GE_status = Graphics_drawLine( x, y + current_x, x-current_y, y+current_x, fillColour);	// Draw a line from (x,y) to the circumference of the circle.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			GE_status = LT24_drawPixel( colour, x - current_y, y + current_x);						// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			/* Octant #4 */
			GE_status = Graphics_drawLine( x, y+current_y, x-current_x, y+current_y, fillColour);	// Draw a line from (x,y) to the circumference of the circle.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			GE_status = LT24_drawPixel( colour, x - current_x, y + current_y);						// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			/* Octant #5 */
			GE_status = Graphics_drawLine( x, y-current_y, x-current_x, y-current_y, fillColour);	// Draw a line from (x,y) to the circumference of the circle.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			GE_status = LT24_drawPixel( colour, x - current_x, y - current_y);						// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			/* Octant #6 */
			GE_status = Graphics_drawLine( x, y-current_x, x-current_y, y-current_x, fillColour);	// Draw a line from (x,y) to the circumference of the circle.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			GE_status = LT24_drawPixel( colour, x - current_y, y - current_x);						// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			/* Octant #7 */
			GE_status = Graphics_drawLine( x, y-current_x, x+current_y, y-current_x, fillColour);	// Draw a line from (x,y) to the circumference of the circle.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			GE_status = LT24_drawPixel( colour, x + current_y, y - current_x);						// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			/* Octant #8 */
			GE_status = Graphics_drawLine( x, y-current_y, x+current_x, y-current_y, fillColour);	// Draw a line from (x,y) to the circumference of the circle.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
			
			GE_status = LT24_drawPixel( colour, x + current_x, y - current_y);						// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.

			if (increment_error <= 0)																// Check whether the error is less than 0.
			{
				current_y++;																		// Increment y-points.
				increment_error += delta_y;															// Recalculate the error.
				delta_y += 2;																		// Recalculate delta y.
			}
			
			if (increment_error > 0)																// Check whether the error is less than 0.	
			{
				current_x--;																		// Decrement x-points.
				delta_x += 2;																		// Recalculate delta x.
				increment_error += delta_x - (r << 1);												// Recalculate the error.
			}
		}
	}
	
	else 
	{
		while (current_x >= current_y)
		{	
			/* Octant #1 */
			GE_status = LT24_drawPixel(colour, x + current_x, y + current_y); 	// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.
			
			/* Octant #2 */
			GE_status = LT24_drawPixel(colour, x + current_y, y + current_x);	// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.
			
			/* Octant #3 */
			GE_status = LT24_drawPixel(colour, x - current_y, y + current_x);	// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.
			
			/* Octant #4 */
			GE_status = LT24_drawPixel(colour, x - current_x, y + current_y);	// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.
			
			/* Octant #5 */
			GE_status = LT24_drawPixel(colour, x - current_x, y - current_y);	// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.
			
			/* Octant #6 */
			GE_status = LT24_drawPixel(colour, x - current_y, y - current_x);	// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.
			
			/* Octant #7 */
			GE_status = LT24_drawPixel(colour, x + current_y, y - current_x);	// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.
			
			/* Octant #8 */
			GE_status = LT24_drawPixel(colour, x + current_x, y - current_y);	// Draw single pixel.
			if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.

			if (increment_error <= 0)											// Check whether the error is less than 0.
			{
				current_y++;													// Increment y-points.
				increment_error += delta_y;										// Recalculate the error.
				delta_y += 2;													// Recalculate delta y.
			}
			
			if (increment_error > 0)											// Check whether the error is less than 0.	
			{
				current_x--;													// Decrement x-points.
				delta_x += 2;													// Recalculate delta x.
				increment_error += delta_x - (r << 1);							// Recalculate the error.
			}
		}
	}
	
	return GE_SUCCESS;
}

/* Function to draw a triangle with three vertices: (x1,y1), (x2,y2) and (x3,y3) with a boundary colour. 
   The function has a boolean argument that decides whether the triangle should be filled or not with with a particular colour mentioned.*/
signed int Graphics_drawTriangle( unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int x3, unsigned int y3, unsigned short colour, bool noFill, unsigned short fillColour )
{
	/* Declaring the local variables required for drawing a line */
	signed int GE_status = 0;												// Status variable to check for errors.
	unsigned int current_x;													// The x - point from where the pixel has to be drawn.
	unsigned int current_y;													// The y - point from where the pixel has to be drawn.	
	
	unsigned int boundary_min_x;											// The minimum x value of the triangle.
	unsigned int boundary_max_x;											// The maximum x value of the triangle.
	unsigned int boundary_min_y;											// The minimum y value of the triangle.
	unsigned int boundary_max_y;											// The maximum y value of the triangle.
	
	
	if ( noFill == false )													// Check whether the triangle should be filled or not.
	{		
		boundary_min_x = MIN ( x1, MIN ( x2, x3 ) );						// Find the minimum x value of the triangle.
		boundary_max_x = MAX ( x1, MAX ( x2, x3 ) );						// Find the maximum x value of the triangle.		
		
		boundary_min_y = MIN ( y1, MIN ( y2, y3 ) );						// Find the minimum y value of the triangle.
		boundary_max_y = MAX ( y1, MAX ( y2, y3 ) );						// Find the maximum y value of the triangle.
		
		for ( current_x = boundary_max_x ; current_x >= boundary_min_x ; current_x -- )						// Iterate through all the pixels in the x-direction.
		{	
			for ( current_y  = boundary_max_y ; current_y >= boundary_min_y ; current_y -- )				// Iterate through all the pixels in the y-direction.
			{
				if ( Graphics_Engine_Point_Test( current_x, current_y, x1, y1, x2, y2, x3, y3 )  == true )	// Check if the pixel lies within the triangle
				{	
					GE_status = LT24_drawPixel( fillColour, current_x, current_y);							// Draw single pixel.
					if ( GE_status != GE_SUCCESS) return GE_status;											// If there is an error, report it.
				}
			}
		}
		
		GE_status = Graphics_drawLine(x1,y1,x2,y2,colour);					// Draw a line from (x1,y1) to (x2,y2)
		if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.
		
		GE_status = Graphics_drawLine(x1,y1,x3,y3,colour);					// Draw a line from (x1,y1) to (x3,y3)
		if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.
		
		GE_status = Graphics_drawLine(x2,y2,x3,y3,colour);					// Draw a line from (x2,y2) to (x3,y3)
		if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.
	}
	
	else 
	{	
		GE_status = Graphics_drawLine(x1,y1,x2,y2,colour);					// Draw a line from (x1,y1) to (x2,y2)
		if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.
		
		GE_status = Graphics_drawLine(x1,y1,x3,y3,colour);					// Draw a line from (x1,y1) to (x3,y3)
		if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.
		
		GE_status = Graphics_drawLine(x2,y2,x3,y3,colour);					// Draw a line from (x2,y2) to (x3,y3)
		if ( GE_status != GE_SUCCESS) return GE_status;						// If there is an error, report it.
	}
	
	return GE_SUCCESS;
}


/* Function to swap axes i.e., swap x and y points */
unsigned int* Graphics_Engine_Swap_Vertices ( unsigned int point_x, unsigned int point_y )
{
	unsigned int temp_value;												// Variable to hold value of x.
	unsigned int swap_array[2];												// Array to hold the swapped values.
	
	temp_value = point_x;													// Assign current value of x to the temporary variable.
	point_x = point_y;														// Re-assign x with value of y.
	point_y = temp_value;													// Re-assign y with previous value of x.
	
	swap_array[0] = point_x;												// Place new value of x in the array.
	swap_array[1] = point_y;												// Place new value of y in the array.
	
	return swap_array;														// Return the swapped points.
}

/* Graphic function to determine whether a coordinate lies inside the triangle to fill triangle.*/
bool Graphics_Engine_Point_Test ( unsigned int point_x, unsigned int point_y, unsigned int x1, unsigned int y1, unsigned int x2, unsigned int y2, unsigned int x3, unsigned int y3)
{
	signed int check_side_1 = (y2 - y1)*(point_x - x1) + (-x2 + x1)*(point_y - y1);		// Find the cross-product of side 1.
	signed int check_side_2 = (y3 - y2)*(point_x - x2) + (-x3 + x2)*(point_y - y2);		// Find the cross-product of side 2.
	signed int check_side_3 = (y1 - y3)*(point_x - x3) + (-x1 + x3)*(point_y - y3);		// Find the cross-product of side 3.
	 
	if ( ( check_side_1 >=0 && check_side_2 >=0 ) && check_side_3 >=0 ) 				// Check if the pixel lies within the three lines.
	{
		return  true;																	// Return true if the pixel is within the three lines.
	}
	else 
	{	
		return false;																	// Return false if the pixel is not within the three lines.
	}
}

signed int Graphics_drawLetter(unsigned int x1, unsigned int y1, unsigned int x2,  unsigned int y2, unsigned int letter, unsigned short colour)
{

	//Letter S
	if(letter == 1)
	{
		Graphics_drawLine(x1,y1,x1,y2,colour);
		Graphics_drawLine(x1,y2,((x1+x2)/2),y2,colour);
		Graphics_drawLine(((x1+x2)/2),y1,((x1+x2)/2),y2,colour);
		Graphics_drawLine(((x1+x2)/2),y1,x2,y1,colour);
		Graphics_drawLine(x2,y1,x2,y2,colour);
	}
	//Letter T
	
	else if(letter == 2)
	{
		Graphics_drawLine(x1,y1,x1,y2,colour);
		Graphics_drawLine(x1,(y1+y2)/2,x2,(y1+y2)/2,colour);
	}
	
	//Letter A
	else if(letter == 3)
	{
		Graphics_drawLine(x1,y1,x2,y1,colour);
		Graphics_drawLine(((x1+x2)/2),y1,((x1+x2)/2),y2,colour);
		Graphics_drawLine(x1,y2,x2,y2,colour);
		Graphics_drawLine(x1,y1,x1,y2,colour);
	}
	
	//Letter R
	else if(letter == 4)
	{
		Graphics_drawLine(x1,y2,x2,y2,colour);
		Graphics_drawLine(x1,y1,((x1+x2)/2),y1,colour);
		Graphics_drawLine(((x1+x2)/2),y1,((x1+x2)/2),y2,colour);
		Graphics_drawLine(x1,y1,x1,y2,colour);
		Graphics_drawLine((x1+x2)/2,y2,x2,y1,colour);

	}
	
	//Letter O
	else if(letter == 5)
	{
		Graphics_drawLine(x1,y1,x2,y1,colour);
		Graphics_drawLine(x2,y1,x2,y2,colour);
		Graphics_drawLine(x2,y2,x1,y2,colour);
		Graphics_drawLine(x1,y2,x1,y1,colour);
	}
	
	//Letter P
	else if(letter == 6)
	{
		Graphics_drawLine(x1,y2,x2,y2,colour);
		Graphics_drawLine(x1,y1,((x1+x2)/2),y1,colour);
		Graphics_drawLine(((x1+x2)/2),y1,((x1+x2)/2),y2,colour);
		Graphics_drawLine(x1,y1,x1,y2,colour);

	}
	
	//Letter W
	else if(letter == 7)
	{
		Graphics_drawLine(x1,y1,x2,y1,colour);
		Graphics_drawLine(x1,y2,x2,y2,colour);
		Graphics_drawLine(x2,y2,(x1+x2)/2,(y1+y2)/2,colour);
		Graphics_drawLine(x2,y1,(x1+x2)/2,(y1+y2)/2,colour);
	}

	//Letter I
	else if(letter == 8)
	{
		Graphics_drawLine(x1,y1,x1,y2,colour);
		Graphics_drawLine(x1,(y1+y2)/2,x2,(y1+y2)/2,colour);
		Graphics_drawLine(x2,y1,x2,y2,colour);
	}

	//Letter N
	else if(letter == 9)
	{
		Graphics_drawLine(x1,y1,x2,y1,colour);
		Graphics_drawLine(x1,y2,x2,y2,colour);
		Graphics_drawLine(x1,y2,x2,y1,colour);
	}

		//Letter E
	else if(letter == 10)
	{

		Graphics_drawLine(((x1+x2)/2),y1,((x1+x2)/2),y2,colour);
		Graphics_drawLine(x1,y2,x2,y2,colour);
		Graphics_drawLine(x1,y1,x1,y2,colour);
		Graphics_drawLine(x2,y1,x2,y2,colour);
	}
	
	else if(letter == 11)
	{
		Graphics_drawLine(x1,y1,x2,y1,colour);
	}
	
	else if(letter == 12)
	{
		Graphics_drawLine(x1,y1,x1,y2,colour);
		Graphics_drawLine(x1,y1,((x1+x2)/2),y1,colour);
		Graphics_drawLine(((x1+x2)/2),y1,((x1+x2)/2),y2,colour);
		Graphics_drawLine(((x1+x2)/2),y2,x2,y2,colour);
		Graphics_drawLine(x2,y1,x2,y2,colour);
	}
	
	return 0;

}
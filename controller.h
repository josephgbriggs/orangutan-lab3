#ifndef __CONTROLLER_H
#define __CONTROLLER_H

#include <inttypes.h>
#include <stdio.h>

// Global Gains
volatile uint16_t G_Kp; // Proportial gain
volatile uint16_t G_Kd; // Derivative gain

/*
 * Set up the sampling rate in Hz for the calculations in the controller.
 * Valid values for the parameter hz are {1000, 100, 50, 10}
 * If one of those values not selected, it defaults to 1000 Hz
 */
void init_controller_w_rate(uint16_t hz);

/*
 * Set the speed in 'counts' per second
 * Maximum speed range is [-255, 255]
 */
void set_speed(int16_t);

/*
 * Set position of the wheel. The inital position at program start is '0'.
 * Positions increment clockwise around the wheel in the range 0..127
 */
void set_position(uint8_t);

/*
 * Rotate the wheel clockwise a certain number of 'counts' from the 
 * current position. There are 128 counts in a complete rotation.
 */
void rotate_cw(uint16_t cnt);

/*
 * Rotate the wheel clockwise a certain number of 'counts' from the 
 * current position. There are 128 counts in a complete rotation.
 */
void rotate_ccw(uint16_t cnt);

/*
 * fill the buffer with the current values
 */
int buffer_controller_values(char* buffer);

#endif //__CONTROLLER_H

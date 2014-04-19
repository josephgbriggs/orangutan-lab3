#ifndef __CONTROLLER_H
#define __CONTROLLER_H

#include <inttypes.h>
// extern


typedef struct {
	uint16_t last_position;
	
	uint16_t Kp; // Proportial gain
	uint16_t Kd; // Derivative gain
	
} control_state_s;

/*
 * Set up the sampling rate for the calculations in the controller
 * Valid values for Hz are { 1000, 100, 50, 10}
 * If one of those values not selected, it defaults to 1000 Hz
 */
void init_clock(uint16_t hz);

/*
 * Use Proportial and Derivative closed-loop control to acheive the desired setting
 */
int16_t calculate_pd(control_state_s *state, int16_t target_p, int16_t current_p);

/*
 * Use Proportial and Derivative closed-loop control to acheive the desired setting
 */
int16_t calculate_pd_w_rate(control_state_s *state, int16_t target_p, int16_t current_p, int16_t current_rate);


void rotate_cw(uint16_t cnt);

void rotate_ccw(uint16_t cnt);


#endif //__CONTROLLER_H

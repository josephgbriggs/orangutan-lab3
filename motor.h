/*
 * Portions based on: OrangutanMotors.h
 * Written by Ben Schmidel, May 15, 2008.
 * Last modified: September 29, 2008
 * Copyright (c) 2008-2012 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18
 *
 * You may freely modify and share this code, as long as you keep this
 * notice intact (including the two links above).  Licensed under the
 * Creative Commons BY-SA 3.0 license:
 *
 *   http://creativecommons.org/licenses/by-sa/3.0/
 *
 * Disclaimer: To the extent permitted by law, Pololu provides this work
 * without any warranty.  It might be defective, in which case you agree
 * to be responsible for all resulting costs and damages.
 */
/*
 * Portions based on: PololuWheelEncoders.h
 * Copyright (c) 2009-2012 Pololu Corporation. For more information, see
 *
 *   http://www.pololu.com
 *   http://forum.pololu.com
 *   http://www.pololu.com/docs/0J18
 *
 * You may freely modify and share this code, as long as you keep this
 * notice intact (including the two links above).  Licensed under the
 * Creative Commons BY-SA 3.0 license:
 *
 *   http://creativecommons.org/licenses/by-sa/3.0/
 *
 * Disclaimer: To the extent permitted by law, Pololu provides this work
 * without any warranty.  It might be defective, in which case you agree
 * to be responsible for all resulting costs and damages.
 */
#ifndef __MOTOR_H
#define __MOTOR_H

#include <inttypes.h>

// external globals
extern volatile uint16_t G_enc_count;
extern volatile uint8_t G_enc_position;
extern volatile uint8_t G_encoder_event;
extern volatile uint8_t G_mot_sig_torque;
extern volatile uint8_t G_mot_sig_dir;

/*
 * Initializes the clocks and other PWM settings for the motor.
 */
void init_motor();

/*
 * Initializes the wheel encoders. 
 * 
 * init_encoder() may be called multiple times.
 */
void init_encoder();

/*
 * Set the rotational speed of the motor
 * Input is limited between -255 and 255
 */
void set_torque(int16_t torque);

/* 
 * Returns the position of the wheel as a value from 0 to 127
 */
uint8_t get_position();

#endif //__MOTOR_H
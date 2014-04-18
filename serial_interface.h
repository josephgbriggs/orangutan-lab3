/*******************************************
*
* Header file for menu stuff.
*
*******************************************/
#ifndef __SERIAL_INTERFACE_H
#define __SERIAL_INTERFACE_H

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <pololu/orangutan.h>  


// extern GLOBALS
extern volatile uint8_t G_logging_flag;

/* This is a customization of the serial2 example from the Pololu library examples. (ACL)
 *
 * serial2: for the Orangutan SVP and Orangutan X2 controllers.
 *
 * This example listens for serial bytes transmitted via USB to the controller's
 * virtual COM port (USB_COMM).  Whenever it receives a byte, it performs a
 * custom action.  
 *
 * This example will not work on the Orangutan LV, SV, Baby Orangutan, or 3pi robot.
 * 
 * http://www.pololu.com/docs/0J20 
 * http://www.pololu.com  
 * http://forum.pololu.com  
 */   

#define PROMPT ">"
#define PROMPT_LENGTH 1


// wait_for_sending_to_finish:  Waits for the bytes in the send buffer to
// finish transmitting on USB_COMM.  We must call this before modifying
// send_buffer or trying to send more bytes, because otherwise we could
// corrupt an existing transmission.
void wait_for_sending_to_finish();

// If there are received bytes to process, this function loops through the receive_buffer
// accumulating new bytes (keystrokes) in another buffer for processing.
void check_for_new_bytes_received();

// This initializes serial communication through the USB.
void init_interface();

// A generic function for whenever you want to print to your serial comm window.
// Provide a string and the length of that string. My serial comm likes "\r\n" at 
// the end of each string (be sure to include in length) for proper linefeed.
void print_usb(char*,int);

// Display command options to the user
void print_usage();
	
#endif //__SERIAL_INTERFACE_H

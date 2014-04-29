#include <pololu/orangutan.h>
#include <inttypes.h> //gives us uintX_t

#include "motor.h"
#include "controller.h"
#include "serial_interface.h"

// Globals
volatile uint8_t G_reset_source = 0;
volatile uint8_t G_logging_flag = 0;

/******************************************************************************/
/* Print Reset Source                                                         */
/*                                                                            */
/* This dumps strings over the serial port to indicate why we recently        */
/* reset.                                                                     */
/******************************************************************************/
void print_reset_reason() {
   if (G_reset_source & (1<<JTRF))	{ print_usb("JTAG Reset\r\n", 12); }
   if (G_reset_source & (1<<WDRF))	{ print_usb("Watchdog Reset\r\n", 16); }
   if (G_reset_source & (1<<BORF))	{ print_usb("Brownout Reset\r\n", 16); }
   if (G_reset_source & (1<<EXTRF))	{ print_usb("External Reset\r\n", 16); }
   if (G_reset_source & (1<<PORF))	{ print_usb("Power-on Reset\r\n", 16); }
}

 
int main() {
	
	// capture any reset reason 
	G_reset_source = MCUSR;
	// clear the status register by writing ones 
	// (odd, I know, but that is how it is done)
	MCUSR = 0x1F;
	
	// Display on the LCD that serial connection is needed
	print("Waiting for");
	lcd_goto_xy(0, 1);
	print(" serial conn...");
	
	// This init will block if no serial connection present
	// so user sees message on LCD to make a connection
	init_interface();

	// Display the user interface over the serial usb
	// connection
	serial_check();
	print_reset_reason();
	
	print_usb("Welcome to lab 3!\r\n", 19);
	print_usage();
	print_prompt();
	
	// clear "Waiting for connection" message from the LCD
	clear();
	
	// turn on interrupts
	sei();
	
	init_motor();
	init_encoder();
	// set controller for 1000 Hz
	init_controller_w_rate(50);
	
	
	while (1) {
		serial_check();
		check_for_new_bytes_received();
	}
	
	return 0;
}
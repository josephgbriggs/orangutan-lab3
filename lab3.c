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
	print_usb(PROMPT, PROMPT_LENGTH);
	
	// clear "waiting for connection" message from the LCD
	clear();
	sei();
	
	// Used to print to serial comm window
	char tempBuffer[32];
	int length = 0;
	
	// set controller for 1000 Hz
	init_controller_rate(1000);
	init_motor();
	init_encoder();
	
	while (1) {
		serial_check();
		check_for_new_bytes_received();
		// if (G_logging_flag) {
		// 	length = sprintf(tempBuffer, "encoder cnt: %u  ", G_enc_count);
		// 	print_usb(tempBuffer, length);
		// 	length = sprintf(tempBuffer, "position: %u\r\n", G_wheel_position);
		// 	print_usb(tempBuffer, length);
		// 	length = buffer_controller_values(tempBuffer);
		// 	print_usb(tempBuffer, length);
		// }
	}
	
	return 0;
}
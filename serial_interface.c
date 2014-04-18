#include "serial_interface.h"
#include "motor.h"


// local global data structures
static char receive_buffer[32];
static unsigned char receive_buffer_position;
static char send_buffer[32];

// A generic function for whenever you want to print to your serial comm window.
// Provide a string and the length of that string. My serial comm likes "\r\n" at 
// the end of each string (be sure to include in length) for proper linefeed.
void print_usb( char *buffer, int n ) {
	serial_send( USB_COMM, buffer, n );
	wait_for_sending_to_finish();
}	
	
void print_usage() {
	print_usb("usage:    <command>[arg]\r\n", 26);
	print_usb("command: \r\n", 11);
	print_usb("  l       Toggle Logging\r\n", 26);
	print_usb("  v       View current values\r\n", 31);
	print_usb("  r<arg>  Set position [0,127]\r\n", 32);
	print_usb("  s<arg>  Set speed [-255,255]\r\n", 32);
	print_usb("  P       Increase Kp\r\n", 23);
	print_usb("  p       Decrease Kp\r\n", 23);
	print_usb("  D       Increase Kd\r\n", 23);
	print_usb("  d       Decrease Kd\r\n", 23);
	print_usb("  t<arg>  Execute trajectory\r\n", 30);
	print_usb("  h       Print this menu\r\n\r\n", 29);
}
	
//------------------------------------------------------------------------------------------
// Initialize serial communication through USB and print menu options
// This immediately readies the board for serial comm
void init_interface() {
	
	// Set the baud rate to 9600 bits per second.  Each byte takes ten bit
	// times, so you can get at most 960 bytes per second at this speed.
	serial_set_baud_rate(USB_COMM, 9600);

	// Start receiving bytes in the ring buffer.
	serial_receive_ring(USB_COMM, receive_buffer, sizeof(receive_buffer));

	print_usb( "USB Serial Initialized\r\n", 24);
}

//------------------------------------------------------------------------------------------
// process_received_byte: Parses a menu command (series of keystrokes) that 
// has been received on USB_COMM and processes it accordingly.
// The menu command is buffered in check_for_new_bytes_received (which calls this function).
void process_received_string(const char* buffer)
{
	// Used to pass to USB_COMM for serial communication
	int length;
	char tempBuffer[32];
	
	// parse and echo back to serial comm window (and optionally the LCD)
	char op_char;
	int value;
	int parsed;
	parsed = sscanf(buffer, "%c %d", &op_char, &value);
#ifdef ECHO2LCD
	lcd_goto_xy(0,0);
	printf("Got %c %d\n", op_char, color, value);
#endif
	length = sprintf( tempBuffer, "\r\nOp:%c V:%d\r\n", op_char, value );
	print_usb( tempBuffer, length );

	// Check valid command and execute
	switch (op_char) {
		// print help
		case 'h':
		case 'H':
			print_usage();
			break; 
		// Start/Stop Logging (print) the values of Pr, Pm, and T.
		case 'L':
		case 'l':
			G_logging_flag = (G_logging_flag == 0) ? 1 : 0;
			break;
		// View the current values Kd, Kp, Vm, Pr, Pm, and T
		case 'V':
		case 'v':
			break;
		// Set the reference position (use unit "counts")
		case 'R':
		case 'r':
			break;
		// Set the reference speed (use unit "counts"/sec)
		case 'S':
		case 's':
			set_speed(value);
			break;
		// Increase Kp
		case 'P':
			break;
		// Decrease Kp
		case 'p':
			break;
		// Increase Kd
		case 'D':
			break;
		// Decrease Kd
		case 'd':
			break;
		case 'T':
		case 't':
			break;
		default:
			print_usb( "Command does not compute.\r\n", 27 );
		} // end switch(op_char) 
		
	print_usb( PROMPT, PROMPT_LENGTH);

} //end menu()

//---------------------------------------------------------------------------------------
// If there are received bytes to process, this function loops through the receive_buffer
// accumulating new bytes (keystrokes) in another buffer for processing.
void check_for_new_bytes_received()
{
	/* 
	The receive_buffer is a ring buffer. The call to serial_check() (you should call prior to this function) fills the buffer.
	serial_get_received_bytes is an array index that marks where in the buffer the most current received character resides. 
	receive_buffer_position is an array index that marks where in the buffer the most current PROCESSED character resides. 
	Both of these are incremented % (size-of-buffer) to move through the buffer, and once the end is reached, to start back at the beginning.
	This process and data structures are from the Pololu library. See examples/serial2/test.c and src/OrangutanSerial/ *.*
	
	A carriage return from your comm window initiates the transfer of your keystrokes.
	All key strokes prior to the carriage return will be processed with a single call to this function (with multiple passes through this loop).
	On the next function call, the carriage return is processes with a single pass through the loop.
	The menuBuffer is used to hold all keystrokes prior to the carriage return. The "received" variable, which indexes menuBuffer, is reset to 0
	after each carriage return.
	*/ 
	char menuBuffer[32];
	static int received = 0;
	
	// while there are unprocessed keystrokes in the receive_buffer, grab them and buffer
	// them into the menuBuffer
	while(serial_get_received_bytes(USB_COMM) != receive_buffer_position)
	{
		// place in a buffer for processing
		menuBuffer[received] = receive_buffer[receive_buffer_position];
		
		// echo back each byte sent by the user
		if (menuBuffer[received] == '\r' || menuBuffer[received] == '\n') {
			// don't echo back newlines
		}
		else {
			send_buffer[0] = menuBuffer[received];
			print_usb(send_buffer, 1);
		}
		
		++received;
		
		// Increment receive_buffer_position, but wrap around when it gets to
		// the end of the buffer. 
		if ( receive_buffer_position == sizeof(receive_buffer) - 1 )
		{
			receive_buffer_position = 0;
		}			
		else
		{
			receive_buffer_position++;
		}
	}
	// If there were keystrokes processed, check if a menu command
	if (received) {
		
		char* loc;
		int idx;
		uint8_t commandComplete = 0;

		// null terminate the string in the menuBuffer if newline found
        if ((loc = strchr(menuBuffer, '\r')) != NULL) {

			// address difference to find our array index
			idx = loc - menuBuffer;
            menuBuffer[idx] = '\0';
			if (idx < 32) {
				++idx;
				menuBuffer[idx] = '\0'; // assume '\r' is followed by '\n'
			} 
			commandComplete = 1;
			
        }
		else if ((loc = strchr(menuBuffer, '\n')) != NULL) {

			// address difference to find our array index
			idx = loc - menuBuffer;
            menuBuffer[idx] = '\0';
			commandComplete = 1;
        }
		
		// the user pressed 'return' so process the command
		if (commandComplete) {
			commandComplete = 0;

			process_received_string(menuBuffer);
			
#ifdef ECHO2LCD
			lcd_goto_xy(0,1);
			print("RX: (");
			print_long(received);
			print_character(')');
			for (int i=0; i<received; i++)
			{
				print_character(menuBuffer[i]);
			}
#endif
			received = 0;
		}
	}
}
	
//-------------------------------------------------------------------------------------------
// wait_for_sending_to_finish:  Waits for the bytes in the send buffer to
// finish transmitting on USB_COMM.  We must call this before modifying
// send_buffer or trying to send more bytes, because otherwise we could
// corrupt an existing transmission.
void wait_for_sending_to_finish()
{
	while(!serial_send_buffer_empty(USB_COMM))
		serial_check();		// USB_COMM port is always in SERIAL_CHECK mode
}


#include "serial_interface.c"

int main() {
	
	init_interface();
	
	while (1) {
		serial_check();
		check_for_new_bytes_received();
	}
	
	return 0;
}
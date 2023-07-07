// gcc scheme.c example-repl-simple.c -o example-repl-simple.out

#include "scheme.h"
#include "scheme-private.h"
#include <libdragon.h>
#include "usb.h"

#define MAX_BUFFER_SIZE 1024
int incoming_size = 0;
char incoming_type = 0;

void read_line( char *buf){
	 while (usb_poll() != 0){
		uint32_t header = usb_poll();
		
		// Store the size from the header
		incoming_size = USBHEADER_GETSIZE(header);
		incoming_type = USBHEADER_GETTYPE(header);
		
		// If the amount of data is larger than our echo buffer
		if (incoming_size > 1024)
		{
			// Purge the USB data
			usb_purge();
			
			// Write an error message to buffer instead
			sprintf(buf, "Incoming data larger than echo buffer!\n");
			incoming_size = strlen(buf)+1;
			incoming_type = DATATYPE_TEXT;
			
			// Restart the while loop to check if there's more USB data
			continue;
		}
		
		// Read the data from the USB into our echo buffer
		usb_read(buf, incoming_size);
	}
	

	int i = 0;
	while (i < incoming_size - 1) {
		char c = buf[i];
		
		if (c == '\n') {
			break;
		}
		buf[i++] = c;
	}
	buf[i] = '\0';
}

int main( int argc, char **argv ){
	scheme *sc;
	pointer write;
	pointer args;

	if (!(sc = scheme_init_new())) {
		return 2;
	}

	console_init();
	console_set_render_mode(RENDER_MANUAL);
	console_clear();
	usb_initialize();
	printf("\n\nTinyScheme 64\n");
	usb_write(DATATYPE_TEXT, "\n\nTinyScheme 64\n ", 17);

	scheme_set_input_port_file(sc, stdin);
	scheme_set_output_port_file(sc, stdout);

	write = scheme_eval(sc, mk_symbol(sc, "write"));

	console_render();
	while (1) {
		char buf[MAX_BUFFER_SIZE];
		read_line( buf);
		if (incoming_size > 0){
			printf("> ");
			printf(buf);
			printf("\n");
			scheme_load_string(sc, buf);
			args = cons(sc, sc->value, sc->NIL);
			scheme_call(sc, write, args);
			printf("\n\n");
			console_render();

			// Clear everything
			incoming_type = 0;
			incoming_size = 0;
            memset(buf, 0, 1024);
		}
	}
	scheme_deinit(sc);

	return 0;
}
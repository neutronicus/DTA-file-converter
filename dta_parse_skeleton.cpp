#include "dta_parse_skeleton.h"

int parse_dta_file (message_handler* handlers, void** additional_data, FILE* dta_file) {
  unsigned short length;
  byte length_msb, length_lsb, id;
  void* message_contents;
  size_t bytes_read;

  while (! feof(dta_file) ) {
	fread((void *) &length_lsb, 1, 1, dta_file);
	fread((void *) &length_msb, 1, 1, dta_file);
	fread((void *) &id, 1, 1, dta_file);

	length = ( (short) length_msb << 8 | (short) length_lsb);

	message_contents = malloc(length - 1);

	bytes_read = (length - 1) * fread(message_contents, length - 1, 1, dta_file);
	if (bytes_read != length - 1) {
	  if ( feof(dta_file) ) {
		return -1;
	  } else if ( ferror(dta_file) ) {
		printf("Error reading from dta file. \n");
	  } else {
		printf("Derp.\n");
	  }
	}

	if (handlers[id]) {
	  handlers[id](message_contents, length - 1, additional_data[id]);
	}
	free(message_contents);
  }
}

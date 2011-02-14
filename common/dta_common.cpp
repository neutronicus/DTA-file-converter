#include "common.h"

/*********************************************************************************
 * This array contains the length of each AE characteristic.
 *
 * Alone among the AE characteristics, the length of characteristic 22 varies at
 * runtime, based on the parsing of message 109, so that element of this array
 * must be set at runtime.
 *********************************************************************************/
byte chid_to_length [] =
  {NULL,
   2,
   2,
   2,
   2,
   4, //5
   1,
   1,
   1,
   1,
   1, // 10
   1,
   4,
   2,
   2,
   6, // 15
   0, // Not quite sure what's going on here
   2,
   2,
   2,
   4, // 20
   4,
   0, // Rember to set this at runtime - id 22
   2,
   2};

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

void message8_handler_common (void* data, int length, void* additional_data) {
  FILE* f = (FILE *)additional_data;

  long int pos = ftell (f);
  // Was this message read from the beginning of the file?
  if (pos == (length + 3)) {
	// If so, it contains many other messages.
	// Move the file pointer back to where the other messages start.
	fseek (f, -length + 8,SEEK_CUR);
  }
  // Otherwise, do nothing.
}

void message42_handler_common (void* data, int length, void* additional_data) {
  FILE* f = (FILE *) additional_data;

  // After the extra ID byte and the version bytes, there are just
  // more messages, so might as well put the file pointer there and
  // let them parse normally.
  fseek (f, -length + 3, SEEK_CUR);
}

#include "dta_to_json.h"
/*********************************************************************************
 * This is an array of function pointers - the targets of the pointers satisfy
 * the following description:
 *
 * Arguments:
 * 1. A void* pointing to the contents of the message most recently read from the
 *    the data file.  This is what will be parsed.
 * 2. An int recording the length of the data.
 * 3. A void* pointing to arbitrary data that the function may need to do its job.
 *    Basically, this parameter is used to allow message handlers to access earlier
 *    data which may be relevant, modify data which may be necessary for a later
 *    handler, and to pass in file handles and whatever other data a handler may
 *    need to generate output.
 *
 * Note: If the message requires no parsing, the corresponding index in this array
 * will contain a NULL pointer.
 *********************************************************************************/
message_handler json_handlers [256];

/*********************************************************************************
 * This is an array of things to be passed as the third arguments to each of the
 * JSON handlers.
 *********************************************************************************/
void * json_ctx [256];

/*********************************************************************************
 * This array contains a string corresponding to each AE characteristic ID.
 * Together with the function pointers in chid_handlers, these are used to generate
 * JSON for each AE characteristic.
 *********************************************************************************/
char characteristic_names[][25] = {"Nothing",
								   "Rise Time",
								   "Counts to Peak",
								   "Total Counts",
								   "Energy Counts",
								   "Duration (microsec)",
								   "Amplitude",
								   "RMS Voltage",
								   "ASL (dB)",
								   "Gain",
								   "Threshold",
								   "Pre-amp Current",
								   "Lost Hits",
								   "Average Frequency",
								   "Unknown",
								   "TOT",
								   "Unknown",
								   "RMS Voltage",
								   "Reverberation Frequency",
								   "Initiation Frequency",
								   "Signal Strength",
								   "Absolute Energy",
								   "Partial Power",
								   "Frequency Centroid",
								   "Frequency Peak"};

/*************************************************************************************************************************
 * This array contains pointers to functions meeting the following description:
 * Arguments:
 *   1. A handle to a YAJL yajl_gen, used for generating JSON.
 *   2. A pointer into the block of data in the message devoted to AE characteristics.
 *   3. A pointer to arbitrary data that some of these functions may need.
 *      Specifically, skip_partial_power requires a pointer to an integer enumerating
 *      how many partial power segments have been defined.  The others do not require
 *      any additional data and can simply be passed NULL.
 *
 * Each function adds to the YAJL generator's internal buffer (which will be flushed
 * at the end of message processing) and advances the data pointer to the start of the
 * next AE characteristic stored in the data block.
 *
 * Each index in the array corresponds to a characteristic ID, and some characteristic
 * IDs require identical processing, so many of the indices point to the same functions.
 *************************************************************************************************************************/
characteristic_handler_json chid_handlers [] =
  {NULL, // No characteristic of ID zero
   &ushort_to_json, // first four are all unsigned shorts
   &ushort_to_json,
   &ushort_to_json,
   &ushort_to_json,
   &uint_to_json,
   &byte_to_json,
   &byte_to_json,
   &byte_to_json,
   &byte_to_json,
   &byte_to_json,
   &byte_to_json,
   &uint_to_json,
   &ushort_to_json,
   &ushort_to_json,
   &tot_to_json,
   NULL, // For now ,anyways - length varies based on "message 12," marked only "reserved" in docs, hopefully we don't see one of these
   &rms16_to_json,
   &ushort_to_json,
   &ushort_to_json,
   &sig_strength_to_json,
   &abs_energy_to_json,
   &skip_partial_power,
   &ushort_to_json,
   &ushort_to_json };

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

void json_handlers_init () {
  memset (json_handlers, NULL, sizeof (json_handlers));
  json_handlers [1] = &message1_handler_json;
  json_handlers [2] = &message2_handler_json;
  json_handlers [5] = &message5_handler_json;
  json_handlers [6] = &message6_handler_json;
  json_handlers [24] = &message24_handler_json;
  json_handlers [26] = &message26_handler_json;
  json_handlers [42] = &message42_handler_json;
  json_handlers [109] = &message109_handler_json;
  json_handlers [110] = &message110_handler_json;
  json_handlers [173] = &message173_handler_json;
}

void json_ctx_init () { memset (json_handlers, NULL, sizeof (json_handlers)); }

/*************************************************************************************************************************
 * These functions are responsible for turning various sorts of AE characteristic
 * data into JSON.
 *************************************************************************************************************************/
void sig_strength_to_json (yajl_gen g, void* &data, void* ctx) {
  yajl_gen_double (g, (float)(*(unsigned int *)(data)) * 3.05f);
  data = (byte *) data + 4;
}

void tot_to_json (yajl_gen g, void* &data, void* ctx) {
  double v=0.0;

  unsigned short* up = (unsigned short *) data;

  // scale the 6-byte quantity
  for (int i=3; i; )
	v = v * 65536.0 + (double)up[--i];

  yajl_gen_double (g, v / 4e6);
  data = (byte *) data + 6;
}
   
void ushort_to_json (yajl_gen g, void* &data, void* ctx) {
  yajl_gen_integer (g, (long int) *(unsigned short *)data);
  data = (byte *) data + 2;
}

void uint_to_json (yajl_gen g, void* &data, void* ctx) {
  yajl_gen_integer (g, (long int) *(unsigned int *)data);
  data = (byte *) data + 4;
}

void byte_to_json (yajl_gen g, void* &data, void* ctx) {
  yajl_gen_integer (g, (long int) *(byte *) data);
  data = (byte *) data + 1;
}

void abs_energy_to_json (yajl_gen g, void* & data, void* ctx) {
  yajl_gen_integer (g, (double) *(float *) data * 0.000931f);
  data = (byte *) data + 4;
}

void rms16_to_json (yajl_gen g, void* &data, void* ctx) {
  yajl_gen_double (g, (double)(*(unsigned short *) data) / 5000.0f);
  data = (byte *) data + 2;
}

void skip_partial_power (yajl_gen g, void* &data, void* ctx) {
  data = (byte *) data + *(int *) ctx;
}

void message5_handler_json(void* data, int length, void* additional_data) {
  m1_control* m1_state = (m1_control *) additional_data;
  
  m1_state->num_characteristics = *(byte *) data;
  m1_state->characteristics = (byte *) malloc(m1_state->num_characteristics);
  memcpy(m1_state->characteristics, (byte *) data + 1, m1_state->num_characteristics);
  m1_state->num_parametrics = *( (byte *) data + 1 + m1_state->num_characteristics);
}

void message6_handler_json(void* data, int length, void* additional_data) {
  m2_control* m2_state = (m2_control *) additional_data;

  m2_state->num_characteristics = ((byte *) data)[0];
  if (m2_state->num_characteristics > 0) {
	m2_state->characteristics = (byte *) malloc(m2_state->num_characteristics);
	memcpy(m2_state->characteristics, (void*) ((byte *) data + 1), m2_state->num_characteristics);
  } else { m2_state->characteristics = NULL; }
  
  m2_state->num_parametrics = *( (byte *) data + 1 + m2_state->num_characteristics);
  if (m2_state->num_parametrics > 0) {
	m2_state->parametrics = (byte *) malloc(m2_state->num_parametrics);
	memcpy(m2_state->parametrics, (void*) ((byte*) data + 1 + m2_state->num_parametrics + 1), m2_state->num_parametrics);
  } else { m2_state->parametrics = NULL; }
}

void message24_handler_json (void* data, int length, void* additional_data) {
  m173_control* c = (m173_control *) additional_data;

  byte channel_id = *(byte *) data;
  unsigned short hdt = *(unsigned short *) ((byte *) data + 1);

  if (channel_id == 0) {
	for (int i = 1; i < __AE_NUM_CHANNELS + 1; i++) { c->channel_hdt [i] = 2 * hdt; }
  } else { c->channel_hdt [channel_id] = 2 * hdt; }
}

void message26_handler_json (void* data, int length, void* additional_data) {
  m173_control* c = (m173_control *) additional_data;

  byte channel_id = *(byte *) data;
  unsigned short pdt = *(unsigned short *) ((byte *) data + 1);

  if (channel_id == 0) {
	for (int i = 1; i < __AE_NUM_CHANNELS + 1; i++) { c->channel_pdt [i] = 2 * pdt; }
  } else { c->channel_pdt [channel_id] = 2 * pdt; }
}

void message109_handler_json (void* data, int length, void* additional_data) {
  int* num_partial_power_segments = (int*) additional_data;
  *num_partial_power_segments = *(unsigned short *) ((byte *) data + 1); // read a ushort starting at 1 byte offset;
}

void message110_handler_json (void* data, int length, void* additional_data) {
  m110_data* d = ((m1_control *) additional_data)->parametric_info;

  d->num_pids = ((byte *)data) [0];
  d->pids = (byte *) malloc (d->num_pids);
  memcpy (d->pids, (void *) ((byte *) data + 1), d->num_pids);
}

void message42_handler_json (void* data, int length, void* additional_data) {
  FILE* f = (FILE *) additional_data;

  // After the extra ID byte and the version bytes, there are just
  // more messages, so might as well put the file pointer there and
  // let them parse normally.
  fseek (f, -length + 3, SEEK_CUR);
}

void message1_handler_json (void* data, int length, void* additional_data) {
  m1_control* c = (m1_control *) additional_data;
  m110_data* p_info = c->parametric_info;
  yajl_gen g = c->json_handle;

  void* anchor = data;

  yajl_gen_map_open(g);

  yajl_gen_string(g, (unsigned char *) "Message ID", strlen("Message ID"));
  yajl_gen_integer(g, 1);

  yajl_gen_string (g, (unsigned char *) "TOT", strlen ("TOT"));
  tot_to_json (g, data, NULL);

  yajl_gen_string (g, (unsigned char *) "Channel Number", strlen ("Channel Number"));
  yajl_gen_integer (g, ((byte *) data) [6]);

  void* characteristics_ptr = ((byte *) data + 1);

  // See comments for characteristic_names and chid_handlers
  for (int i = 0; i < c->num_characteristics; i++) {
	// Get the right string from the lookup table
	if (c->characteristics [i] != 22) {
	  yajl_gen_string (g,
					   (unsigned char *) characteristic_names [c->characteristics [i]],
					   strlen (characteristic_names [c->characteristics [i]]));
	}

	// Get the right function from the lookup table; call if not null
	if (chid_handlers [c->characteristics [i]])
	  chid_handlers [c->characteristics [i]] (g,
											  characteristics_ptr,
											  c->partial_power_segs_p);
  }

  // Note characteristics_ptr now points to the beginning of the parametric ID's block
  for (int i = 0; i < p_info->num_pids; i++) {
	char buf [15];
	byte p_lsb, p_second_b, p_third_b;
	sprintf (buf, "Parametric %d", p_info->pids [i]);
	yajl_gen_string (g, (unsigned char *) buf, strlen (buf));

	if (p_info->pids [i] > 32) {
	  p_lsb = ((byte *) characteristics_ptr) [0];
	  p_second_b = ((byte *) characteristics_ptr) [1];
	  p_third_b = ((byte *) characteristics_ptr) [2];
	  int cycle_count = 256 * (256 * p_third_b + p_second_b) + p_lsb;
	  
	  yajl_gen_integer (g, cycle_count);
	  
	  characteristics_ptr = ((byte *) characteristics_ptr) + 3;
	} else {
	  p_lsb = ((byte *) characteristics_ptr) [0];
	  p_second_b = ((byte *) characteristics_ptr) [1];
	  int para_val = ((short*) characteristics_ptr) [0];
	  
	  yajl_gen_double (g, (double) para_val * 10.0 / 32768.0);
	  
	  characteristics_ptr = ((byte *) characteristics_ptr) + 2;
	}
  }

  yajl_gen_map_close (g);

  const unsigned char * buf;
  unsigned int len;
  yajl_gen_get_buf (g, &buf, &len);
  fwrite (buf, 1, len, c->output_handle);
  yajl_gen_clear (g);  
}

void store_num_samples (void* data, m173_control* c) {
  // Don't care about most of the data in this message
  // Just jump to the ones I care about.
  byte channel_id = *((byte *) data + 8);
  unsigned short num_samples = 1024 * (unsigned short) *((byte *) data + 9);
  if (channel_id == 0) {
	for (int i = 0; i < sizeof (c->n_samples_per_channel); i++)
	  c->n_samples_per_channel [i] = num_samples;
  } else {
	c->n_samples_per_channel [channel_id] = num_samples;
  }
}

void message173_handler_json (void* data, int length, void* additional_data) {
  m173_control * m173_state = (m173_control *) additional_data;
  yajl_gen g = m173_state->json_handle;

  // Check sub-ID byte
  if (*(byte *)data == 42) {
	store_num_samples (data, m173_state);
	return;
  } else if (*(byte *)data != 1) {
	return;
  }

  data = ((byte *) data) + 1; // Move the pointer

  yajl_gen_map_open (g);

  yajl_gen_string (g, (unsigned char *) "Message ID", strlen ("Message ID"));
  yajl_gen_integer (g, 173);
  
  yajl_gen_string (g, (unsigned char *) "TOT", strlen ("TOT"));
  tot_to_json (g, data, NULL);

  yajl_gen_string (g, (unsigned char *) "Channel Number", strlen ("Channel Number"));
  yajl_gen_integer (g, *((byte *) data));
  byte channel_id = *((byte *) data);

  data = ((byte *) data + 2);

  yajl_gen_string (g, (unsigned char *) "Number of Samples", strlen ("Number of Samples"));
  yajl_gen_integer (g, m173_state->n_samples_per_channel [channel_id]);

  short * waveform_sample = (short *) data;

  yajl_gen_string (g, (unsigned char *) "Samples", strlen ("Samples"));
  yajl_gen_array_open (g);
  for (int i = 0; i < m173_state->n_samples_per_channel [channel_id]; i++)
	yajl_gen_double (g, (double) waveform_sample [i] * 10.0 / 32768.0);
  yajl_gen_array_close (g);

  yajl_gen_map_close (g);

  const unsigned char * buf;
  unsigned int len;
  yajl_gen_get_buf (g, &buf, &len);
  fwrite (buf, 1, len, m173_state->output_handle);
  yajl_gen_clear (g);
}

void message2_handler_json (void* data, int length, void * additional_data) {
  m2_control* c = (m2_control *) additional_data;
  void* data_alias = data;
  yajl_gen g = c->json_handle;

  yajl_gen_map_open (g);

  yajl_gen_string (g, (unsigned char *) "Message ID", strlen ("Message ID"));
  yajl_gen_integer (g, 2);

  yajl_gen_string (g, (unsigned char *) "TOT", strlen ("TOT"));
  tot_to_json (g, data_alias, NULL);

  for (int i = 0; i < c->num_parametrics; i++) {
	char buf [15];
	byte pid = *(byte *) data_alias;
	data_alias = ((byte *) data_alias + 1);

	sprintf (buf, "Parametric %d", pid);

	if (pid > 32) {
	  unsigned int value = ((byte *) data_alias) [0] + 256 * (((byte *) data_alias) [1] + 256 * ((byte *) data_alias) [2]);
	  data_alias = ( (byte *) data_alias + 3);

	  yajl_gen_string (g, (unsigned char *) buf, strlen (buf));
	  yajl_gen_integer (g, value);
	} else {
	  unsigned short value = *((unsigned short *) data_alias);
	  data_alias = ((byte *) data_alias + 2);
	  
	  yajl_gen_string (g, (unsigned char *) buf, strlen (buf));
	  yajl_gen_double (g, (double) value * 10.0 / 32768.0);
	}
  }

  chid_to_length [22] = *c->partial_power_segs_p;
  
  // Compute length in bytes of single batch of AE characteristics
  unsigned short chars_length = 1; // 1 byte for channel ID
  for (int i = 0; i < c->num_characteristics; i++)
	chars_length += chid_to_length [c->characteristics [i]];

  // Compute number of channels
  unsigned short length_remaining = length - (int) ((byte *) data_alias - (byte *) data);
  unsigned short n_channels = length_remaining / chars_length; // These _should_ divide evenly

  for (int i = 0; i < n_channels; i++) {
	char buf [15];

	sprintf (buf, "Channel %d", * (byte *) data_alias);
	data_alias = ((byte *) data_alias + 1);
	
	yajl_gen_string (g, (unsigned char *) buf, strlen (buf));

	if (c->num_characteristics > 0) {
	  yajl_gen_map_open (g);
	  for (int j = 0; j < c->num_characteristics; j++) {
		// Get the right string from the lookup table
		yajl_gen_string (g,
						 (unsigned char *) characteristic_names [c->characteristics [j]],
						 strlen (characteristic_names [c->characteristics [j]]));

		// Get the right function from the lookup table; call if not null
		if (chid_handlers [c->characteristics [j]])
		  chid_handlers [c->characteristics [j]] (g,
												  data_alias,
												  c->partial_power_segs_p);
	  }
	  yajl_gen_map_close (g);
	} else { yajl_gen_string (g, (unsigned char *) "No Data", strlen ("No Data")); }
  }
  yajl_gen_map_close (g);

  const unsigned char * buf;
  unsigned int len;
  yajl_gen_get_buf (g, &buf, &len);
  fwrite (buf, 1, len, c->output_handle);
  yajl_gen_clear (g);
}

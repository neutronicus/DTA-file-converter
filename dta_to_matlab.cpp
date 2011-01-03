#include "dta_to_matlab.h"
#include <sys/stat.h>

characteristic_handler_mx chid_handlers_mx [] =
  {NULL, // No characteristic of ID zero
   &ushort_to_double, // first four are all unsigned shorts
   &ushort_to_double,
   &ushort_to_double,
   &ushort_to_double,
   &uint_to_double,
   &byte_to_double,
   &byte_to_double,
   &byte_to_double,
   &byte_to_double,
   &byte_to_double,
   &byte_to_double,
   &uint_to_double,
   &ushort_to_double,
   &ushort_to_double,
   &tot_to_double,
   NULL, // For now ,anyways - length varies based on "message 12," marked only "reserved" in docs, hopefully we don't see one of these
   &rms16_to_double,
   &ushort_to_double,
   &ushort_to_double,
   &sig_strength_to_double,
   &abs_energy_to_double,
   &skip_partial_power_mx,
   &ushort_to_double,
   &ushort_to_double };

message_handler mx_handlers [256];
void * mx_ctx [256];

char characteristic_names_mx [][25] = {"Nothing",
									   "rise_time",
									   "counts_to_peak",
									   "total_counts",
									   "energy_counts",
									   "duration",			// 5
									   "amplitude",
									   "rms_voltage",
									   "asl",
									   "gain",
									   "threshold",			// 10
									   "pre_amp_current",
									   "lost_hits",
									   "avg_freq",
									   "Unknown",
									   "tot",				// 15
									   "Unknown",
									   "rms_voltage",
									   "reverb_freq",
									   "init_freq",
									   "signal_strength",	// 20
									   "abs_energy",
									   "partial_power",
									   "freq_centroid",
									   "freq_peak"};

void mx_handlers_init () {
  memset (mx_handlers, NULL, sizeof (mx_handlers));
  mx_handlers [1] = &message1_handler_mx;
  mx_handlers [2] = &message2_handler_mx;
  mx_handlers [5] = &message5_handler_mx;
  mx_handlers [6] = &message6_handler_mx;
  mx_handlers [24] = &message24_handler_mx;
  mx_handlers [26] = &message26_handler_mx;
  mx_handlers [42] = &message42_handler_json;
  mx_handlers [109] = &message109_handler_mx;
  mx_handlers [110] = &message110_handler_mx;
  mx_handlers [128] = &message128_handler_mx;
  mx_handlers [173] = &message173_handler_mx;
}

void mx_ctx_init () { memset (mx_ctx, NULL, sizeof (mx_ctx)); }

void message5_handler_mx (void* data, int length, void* additional_data) {
  mx_m1_control* m1_state = (mx_m1_control *) additional_data;
  
  m1_state->num_characteristics = *(byte *) data;
  m1_state->characteristics = (byte *) malloc(m1_state->num_characteristics);
  memcpy(m1_state->characteristics, (byte *) data + 1, m1_state->num_characteristics);
  for (int i = 0; i < m1_state->num_characteristics; i++) { mexPrintf ("chid %d, %d\n", i, m1_state->characteristics [i]); }
  m1_state->num_parametrics = *( (byte *) data + 1 + m1_state->num_characteristics);
}

void message6_handler_mx (void* data, int length, void* additional_data) {
  mx_m2_control* m2_state = (mx_m2_control *) additional_data;
  
  m2_state->num_characteristics = ((byte *) data)[0];
  if (m2_state->num_characteristics > 0) {
	m2_state->characteristics = (byte *) malloc(m2_state->num_characteristics);
	memcpy(m2_state->characteristics, (void*) ((byte *) data + 1), m2_state->num_characteristics);
  } else { m2_state->characteristics = NULL; }
  
  m2_state->num_parametrics = *( (byte *) data + 1 + m2_state->num_characteristics);
  if (m2_state->num_parametrics > 0) {
	m2_state->parametrics = (byte *) malloc(m2_state->num_parametrics);
	memcpy(m2_state->parametrics, (void*) ((byte*) data + 1 + m2_state->num_characteristics + 1), m2_state->num_parametrics);
  } else { m2_state->parametrics = NULL; }
}

void message24_handler_mx (void* data, int length, void* additional_data) {
  mx_m173_control* c = (mx_m173_control *) additional_data;

  byte channel_id = *(byte *) data;
  unsigned short hdt = *(unsigned short *) ((byte *) data + 1);

  if (channel_id == 0) {
	for (int i = 1; i < __AE_NUM_CHANNELS + 1; i++) { c->channel_hdt [i] = 2 * hdt; }
  } else { c->channel_hdt [channel_id] = 2 * hdt; }
}

void message26_handler_mx (void* data, int length, void* additional_data) {
  mx_m173_control* c = (mx_m173_control *) additional_data;

  byte channel_id = *(byte *) data;
  unsigned short pdt = *(unsigned short *) ((byte *) data + 1);

  if (channel_id == 0) {
	for (int i = 1; i < __AE_NUM_CHANNELS + 1; i++) { c->channel_pdt [i] = 2 * pdt; }
  } else { c->channel_pdt [channel_id] = pdt; }
}

void message109_handler_mx (void* data, int length, void* additional_data) {
  int* num_partial_power_segments = (int*) additional_data;
  *num_partial_power_segments = *(unsigned short *) ((byte *) data + 1); // read a ushort starting at 1 byte offset;
}

void message110_handler_mx (void* data, int length, void* additional_data) {
  m110_data* d = ((mx_m1_control *) additional_data)->parametric_info;

  d->num_pids = ((byte *)data) [0];
  if (d->num_pids > 0) {
	d->pids = (byte *) malloc (d->num_pids);
	memcpy (d->pids, (void *) ((byte *) data + 1), d->num_pids);
  } else { d->pids = NULL; }
}

void message42_handler_mx (void* data, int length, void* additional_data) {
  FILE* f = (FILE *) additional_data;

  // After the extra ID byte and the version bytes, there are just
  // more messages, so might as well put the file pointer there and
  // let them parse normally.
  fseek (f, -length + 3, SEEK_CUR);
}

void message128_handler_mx (void* data, int length, void* additional_data) {
  mx_m128_control * c = (mx_m128_control *) additional_data;
  FILE * derp = fopen ("/Users/adamvh/Documents/Programming/dta_tool/working_copy/derp.txt", "w");
  mexPrintf ("UNGA BUNGA\n");

  // Okay, we want to count the message 1's and message 2's, so we know enough
  // to allocate the MATLAB struct.  First save place in the file;
  fpos_t anchor;
  fgetpos ( c->input_file_handle, &anchor );

  unsigned short local_length;
  byte id;

  unsigned int n_hitbased, n_timebased, m2_length;
  n_hitbased = n_timebased = 0;
  fread (&local_length, 2, 1, c->input_file_handle);
  fread (&id, 1, 1, c->input_file_handle);
  while (! feof (c->input_file_handle) && local_length != 0 ) {

	// Don't bother counting 173 - There's one for every 1
	if (id == 1)
	  n_hitbased++;
	else if (id == 2) {
	  n_timebased++;
	  m2_length = local_length;
	} /*else if (! (id == 1 || id == 2 || id == 173) ) return;*/

	fseek (c->input_file_handle, (long int) (local_length - 1), SEEK_CUR);
	fread (&local_length, 2, 1, c->input_file_handle);
	fread (&id, 1, 1, c->input_file_handle);
  }


  // Return input file handle to original location
  fsetpos (c->input_file_handle, &anchor);
  
  int m1_ppflag, m2_ppflag;
  char** m1_field_names = alloc_field_names (c->m1_c->characteristics, c->m1_c->num_characteristics, 4, m1_ppflag);
  char** m2_chan_field_names = alloc_field_names (c->m2_c->characteristics, c->m2_c->num_characteristics, 0, m2_ppflag);

  m1_field_names [0] = "channel_id";
  m1_field_names [1] = "tot";
  m1_field_names [2] = "waveform";
  m1_field_names [3] = "parametrics";

  unsigned int n_channels = get_n_channels (c->m2_c, m2_length);

  // Allocate and initialize hit-based data
  mxArray * hit_based_array =
	mxCreateStructMatrix (1, (mwSize) n_hitbased, c->m1_c->num_characteristics + 4 - m1_ppflag, (const char **) m1_field_names);

  for (int i = 0; i < n_hitbased; i++)
	for (int j = 0; j < c->m1_c->num_characteristics + 3 - m1_ppflag; j++)
	  mxSetFieldByNumber (hit_based_array, i, j,
						  j == 2
						  // Assume all channels sample at same rate for now
						  ? mxCreateDoubleMatrix (1, c->m173_c->n_samples_per_channel [1], mxREAL)
						  : j == 3
						  ? mxCreateDoubleMatrix (1, c->m1_c->parametric_info->num_pids, mxREAL)
						  : mxCreateDoubleScalar (0.0));

  c->m1_c->matlab_array_handle = hit_based_array;
  c->m173_c->matlab_array_handle = hit_based_array;

  // Allocate and initialize time-based data
  char * m2_field_names_storage = (char *) mxCalloc (25, 1);
  char ** m2_field_names = (char **) mxCalloc (3, sizeof (char *));
  for (int i = 0; i < 3; i++) { m2_field_names [i] = & m2_field_names_storage [25*i]; }
  m2_field_names [0] = "tot";
  m2_field_names [1] = "parametrics";
  m2_field_names [2] = "channels";
  
  mxArray * time_based_array =
	mxCreateStructMatrix ( 1, (mwSize) n_timebased, 3, (const char **) m2_field_names);


  for (int i = 0; i < n_timebased; i++) {
	mxSetFieldByNumber (time_based_array, i, 0,
						mxCreateDoubleScalar (0.0));
	mxSetFieldByNumber (time_based_array, i, 1,
						mxCreateDoubleMatrix (1, c->m2_c->num_parametrics, mxREAL));
	mxArray * tmp = mxCreateStructMatrix (1, n_channels,
										  c->m2_c->num_characteristics - m2_ppflag,
										  (const char **) m2_chan_field_names);
	for (int j = 0; j < n_channels; j++)
	  for (int k = 0; k < c->m2_c->num_characteristics; k++)
		mxSetFieldByNumber (tmp, j, k, mxCreateDoubleScalar (0.0));
	mxSetFieldByNumber (time_based_array, i, 2, tmp);
  }
  c->m2_c->matlab_array_handle = time_based_array;
}

char ** alloc_field_names (byte* cs, int ncs, int extras, int& ppFlag) {
  ppFlag = false;
  for (int i = 0; i < ncs; i++) { ppFlag |= (cs [i] == 22); }
  // ppFlag will be true if any of the characteristics are 22

  char* storage = (char *) mxCalloc ((extras + ncs - ppFlag), 25);
  char** ret_val = (char **) mxCalloc ((extras + ncs - ppFlag), sizeof (char *));
  
  // Copying strings
  bool b = false;
  for (int i = 0; i < ncs + extras; i++) {
	ret_val [i] = & storage [25 * i];
	if ( i >= extras && cs [i - extras] != 22) {
	  mexPrintf ("sherpa: %s\n", characteristic_names_mx [cs [i - extras]]);
	  memcpy (ret_val [i - b], characteristic_names_mx [cs [i - extras]], 25);
	} else if (i >= extras && cs [i - extras] == 22) { b = true; }
  }

  return ret_val;
}

unsigned int get_n_channels (mx_m2_control* m2_c, unsigned int m2_length) {
  // OK, I'm guessing that all the message 2's are the same length and based on that
  // I can figure out how many channels are in each message 2
  // God damn it, if this information were just in message 6 this would all be so much
  // easier.  That felt good.  Back to work.
  unsigned int length_of_parametrics = 0;
  for (int i = 0; i < m2_c->num_parametrics; i++) {
	if (m2_c->parametrics [i] > 32) { length_of_parametrics += 4;}
	else { length_of_parametrics += 3;}
  }
  unsigned int length_of_characteristics = 1; // 1 for the CID byte
  chid_to_length [22] = *m2_c->partial_power_segs_p;
  for (int i = 0; i < m2_c->num_characteristics; i++) {
	length_of_characteristics += chid_to_length [m2_c->characteristics [i]];
  }
  return (m2_length - 1 - 6 - length_of_parametrics) / length_of_characteristics;
}

void message1_handler_mx (void* data, int length, void* additional_data) {
  mx_m1_control * c = (mx_m1_control *) additional_data;
  mxArray * a = c->matlab_array_handle;

  double TOT = tot_to_double (data, NULL);
  mxSetFieldByNumber (a, c->index, 1, mxCreateDoubleScalar ((double) TOT));

  byte channel_id = *(byte *) data;
  data = ((byte *) data + 1);
  mxSetFieldByNumber (a, c->index, 0, mxCreateDoubleScalar ((double) channel_id));

  bool b = false;
  for (int i = 0; i < c->num_characteristics; i++) {
	double value = chid_handlers_mx [c->characteristics [i]] (data, c->partial_power_segs_p);
	if (c->characteristics [i] == 22) {
	  b = true;
	} else {
	  mxSetFieldByNumber (a, c->index, i + (b ? 3 : 4), mxCreateDoubleScalar (value));
	  // mexPrintf ("readback attempt: a(%d).%s = %f\n", c->index, mxGetFieldNameByNumber (a, i + (b ? 3 : 4)),* mxGetPr (mxGetFieldByNumber (a, c->index, i + (b ? 3 : 4))));
	}
  }

  double * mxParametrics = mxGetPr (mxGetField (a, c->index, "parametrics"));
  set_parametrics (data, c->parametric_info->pids,
				   c->parametric_info->num_pids, mxParametrics);

  c->index++;
}

void message2_handler_mx (void* data, int length, void* additional_data) {
  mx_m2_control * c = (mx_m2_control *) additional_data;
  mxArray * a = c->matlab_array_handle;

  double TOT = tot_to_double (data, NULL);
  mxSetFieldByNumber (a, c->index, 0, mxCreateDoubleScalar (TOT));

  double * mxParametrics = mxGetPr (mxGetFieldByNumber (a, c->index, 1));
  set_parametrics (data, c->parametrics, c->num_parametrics, mxParametrics);

  unsigned int n_channels = get_n_channels (c, length + 1);

  mxArray * chs = mxGetFieldByNumber (a, c->index, 2);
  for (int i = 0; i < n_channels; i++) {
	data = (byte *) data + 1;
	for (int j = 0; j < c->num_characteristics; j++) {
	  double value = chid_handlers_mx [c->characteristics [j]] (data, c->partial_power_segs_p);
	  mexPrintf ("value = %f\n", value);
	  if (c->characteristics [j] != 22)
		mxSetFieldByNumber (chs, i,
							c->characteristics [j] > 22
							? j - 1
							: j,
							mxCreateDoubleScalar (value));
	}
  }

  c->index++;
}

void message173_handler_mx (void* data, int length, void* additional_data) {
  mx_m173_control * c = (mx_m173_control *) additional_data;

  // Check sub-ID byte
  if (*(byte *)data == 42) {
	mx_store_num_samples (data, c);
	return;
  } else if (*(byte *)data != 1) {
	return;
  }
  mxArray * a = mxGetFieldByNumber (c->matlab_array_handle, c->index, 2);
  double * w = mxGetPr ( mxGetFieldByNumber (c->matlab_array_handle, c->index, 2));
  byte channel_id = *( (byte *) data + 7);
  short * m_w = (short *) ((byte *) data + 9);

  for (int i = 0; i < c->n_samples_per_channel [channel_id]; i++)
	w [i] = (double) m_w [i] * 10.0 / 32768.0;

  c->index++;
}

void set_parametrics (void* &data, byte* ps, unsigned int nps, double * mxParametrics) {
  for (int i = 0; i < nps; i++) {
	byte * holder;
	data = (byte *) data + 1;
	holder = (byte *) data;
	if (ps [i] > 32) {
	  mxParametrics [i] = holder [0] + 256 * (holder [1] + 256 * holder [2]);
	  data = (byte *) data + 3;
	} else {
	  mxParametrics [i] = ((double) *(short *) data) * 10.0 / 32768.0;
	  data = (byte *) data + 2;
	}
  }
}

void mx_store_num_samples (void* data, mx_m173_control* c) {
  // Don't care about most of the data in this message
  // Just jump to the ones I care about.
  byte channel_id = *((byte *) data + 8);
  unsigned short num_samples = 1024 * (unsigned short) *((byte *) data + 9);
  mexPrintf ("Channel %d needs %d samples\n", channel_id, num_samples);
  if (channel_id == 0) {
	for (int i = 0; i < sizeof (c->n_samples_per_channel); i++)
	  c->n_samples_per_channel [i] = num_samples;
  } else {
	c->n_samples_per_channel [channel_id] = num_samples;
  }
}

double sig_strength_to_double (void* &data, void* ctx) {
  double result = (double) (*(unsigned int *)(data)) * 3.05f;
  data = (byte *) data + 4;
  return result;
}

double tot_to_double (void* &data, void* ctx) {
  double v=0.0;

  unsigned short* up = (unsigned short *) data;

  // scale the 6-byte quantity
  for (int i=3; i; )
	v = v * 65536.0 + (double)up[--i];

  data = (byte *) data + 6;
  return v / 4e6;
}
   
double ushort_to_double (void* &data, void* ctx) {
  double result = (double) *(unsigned short *)data;
  data = (byte *) data + 2;
  return result;
}

double uint_to_double (void* &data, void* ctx) {
  double result = (double) *(unsigned int *)data;
  data = (byte *) data + 4;
  return result;
}

double byte_to_double (void* &data, void* ctx) {
  double result = (double) *(byte *) data;
  data = (byte *) data + 1;
  return result;
}

double abs_energy_to_double (void* & data, void* ctx) {
  double result = *(float *) data * 0.000931f;
  data = (byte *) data + 4;
  return result;
}

double rms16_to_double (void* &data, void* ctx) {
  double result = (*(unsigned short *) data) / 5000.0f;
  data = (byte *) data + 2;
  return result;
}

double skip_partial_power_mx (void* &data, void* ctx) {
  data = (byte *) data + *(int *) ctx;
  mexPrintf ("HERRO! %d\n", *(int *) ctx);
  return -1;
}

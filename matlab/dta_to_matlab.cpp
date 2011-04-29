#include "dta_to_matlab.h"
#include <sys/stat.h>
#include <math.h>

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
   &partial_power_to_byte_array_as_double,
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

void mx_handlers_init (bool process_hit_based, bool process_time_based,
					   bool process_waveform, bool process_time_marks) {
  memset (mx_handlers, NULL, sizeof (mx_handlers));
  mx_handlers [1] = process_hit_based ? &message1_handler_mx : NULL;
  mx_handlers [2] = process_time_based ? &message2_handler_mx : NULL;
  mx_handlers [5] = &message5_handler_mx;
  mx_handlers [6] = &message6_handler_mx;
  mx_handlers [8] = &message8_handler_common;
  mx_handlers [23]= &message23_handler_mx;
  mx_handlers [24] = &message24_handler_mx;
  mx_handlers [26] = &message26_handler_mx;
  mx_handlers [42] = &message42_handler_common;
  mx_handlers [106] = &message106_handler_mx;
  mx_handlers [109] = &message109_handler_mx;
  mx_handlers [110] = &message110_handler_mx;
  mx_handlers [11] = &message128_handler_mx;
  mx_handlers [173] = process_waveform ? &message173_handler_mx : NULL;
  mx_handlers [211] = process_time_marks ? &message211_handler_mx : NULL;
}

void mx_ctx_init () { memset (mx_ctx, NULL, sizeof (mx_ctx)); }

void message5_handler_mx (void* data, int length, void* additional_data) {
  mx_m1_control* c = (mx_m1_control *) additional_data;
  
  c->num_characteristics = *(byte *) data;
  c->characteristics = (byte *) malloc(c->num_characteristics);
  memcpy(c->characteristics, (byte *) data + 1, c->num_characteristics);
  c->num_parametrics = *( (byte *) data + 1 + c->num_characteristics);
}

void message6_handler_mx (void* data, int length, void* additional_data) {
  mx_m2_control* c = (mx_m2_control *) additional_data;
  
  c->num_characteristics = ((byte *) data)[0];
  if (c->num_characteristics > 0) {
	c->characteristics = (byte *) malloc(c->num_characteristics);
	memcpy(c->characteristics, (void*) ((byte *) data + 1), c->num_characteristics);
  } else { c->characteristics = NULL; }
  
  c->num_parametrics = *( (byte *) data + 1 + c->num_characteristics);
  if (c->num_parametrics > 0) {
	c->parametrics = (byte *) malloc(c->num_parametrics);
	memcpy(c->parametrics, (void*) ((byte*) data + 1 + c->num_characteristics + 1), c->num_parametrics);
  } else { c->parametrics = NULL; }
}

void message23_handler_mx (void* data, int length, void* additional_data) {
  mx_m173_control* c = (mx_m173_control *) additional_data;

  unsigned short channel_id = *(byte *) data;
  unsigned short gain_db = *((byte *) data + 1);

  c->channel_gain [channel_id] = (int)(pow(10.0, (gain_db / 20.0)) + 0.5);
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

void message106_handler_mx (void* data, int length, void* additional_data) {
  mx_m2_control * c = (mx_m2_control *) additional_data;

  c->n_channels = *((byte *) data + 1);
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

  // Okay, we want to count the message 1's and message 2's, so we know enough
  // to allocate the MATLAB struct.  First save place in the file;
  fpos_t anchor;
  fgetpos ( c->input_file_handle, &anchor );

  unsigned short local_length;
  byte id;

  unsigned int n_channels = c->m2_c->n_channels;

  memset (c->m173_c->num_data_points, 0, (__AE_NUM_CHANNELS + 1) * sizeof (unsigned int));

  unsigned int n_timebased, m2_length, n_marks;
  unsigned int * n_hitbased = (unsigned int *) mxCalloc (n_channels, sizeof (unsigned int));
  memset (n_hitbased, 0, n_channels * sizeof (unsigned int));
  n_timebased = n_marks = 0;

  struct stat st;
  fstat (fileno (c->input_file_handle), &st);
  
  fread (&local_length, 2, 1, c->input_file_handle);
  fread (&id, 1, 1, c->input_file_handle);

  bool seen_173 = false;
  while (! feof (c->input_file_handle) && local_length != 0 ) {

	// Don't bother counting 173 - There's one for every 1
	if (id == 173) seen_173 = true;
	if (id == 1) {
	  if (seen_173) {
		fpos_t m1_anchor;
		byte channel_id;
		fgetpos (c->input_file_handle, & m1_anchor);
		fseek (c->input_file_handle, 6, SEEK_CUR);
		fread (&channel_id, 1, 1, c->input_file_handle);
		fsetpos (c->input_file_handle, & m1_anchor);
	  
		(n_hitbased [channel_id - 1])++;
		seen_173 = false;
	  }
	}
	else if (id == 2) {
	  n_timebased++;
	  m2_length = local_length;
	}
	else if (id == 211) {
	  n_marks++;
	}

	if ((long int) local_length - 1 + ftell (c->input_file_handle) >= st.st_size) {
	  break;
	}
	
	fseek (c->input_file_handle, (long int) (local_length - 1), SEEK_CUR);
	fread (&local_length, 2, 1, c->input_file_handle);
	fread (&id, 1, 1, c->input_file_handle);
  }

  unsigned short non_empty_channels = 0;

  memcpy (c->m173_c->num_data_points + 1, n_hitbased, n_channels * sizeof (unsigned int));

  for (unsigned short i = 0; i < n_channels; i++)
	non_empty_channels += n_hitbased [i] > 0;

  memset (c->m173_c->channel_map, 0, (__AE_NUM_CHANNELS + 1)*sizeof (unsigned short));
  
  // Map each non-empty channel ID to its location in the output array
  unsigned short cur_chan_index = 0;
  for (unsigned short i = 0; i < n_channels; i++)
	if (n_hitbased [i] > 0)
	  c->m173_c->channel_map [i + 1] = cur_chan_index++;

  memcpy (c->m1_c->channel_map , c->m173_c->channel_map,
		  (__AE_NUM_CHANNELS + 1) * sizeof (unsigned short));

  // Return input file handle to original location
  fsetpos (c->input_file_handle, &anchor);
  
  char** m1_field_names = alloc_field_names (c->m1_c->characteristics, c->m1_c->num_characteristics, 5);
  char** m2_chan_field_names = alloc_field_names (c->m2_c->characteristics, c->m2_c->num_characteristics, 0);

  m1_field_names [0] = "channel_id";
  m1_field_names [1] = "tot";
  m1_field_names [2] = "x";
  m1_field_names [3] = "waveform";
  m1_field_names [4] = "parametrics";

  // Allocate and initialize hit-based data
  mxArray * hit_based_array =
	mxCreateStructMatrix (1, non_empty_channels,
						  c->m1_c->num_characteristics + 5,
						  (const char **) m1_field_names);


  for (unsigned int k = 0; k < n_channels; k++) {
	if (n_hitbased [k] > 0) {
	  for (unsigned int j = 0; j < c->m1_c->num_characteristics + 5; j++) {
		mxSetFieldByNumber (hit_based_array, c->m173_c->channel_map [k + 1], j,
							j == 2
							? (c->options [2]
							   ? mxCreateDoubleMatrix (c->m173_c->n_samples_per_channel [k + 1], 1, mxREAL)
							   : mxCreateString ("unused"))
							: j == 3
							? (c->options [2]
							   ? mxCreateDoubleMatrix (c->m173_c->n_samples_per_channel [k + 1], n_hitbased [k], mxREAL)
							   : mxCreateString ("unused"))
							: j == 4
							? mxCreateDoubleMatrix (c->m1_c->parametric_info->num_pids, n_hitbased [k], mxREAL)
							: (j >= 5 && c->m1_c->characteristics [j - 5] == 22)
							? mxCreateDoubleMatrix (*c->m1_c->partial_power_segs_p, n_hitbased [k], mxREAL)
							: mxCreateDoubleMatrix (1, n_hitbased [k], mxREAL));
	  }
	}
  }

  c->m1_c->matlab_array_handle = hit_based_array;
  c->m173_c->matlab_array_handle = hit_based_array;

  // Allocate and initialize time-based data
  char * m2_field_names_storage = (char *) mxCalloc (25, 3);
  char ** m2_field_names = (char **) mxCalloc (3, sizeof (char *));
  for (int i = 0; i < 3; i++) { m2_field_names [i] = & m2_field_names_storage [25*i]; }
  m2_field_names [0] = "tot";
  m2_field_names [1] = "parametrics";
  m2_field_names [2] = "ae_characteristics";
  
  mxArray * time_based_array =
	mxCreateStructMatrix ( 1, 1, 3, (const char **) m2_field_names);

  mxSetFieldByNumber (time_based_array, 0, 0,
										mxCreateDoubleMatrix (1, n_timebased, mxREAL));
  mxSetFieldByNumber (time_based_array, 0, 1,
										mxCreateDoubleMatrix (c->m2_c->num_parametrics, n_timebased, mxREAL));
  
  mxArray * tmp = mxCreateStructMatrix (1, n_channels,
										   c->m2_c->num_characteristics,
										   (const char **) m2_chan_field_names);
  for (int j = 0; j < n_channels; j++)
	for (int k = 0; k < c->m2_c->num_characteristics; k++)
	  mxSetFieldByNumber (tmp, j, k,
						  c->m2_c->characteristics [k] == 22
						  ? mxCreateDoubleMatrix (*c->m2_c->partial_power_segs_p, n_timebased, mxREAL)
						  : mxCreateDoubleMatrix (1, n_timebased, mxREAL));
  mxSetFieldByNumber (time_based_array, 0, 2, tmp);
  
  c->m2_c->matlab_array_handle = time_based_array;

  if (c->options [2]) {
	for (int i = 0; i < n_channels; i++) {
	  if (n_hitbased [i] > 0) {
		double * d = mxGetPr (mxGetFieldByNumber (hit_based_array, c->m173_c->channel_map [i+1], 2));
		double xmin = c->m173_c->channel_tdly [i+1]
		  / (1000.0 * c->m173_c->channel_srate [i+1]);
		double xmax = ((c->m173_c->channel_tdly [i+1] + c->m173_c->n_samples_per_channel [i+1]) - 1)
		  / (1000.0 * c->m173_c->channel_srate [i+1]);
		int N = c->m173_c->n_samples_per_channel [i+1];
		for (int j = 0; j < N; j++)
		  d [j] = xmin + (double) j / (N - 1) * (xmax - xmin);
	  }
	}
  }

  c->m211_c->matlab_array_handle = mxCreateDoubleMatrix (1, n_marks, mxREAL);
}

char ** alloc_field_names (byte* cs, int ncs, int extras) {
  char* storage = (char *) mxCalloc ((extras + ncs), 25);
  char** ret_val = (char **) mxCalloc ((extras + ncs), sizeof (char *));
  
  // Copying strings
  for (int i = 0; i < ncs + extras; i++) {
	ret_val [i] = & storage [25 * i];
	if ( i >= extras )
	  memcpy (ret_val [i], characteristic_names_mx [cs [i - extras]], 25); }

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
  double* d;

  double TOT = tot_to_double (data, NULL);
  byte channel_id = *(byte *) data;

  *(mxGetPr (mxGetFieldByNumber (a, c->channel_map [channel_id], 0/*"channel_id"*/)) + c->index [channel_id]) = channel_id;
  *(mxGetPr (mxGetFieldByNumber (a, c->channel_map [channel_id], 1/*"tot"*/)) + c->index [channel_id]) = TOT;

  if (TOT != *c->last_tot || ! *c->seen_173) {
	return;
  }
  
  data = ((byte *) data + 1);
  
  for (int i = 0; i < c->num_characteristics; i++) {
	double value = chid_handlers_mx [c->characteristics [i]] (data, c->partial_power_segs_p);
	if (c->characteristics [i] == 22) {
	  double* d = mxGetPr (mxGetFieldByNumber (a, c->channel_map [channel_id], i + 5));
	  // Yes, I am using a double as an array of bytes for the case of the partial powers
	  byte* segs = (byte *) &value;
	  long int offset = *c->partial_power_segs_p * c->index [channel_id];
	  for (int j = 0; j < *c->partial_power_segs_p; j++) d [offset + j] = (double) segs [j];
	} else {
	  *(mxGetPr (mxGetFieldByNumber (a, c->channel_map [channel_id], i + 5)) + c->index [channel_id]) = value;
	}
  }

  double * mxParametrics = (mxGetPr (mxGetField (a, c->channel_map [channel_id], "parametrics"))
							+
							c->parametric_info->num_pids * c->index [channel_id]);
  set_parametrics (data, c->parametric_info->pids,
				   c->parametric_info->num_pids, mxParametrics);

  c->index [channel_id] ++;
  *c->seen_173 = false;
}


void message2_handler_mx (void* data, int length, void* additional_data) {
  mx_m2_control * c = (mx_m2_control *) additional_data;
  mxArray * a = c->matlab_array_handle;

  double TOT = tot_to_double (data, NULL);
  *(mxGetPr (mxGetFieldByNumber (a, 0, 0)) + c->index) = TOT;

  double * mxParametrics =
	mxGetPr (mxGetFieldByNumber (a, 0, 1)) + c->num_parametrics * c->index;
  set_parametrics (data, c->parametrics, c->num_parametrics, mxParametrics);

  unsigned int n_channels = c->n_channels;

  mxArray * chs = mxGetFieldByNumber (a, 0, 2);
  for (int i = 0; i < n_channels; i++) {
	data = (byte *) data + 1;
	for (int j = 0; j < c->num_characteristics; j++) {
	  double value = chid_handlers_mx [c->characteristics [j]] (data, c->partial_power_segs_p);
	  double* d = mxGetPr (mxGetFieldByNumber (chs, i, j));
	  if (c->characteristics [j] == 22) {
		// Yes, I am using value as an array of bytes for the case of the partial powers
		long int offset = c->index * *c->partial_power_segs_p;
		byte* segs = (byte *) &value;
		for (int k = 0; k < *c->partial_power_segs_p; j++) d [offset + k] = (double) segs [k];
	  } else {
		d [c->index] = value;
	  }
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

  data = (byte *) data + 1;
  double TOT = tot_to_double (data, NULL);

  c->last_tot = TOT;
  
  byte channel_id = *(byte *) data;

  if (c->seen_173) {
	c->index [channel_id]--;
  }

  long int offset = c->n_samples_per_channel [channel_id] * c->index [channel_id];
  double * w = mxGetPr (mxGetFieldByNumber (c->matlab_array_handle,
											c->channel_map [channel_id], 3));
  short * m_w = (short *) ((byte *) data + 2);
  double sc_fac = c->channel_mxin [channel_id] / c->channel_gain [channel_id] / 32768.0;

  if (c->index [channel_id] < c->num_data_points [channel_id]) {
	for (int i = 0; i < c->n_samples_per_channel [channel_id]; i++)
	  w [offset + i] = (double) m_w [i] * sc_fac;
	c->index [channel_id]++;
	c->seen_173 = true;
  }
}

void message211_handler_mx (void* data, int length, void* additional_data) {
  mx_m211_control * c = (mx_m211_control *) additional_data;

  double* d = mxGetPr (c->matlab_array_handle);
  d [c->index++] = tot_to_double (data, NULL);
}

void set_parametrics (void* &data, byte* ps, unsigned int nps, double * mxParametrics) {
  if (mxParametrics) { for (int i = 0; i < nps; i++) {
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
}

void mx_store_num_samples (void* data, mx_m173_control* c) {
  // Don't care about most of the data in this message
  // Just jump to the ones I care about.
  byte channel_id = *((byte *) data + 8);
  unsigned short num_samples = 1024 * (unsigned short) *((byte *) data + 9);
  unsigned short srate = *(unsigned short *) ((byte *) data + 13);
  short tdly = *(short *) ((byte *) data + 19);
  unsigned short mxin = *(unsigned short *) ((byte *) data + 21);
  if (channel_id == 0) {
	for (int i = 0; i < sizeof (c->n_samples_per_channel); i++) {
	  c->n_samples_per_channel [i] = num_samples;
	  c->channel_srate [i] = srate;
	  c->channel_tdly [i] = tdly;
	  c->channel_mxin [i] = mxin; }}
  else {
	c->n_samples_per_channel [channel_id] = num_samples;
	c->channel_srate [channel_id] = srate;
	c->channel_tdly [channel_id] = tdly;
	c->channel_mxin [channel_id] = mxin; }
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

// Yes, I mean to do this. There will always be few enough partial power
// segments to fit in a double.  Essentially, I'm using the "double" return
// value as a a byte array in the caller.  I feel dirty, but I figure it's
// worth it to maintain the architecture of the code.
double partial_power_to_byte_array_as_double (void* &data, void* ctx) {
  double ret_val = 0;
  memcpy (& ret_val, data, *(int *) ctx);
  data = (byte *) data + *(int *) ctx;
  return ret_val;
}

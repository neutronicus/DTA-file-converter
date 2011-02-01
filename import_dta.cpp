#include <mex.h>
#include <string.h>
#include "dta_to_matlab.h"

void mexFunction (int nlhs, mxArray * plhs [], int nrhs, const mxArray * prhs []) {
  char * filename;
  FILE * dta_file;

  if (! mxIsChar (prhs [0]))
	mexErrMsgTxt ("Specify a filename in a string\n");
  if (nlhs != 4)
	mexErrMsgTxt ("Specify three values on left-hand-side: <hit-based>, <time-based>, <waveform x>, <marks>\n");

  filename = (char *) malloc (1 + mxGetN (prhs [0]));
  mxGetString (prhs [0], filename, 1 + mxGetN (prhs [0]));

  dta_file = fopen (filename, "rb");
  if (! dta_file) mexErrMsgTxt ("Error opening file.");

  mx_m1_control m1_c;        mx_m2_control m2_c;
  mx_m173_control m173_c;    mx_m128_control m128_c;
  m110_data p_info;          mx_m211_control m211_c;
  int n_pp_segs;             

  m128_c.m1_c = & m1_c;      m128_c.m2_c = & m2_c;
  m128_c.m173_c = & m173_c;  m128_c.m211_c = & m211_c;
  m128_c.input_file_handle = dta_file;

  m1_c.partial_power_segs_p = m2_c.partial_power_segs_p = &n_pp_segs;

  m2_c.index = 0;
  m211_c.index = 0;
  memset (& m1_c.index [0], 0, (__AE_NUM_CHANNELS + 1) * sizeof (int));
  memset (& m173_c.index [0], 0, (__AE_NUM_CHANNELS + 1) * sizeof (int));
  memset (& m173_c.n_samples_per_channel, 0, (__AE_NUM_CHANNELS + 1) * sizeof (unsigned short));
  memset (& m173_c.channel_tdly, 0, (__AE_NUM_CHANNELS + 1) * sizeof (short));
  memset (& m173_c.channel_srate, 0, (__AE_NUM_CHANNELS + 1) * sizeof (unsigned short));
  memset (& m173_c.channel_mxin, 0, (__AE_NUM_CHANNELS + 1) * sizeof (unsigned short));
  memset (& m173_c.channel_gain, 0, (__AE_NUM_CHANNELS + 1) * sizeof (unsigned short));
  memset (& m173_c.channel_pdt, 0, (__AE_NUM_CHANNELS + 1) * sizeof (unsigned short));
  memset (& m173_c.channel_hdt, 0, (__AE_NUM_CHANNELS + 1) * sizeof (unsigned short));

  m1_c.parametric_info = & p_info;

  mx_ctx_init ();          mx_handlers_init ();

  mx_ctx [1] = &m1_c;      mx_ctx [2] = &m2_c;
  mx_ctx [5] = &m1_c;      mx_ctx [6] = &m2_c;
  mx_ctx [8] = dta_file;   mx_ctx [23] = &m173_c;
  mx_ctx [24] = &m173_c;   mx_ctx [26] = &m173_c;
  mx_ctx [42] = dta_file;  mx_ctx [109] = &n_pp_segs;
  mx_ctx [106] = &m2_c;    mx_ctx [110] = &m1_c;
  mx_ctx [128] = &m128_c;  mx_ctx [173] = &m173_c;
  mx_ctx [211] = &m211_c;


  parse_dta_file (mx_handlers, mx_ctx, dta_file);

  free (m1_c.characteristics);
  free (m2_c.characteristics);
  free (m2_c.parametrics);
  free (p_info.pids);

  plhs [0] = m1_c.matlab_array_handle;
  plhs [1] = m2_c.matlab_array_handle;
  plhs [2] = m128_c.x_coordinates;
  plhs [3] = m211_c.matlab_array_handle;

  return; }


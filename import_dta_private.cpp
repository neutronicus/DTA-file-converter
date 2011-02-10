#include <mex.h>
#include <string.h>
#include "dta_to_matlab.h"

// This function takes a cell array of variable names, and a logical array corresponding to
// to which pieces of data are requested.

void mexFunction (int nlhs, mxArray * plhs [], int nrhs, const mxArray * prhs []) {
  char * filename;
  FILE * dta_file;

  filename = (char *) malloc (1 + mxGetN (prhs [0]));
  mxGetString (prhs [0], filename, 1 + mxGetN (prhs [0]));

  unsigned short total_names_length = 0;
  for (unsigned short i = 0; i < 4; i++)
	total_names_length += mxGetN (mxGetCell (prhs [1], i));

  char * var_names_storage = (char *) malloc (4 + total_names_length);
  char * var_names [4];

  var_names [0] = var_names_storage;
  var_names [1] = var_names [0] + 1 + mxGetN (mxGetCell (prhs [1], 0));
  var_names [2] = var_names [1] + 1 + mxGetN (mxGetCell (prhs [1], 1));
  var_names [3] = var_names [2] + 1 + mxGetN (mxGetCell (prhs [1], 2));

  for (unsigned short i=0; i < 4; i++) {
	mxArray* the_var_name = mxGetCell (prhs [1], i);
	mxGetString (the_var_name, var_names [i], 1 + mxGetN (the_var_name));
  }

  mxLogical* options = mxGetLogicals (prhs [2]);

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

  mx_ctx_init ();
  mx_handlers_init (options [0], options [1], options [2], options [3]);

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

 
  if (options [0]) mexPutVariable ("base", var_names [0], m1_c.matlab_array_handle);
  if (options [1]) mexPutVariable ("base", var_names [1], m2_c.matlab_array_handle);
  if (options [2]) mexPutVariable ("base", var_names [2], m128_c.x_coordinates);
  if (options [3]) mexPutVariable ("base", var_names [3], m211_c.matlab_array_handle); 

  return; }

#include <mex.h>
#include <string.h>
#include "dta_to_matlab.h"

void mexFunction (int nlhs, mxArray * plhs [], int nrhs, const mxArray * prhs []) {
  char * filename;
  FILE * dta_file;

  if (! mxIsChar (prhs [0]))
	mexErrMsgTxt ("Specify a filename in a string");
  if (nlhs != 2)
	mexErrMsgTxt ("Specify two values on left-hand-side");

  filename = (char *) malloc (1 + mxGetN (prhs [0]));
  mxGetString (prhs [0], filename, 1 + mxGetN (prhs [0]));

  dta_file = fopen (filename, "rb");
  if (! dta_file) mexErrMsgTxt ("Error opening file.");

  mx_m1_control m1_c;        mx_m2_control m2_c;
  mx_m173_control m173_c;    mx_m128_control m128_c;
  m110_data p_info;          int n_pp_segs;

  m128_c.m1_c = & m1_c;      m128_c.m2_c = & m2_c;
  m128_c.m173_c = & m173_c;  m128_c.input_file_handle = dta_file;

  m1_c.partial_power_segs_p = m2_c.partial_power_segs_p = &n_pp_segs;

  m1_c.index = m2_c.index = m173_c.index = 0;

  m1_c.parametric_info = & p_info;

  mx_ctx_init ();          mx_handlers_init ();

  mx_ctx [1] = &m1_c;      mx_ctx [2] = &m2_c;
  mx_ctx [5] = &m1_c;      mx_ctx [6] = &m2_c;
  mx_ctx [24] = &m173_c;   mx_ctx [26] = &m173_c;
  mx_ctx [42] = dta_file;  mx_ctx [109] = &n_pp_segs;
  mx_ctx [110] = &m1_c;    mx_ctx [128] = &m128_c;
  mx_ctx [173] = &m173_c;

  parse_dta_file (mx_handlers, mx_ctx, dta_file);

  free (m1_c.characteristics);
  free (m2_c.characteristics);
  free (m2_c.parametrics);
  free (p_info.pids);

  plhs [0] = m1_c.matlab_array_handle;
  plhs [1] = m2_c.matlab_array_handle;

  return;
}

#ifndef __DTA_TO_MATLAB_H
#define __DTA_TO_MATLAB_H
#include "dta_to_json.h"
#include <mex.h>

typedef struct m1_control_matlab_struct {
  byte num_characteristics;
  byte* characteristics;
  unsigned short num_parametrics;
  m110_data* parametric_info;
  int* partial_power_segs_p;
  int index [__AE_NUM_CHANNELS + 1];
  mxArray * matlab_array_handle;
} mx_m1_control;

typedef struct m2_control_matlab_struct {
  byte num_characteristics;
  byte* characteristics;
  unsigned short num_parametrics;
  byte* parametrics;
  int* partial_power_segs_p;
  int index;
  int n_channels; // Bastards make you infer this.
  mxArray * matlab_array_handle;
} mx_m2_control;

typedef struct m173_control_matlab_struct {
  unsigned short n_samples_per_channel [__AE_NUM_CHANNELS + 1];
  unsigned short channel_srate [__AE_NUM_CHANNELS + 1];
  short channel_tdly [__AE_NUM_CHANNELS + 1]; // This is signed
  unsigned short channel_mxin [__AE_NUM_CHANNELS + 1];
  unsigned short channel_gain [__AE_NUM_CHANNELS + 1];
  unsigned short channel_hdt [__AE_NUM_CHANNELS + 1];
  unsigned short channel_pdt [__AE_NUM_CHANNELS + 1];
  int index [__AE_NUM_CHANNELS + 1];
  mxArray * matlab_array_handle;
} mx_m173_control;

// Message 128 indicates the start of actual data and the end of setup
// Might as well allocate MATLAB arrays here.
typedef struct m128_control_matlab_struct {
  mx_m1_control* m1_c; // Needs to know number of parametrics, AE characteristics, etc.
  mx_m2_control* m2_c; // Ditto
  mx_m173_control* m173_c; // Needs to know number of samples
  mxArray * x_coordinates;
  FILE* input_file_handle; // Needs the file handle to count the number of 1's, 2's, and 173's
} mx_m128_control;

typedef double (* characteristic_handler_mx) (void* &, void*);

void message1_handler_mx (void*, int, void*);
void message2_handler_mx (void*, int, void*);
void message5_handler_mx (void*, int, void*);
void message6_handler_mx (void*, int, void*);
void message23_handler_mx (void*, int, void*);
void message24_handler_mx (void*, int, void*);
void message26_handler_mx (void*, int, void*);
// For message 42, message42_handler_json works fine here
void message106_handler_mx (void*, int, void*);
void message109_handler_mx (void*, int, void*);
void message110_handler_mx (void*, int, void*);
// Message 128 signals the start of data, so this is when I allocate all of the matlab
// arrays.
void message128_handler_mx (void*, int, void*);
void message173_handler_mx (void*, int, void*);

extern message_handler mx_handlers [256];
extern void * mx_ctx [256];
extern characteristic_handler_mx chid_handlers_mx [];

double sig_strength_to_double (void* &data, void* ctx);
double tot_to_double (void* &data, void* ctx);
double ushort_to_double (void* &data, void* ctx);
double uint_to_double (void* &data, void* ctx);
double byte_to_double (void* &data, void* ctx);
double abs_energy_to_double (void* & data, void* ctx);
double rms16_to_double (void* &data, void* ctx);
// It's a long name - but this function is a litte screwy
double partial_power_to_byte_array_as_double (void* &data, void* ctx);

void mx_ctx_init ();
void mx_handlers_init ();

char ** alloc_field_names (byte*, int, int);
unsigned int get_n_channels (mx_m2_control *, unsigned int);
void set_parametrics (void* & , byte* , unsigned int, double *);
void mx_store_num_samples (void*, mx_m173_control*);

#endif

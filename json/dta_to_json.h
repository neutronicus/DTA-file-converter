#ifndef __DTA_TO_JSON_H
#define __DTA_TO_JSON_H
#include <dta_common.h>
#include <api/yajl_gen.h>

/*********************************************************************************
 * These functions are valid targets for the message_handler typedef from
 * dta_parse_skeleton.h
 *********************************************************************************/

void message1_handler_json (void*, int, void*);
void message2_handler_json (void*, int, void*);
void message5_handler_json (void*, int, void*);
void message6_handler_json (void*, int, void*);
void message23_handler_json (void*, int, void*);
void message24_handler_json (void*, int, void*);
void message26_handler_json (void*, int, void*);
void message109_handler_json (void*, int, void*);
void message110_handler_json (void*, int, void*);
void message173_handler_json (void*, int, void*);
void message211_handler_json (void*, int, void*);

extern message_handler json_handlers [256];
void json_handlers_init ();

extern void * json_ctx [256];
void json_ctx_init ();

/*********************************************************************************
 * The functions below are for handling AE characteristics.
 *********************************************************************************/

typedef void (* characteristic_handler_json) (yajl_gen, void* &, void*);

void ushort_to_json (yajl_gen, void* &, void*);
void uint_to_json (yajl_gen, void* &, void*);
void byte_to_json (yajl_gen, void* &, void*);
void tot_to_json (yajl_gen, void* &, void*);
void rms16_to_json (yajl_gen, void* &, void*);
void sig_strength_to_json (yajl_gen, void* &, void*);
void abs_energy_to_json (yajl_gen, void* &, void*);
void partial_power_to_json (yajl_gen, void* &, void*);

extern characteristic_handler_json chid_handlers [];

extern char characteristic_names_json [] [25];

// These are populated by reading a "5" message, a "110" message, and a "109" message
typedef struct m1_control_struct {
  byte num_characteristics;
  byte* characteristics;
  unsigned short num_parametrics;
  m110_data* parametric_info;
  int* partial_power_segs_p;
  yajl_gen json_handle;
  FILE* output_handle;
} m1_control;

// These are populated by reading a "6" message, 
typedef struct m2_control_struct {
  byte num_characteristics;
  byte* characteristics;
  unsigned short num_parametrics;
  byte* parametrics;
  int* partial_power_segs_p;
  yajl_gen json_handle;
  FILE* output_handle;
} m2_control;

typedef struct m173_control_struct {
  yajl_gen json_handle;
  FILE* output_handle;
  unsigned short n_samples_per_channel [__AE_NUM_CHANNELS + 1];
  unsigned short channel_srate [__AE_NUM_CHANNELS + 1];
  short channel_tdly [__AE_NUM_CHANNELS + 1]; // This is signed
  unsigned short channel_mxin [__AE_NUM_CHANNELS + 1];
  unsigned short channel_gain [__AE_NUM_CHANNELS + 1];
  unsigned short channel_pdt [__AE_NUM_CHANNELS + 1];
  unsigned short channel_hdt [__AE_NUM_CHANNELS + 1];
} m173_control;

typedef struct m211_control_struct {
  yajl_gen json_handle;
} m211_control;

#endif

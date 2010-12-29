#ifndef __DTA_TO_JSON_H
#define __DTA_TO_JSON_H
#include "dta_parse_skeleton.h"
#include <yajl/yajl_gen.h>


/*********************************************************************************
 * These functions are valid targets forthe message_handler typedef from dta_parse_skeleton.h
 *********************************************************************************/

void message1_handler (void*, int, void*);
void message2_handler (void*, int, void*);
void message5_handler (void*, int, void*);
void message6_handler (void*, int, void*);
void message42_handler (void*, int, void*);
void message109_handler (void*, int, void*);
void message110_handler (void*, int, void*);
void message173_handler (void*, int, void*);

extern message_handler json_handlers [256];
void json_handlers_init ();

extern void * json_ctx [256];
void json_ctx_init ();

/*********************************************************************************
 * The functions below are for handling AE characteristics.
 *********************************************************************************/

typedef void (* characteristic_handler) (yajl_gen, void* &, void*);

void ushort_to_json (yajl_gen, void* &, void*);
void uint_to_json (yajl_gen, void* &, void*);
void byte_to_json (yajl_gen, void* &, void*);
void tot_to_json (yajl_gen, void* &, void*);
void rms16_to_json (yajl_gen, void* &, void*);
void sig_strength_to_json (yajl_gen, void* &, void*);
void abs_energy_to_json (yajl_gen, void* &, void*);
void skip_partial_power (yajl_gen, void* &, void*);

extern characteristic_handler chid_handlers [];
extern byte chid_to_length [];

typedef struct m110_data_struct {
  byte num_pids;
  byte* pids;
} m110_data;

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
} m173_control;


#endif

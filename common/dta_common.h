#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define __AE_NUM_CHANNELS 32

typedef void (* message_handler) (void*, int, void*);
typedef unsigned char byte;

int parse_dta_file (message_handler*, void **, FILE*);

void message8_handler_common (void*, int, void*);
void message42_handler_common (void*, int, void*);

typedef struct m110_data_struct {
  byte num_pids;
  byte* pids;
} m110_data;

extern byte chid_to_length [];

#endif

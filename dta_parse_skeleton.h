#ifndef __DTA_PARSE_SKELETON_H
#define __DTA_PARSE_SKELETON_H
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef void (* message_handler) (void*, int, void*);
typedef unsigned char byte;

int parse_dta_file (message_handler*, void **, FILE*);

#endif

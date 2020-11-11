#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "polymatrix.h"

#ifndef IO_H
#define IO_H
#define MAXSTR 100

void not_impl (char *info);
void read_conf (const char *s);
polymatrix_t *read_polymatrix_from_file(FILE *f, int* rdim1, int* rdim2) ;
#endif

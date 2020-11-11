#include <math.h>
#include "matrix.h"

#ifndef UTIL_H
#define UTIL_H

typedef struct {
    matrix_t *xt;
    matrix_t *xt_R_y;
    matrix_t *xt_C_y;
    matrix_t *Ct_x;
    matrix_t *xt_C;
    matrix_t *C_y;
    matrix_t *R_y;
    matrix_t *xt_R;
    int br_R_y_index;
    double br_R_y_val;
    int br_Ct_x_index;
    double br_Ct_x_val;
}extra_t;

extra_t *extra_alloc(matrix_t *R, matrix_t *C, matrix_t *Ct, matrix_t *x, matrix_t *y);
void extra_free(extra_t *ext);

double best_response(matrix_t *mat_s, int *index);
matrix_t *find_delta_br(matrix_t *mat_s, double delta);
matrix_t *not_delta_br(matrix_t *mat_s, double delta);
double compute_epsilon(matrix_t *R, matrix_t *C, matrix_t *x, matrix_t *y, double *eps1, double *eps2);
double compute_epsilon_extra(extra_t *ext, double *eps1, double *eps2);
double compute_epsilon_supp(matrix_t *R, matrix_t *C, matrix_t *x, matrix_t *y, double *eps1, double *eps2);

#endif

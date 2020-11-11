#include "matrix.h"
#include "strategy.h"
#ifndef POLYMATRIX_H
#define POLYMATRIX_H

typedef struct polymatrix {
    int players;
    int **graph;
    matrix_t ***payoffs;
}polymatrix_t;

polymatrix_t *polymatrix_alloc(int players);
void polymatrix_free(polymatrix_t *game);
int count_strategies(polymatrix_t *game, int *s_count);
void polymatrix_set_bimatrix(polymatrix_t *game, matrix_t *A, matrix_t *B, int i, int j);
void polymatrix_normalize(polymatrix_t *game, int *count);
void polymatrix_to_file(FILE *f, char *info, polymatrix_t *game);
void polymatrix_print(polymatrix_t *game);
matrix_t *polymatrix_compute_vi(polymatrix_t *game, strategy_profile_t *x, int i);
double polymatrix_compute_ui_vi(matrix_t *v_i, strategy_profile_t *x, int i);
double polymatrix_compute_ui(polymatrix_t *game, strategy_profile_t *x, int i);
matrix_t *delta_best_response_vi(matrix_t *v_i, double delta);
matrix_t *delta_best_response_i(polymatrix_t *game, strategy_profile_t *x, double delta, int i);
double best_response_vi(matrix_t *v_i, int *index);
double best_response_i(polymatrix_t *game, strategy_profile_t *x, int *index, int i);
double compute_eps(polymatrix_t *game, strategy_profile_t *x, double *eps_i);

#endif

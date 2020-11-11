#include "matrix.h"

#ifndef STRATEGY_H
#define STRATEGY_H

typedef struct
{
    int players;
    matrix_t **strategies;
}strategy_profile_t;

strategy_profile_t *strategy_alloc(int n, int *scount);
strategy_profile_t *strategy_alloc_pure(int n, int *scount);
void strategy_free(strategy_profile_t *x);
void strategy_print(strategy_profile_t *x);
strategy_profile_t *strategy_sub(strategy_profile_t *x, strategy_profile_t *xp, int *scount);
void strategy_read(strategy_profile_t *x);
void strategy_from_class(strategy_profile_t *x, int argc, char **argv);

#endif

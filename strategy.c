#include "strategy.h"
#include <string.h>

strategy_profile_t *strategy_alloc(int n, int *scount)
{
    strategy_profile_t *x = malloc(sizeof(strategy_profile_t));
    x->players = n;
    
    int i, j;
    x->strategies = malloc(sizeof(matrix_t *) * n);
    for (i = 0; i < n; ++i) {
        x->strategies[i] = matrix_alloc(scount[i], 1);
        for (j = 0; j < scount[i]; j++)
            x->strategies[i]->data[j][0] = 1.0 / (double)scount[i];
    }

    return x;
}

strategy_profile_t *strategy_alloc_pure(int n, int *scount)
{
    strategy_profile_t *x = malloc(sizeof(strategy_profile_t));
    x->players = n;
    
    int i, j;
    x->strategies = malloc(sizeof(matrix_t *) * n);
    for (i = 0; i < n; ++i) {
        x->strategies[i] = matrix_alloc(scount[i], 1);
        j = rand() % scount[i];
        x->strategies[i]->data[j][0] = 1;
    }

    return x;
}

void strategy_free(strategy_profile_t *x)
{
    int i;
    for (i = 0; i < x->players; ++i)
        matrix_free(x->strategies[i]);
    free(x->strategies);

    free(x);
}

strategy_profile_t *strategy_sub(strategy_profile_t *x, strategy_profile_t *xp, int *scount)
{
    strategy_profile_t *xh = malloc(sizeof(strategy_profile_t));
    xh->players = x->players;
    xh->strategies = malloc(sizeof(matrix_t *) * x->players);
    
    int i;
    for (i = 0; i < x->players; ++i)
        xh->strategies[i] = matrix_sub(x->strategies[i], xp->strategies[i]);

    return xh;
}

void strategy_print(strategy_profile_t *x)
{
    int i, j;
    printf("s= ");
    for (i = 0; i < x->players; ++i){
        for (j = 0; j < x->strategies[i]->nrows; ++j)
            printf("%le ", x->strategies[i]->data[j][0]);
    }
}

void strategy_read(strategy_profile_t *x)
{
    int i, j;
    for (i = 0; i < x->players; ++i)
        for (j = 0; j < x->strategies[i]->nrows; ++j)
            scanf("%lf ", &x->strategies[i]->data[j][0]);
}

void strategy_from_class(strategy_profile_t *x, int argc, char **argv)
{
    int i, j, k = 3;
    if (strcmp(argv[2], "s=") == 0) {
        for (i = 0; i < x->players; ++i)
            for (j = 0; j < x->strategies[i]->nrows; ++j)
                x->strategies[i]->data[j][0] = atof(argv[k++]);
        return;
    }
}

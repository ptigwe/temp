#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "polymatrix.h"

polymatrix_t *polymatrix_alloc(int players)
{
    int i;
    polymatrix_t *game = malloc(sizeof(polymatrix_t));

    game->players = players;

    game->graph = malloc(sizeof(int *) * players);
    for (i = 0; i < players; i++)
        game->graph[i] = calloc(players, sizeof(int));

    game->payoffs = malloc(sizeof(matrix_t **) * players);
    int j;
    for (i = 0; i < players; i++) {
        game->payoffs[i] = malloc(sizeof(matrix_t *) * players);
        for (j = 0; j < players; j++)
            game->payoffs[i][j] = NULL;
    }

    return game;
}

void polymatrix_free(polymatrix_t *game)
{
    int i, j;

    for (i = 0; i < game->players; ++i) {
        for (j = 0; j < game->players; ++j)
            if (game->payoffs[i][j])
                matrix_free(game->payoffs[i][j]);
        free(game->graph[i]);
        free(game->payoffs[i]);
    }

    free(game->graph);
    free(game->payoffs);
    free(game);
}

int count_strategies(polymatrix_t *game, int *s_count)
{
    int count = 0;
    int i, j;
    
    for (i = 0; i < game->players; ++i)
        for (j = 0; j < game->players; ++j)
            if (game->payoffs[i][j]){
                s_count[i] = game->payoffs[i][j]->nrows;
                count += s_count[i];
                break;
            }

    return count;
}

void polymatrix_set_bimatrix(polymatrix_t *game, matrix_t *R, matrix_t *C, int i, int j)
{
    game->payoffs[i][j] = R;
    game->payoffs[j][i] = C;
}

void polymatrix_upper_lower_i_row(polymatrix_t *game, int i, int r, double *U, double *L)
{
    int j, k;
    double m, M;
    *U = 0;
    *L = 0;

    for (j = 0; j < game->players; ++j) {
        if (!game->payoffs[i][j])
            continue;
        matrix_t *A = game->payoffs[i][j];

        m = M = A->data[r][0];
        for (k = 1; k < A->ncols; ++k) {
            if (m > A->data[r][k])
                m = A->data[r][k];
            if (M < A->data[r][k])
                M = A->data[r][k];
        }
        *U += M;
        *L += m;
    }
}

void polymatrix_upper_lower_i(polymatrix_t *game, int i, int n, double *U, double *L)
{
    int p;
    double m, M;
    polymatrix_upper_lower_i_row(game, i, 0, U, L);

    for (p = 1; p < n; ++p) {
        polymatrix_upper_lower_i_row(game, i, p, &M, &m);
        if (M > *U)
            *U = M;
        if (m < *L)
            *L = m;
    }
}

void polymatrix_normalize(polymatrix_t *game, int *count)
{
    int i, j;
    double U, L;
    U = -INFINITY;
    L = INFINITY;
    for (i = 0; i < game->players; ++i) {
        polymatrix_upper_lower_i(game, i, count[i], &U, &L);
        //printf("i: %d U: %lf L: %lf\n", i, U, L);
        int di = 0;
        for (j = 0; j < game->players; ++j) {
            if (!game->payoffs[i][j])
                continue;
            di++;
        }
        for (j = 0; j < game->players; ++j) {
            if (!game->payoffs[i][j])
                continue;
            if (U == L){
                matrix_set_const(game->payoffs[i][j], 1.0 / di);
            }
            else {
                matrix_add_const_in(game->payoffs[i][j], (-L / di));
                matrix_mul_const_in(game->payoffs[i][j], 1 / (U - L));
            }
        }
    }
}

void polymatrix_to_file(FILE *f, char *info, polymatrix_t *game)
{
    int i, j;

    fprintf(f, "%d\n", game->players);

    for (i = 0; i < game->players; i++)
        for (j = 0; j < game->players; j++)
            fprintf(f, "%d ", game->graph[i][j]);

    for (i = 0; i < game->players; i++)
        for (j = i + 1; j < game->players; j++) {
            if (game->payoffs[i][j] != NULL) {
                fprintf(f, "%d %d ", game->payoffs[i][j]->nrows, game->payoffs[i][j]->ncols);
                matrix_write(game->payoffs[i][j], f);
                matrix_write(game->payoffs[j][i], f);
            }
        }
}

void polymatrix_print(polymatrix_t *game)
{
    int i, j;
    printf("Graph\n");
    for (i = 0; i < game->players; i++){
        for (j = 0; j < game->players; j++) {
            printf("%d ", game->graph[i][j]);
        }
        printf("\n");
    }

    printf("Games\n");
    for (i = 0; i < game->players; i++) {
        for (j = 0; j < game->players; j++) {
            if (game->payoffs[i][j] != NULL) {
                printf("Player %d v %d\n", i, j);
                printf("%d %d\n", game->payoffs[i][j]->nrows, game->payoffs[i][j]->ncols);
                matrix_print(game->payoffs[i][j]);
            }
        }
    }
}

matrix_t *polymatrix_compute_vi(polymatrix_t *game, strategy_profile_t *x, int i)
{
    matrix_t *s, *t, *t2;

    s = NULL;
    int j;
    for (j = 0; j < game->players; ++j) {
        if (i == j || !game->payoffs[i][j])
            continue;
        t = matrix_mul(game->payoffs[i][j], x->strategies[j]);
        if (s) {
            t2 = matrix_add(s, t);
            matrix_free(s);
            matrix_free(t);
            s = t2;
        } else {
            s = t;
        }
    }

    return s;
}

double polymatrix_compute_ui_vi(matrix_t *v_i, strategy_profile_t *x, int i)
{
    matrix_t *xt = matrix_trans(x->strategies[i]);
    matrix_t *xtv = matrix_mul_vec_mat(xt, v_i);
    double res = xtv->data[0][0];
    matrix_free(xt);
    matrix_free(xtv);
    return res;

}

double polymatrix_compute_ui(polymatrix_t *game, strategy_profile_t *x, int i)
{
    matrix_t *v = polymatrix_compute_vi(game, x, i);
    double res = polymatrix_compute_ui_vi(v, x, i);
    matrix_free(v);
    return res;
}

matrix_t *delta_best_response_vi(matrix_t *v_i, double delta)
{
    int i, j, count = 0;
    double payoff = v_i->data[0][0];

    for (i = 0; i < v_i->nrows; ++i)
        if (payoff < v_i->data[i][0]) {
            payoff = v_i->data[i][0];
        }

    for (i = 0; i < v_i->nrows; ++i)
        if (v_i->data[i][0] >= (payoff - delta))
            count++;

    matrix_t *delta_br = matrix_alloc(count, 1);
    j = 0;
    for (i = 0; i < v_i->nrows; ++i)
        if (v_i->data[i][0] >= payoff - delta)
            delta_br->data[j++][0] = i;

    //printf("vi\n");
    //matrix_print(v_i);
    //printf("Delta br\n");
    //matrix_print(delta_br);
    return delta_br;
}

matrix_t *delta_best_response_i(polymatrix_t *game, strategy_profile_t *x, double delta, int i)
{
    matrix_t *vi = polymatrix_compute_vi(game, x, i);
    matrix_t *delta_br = delta_best_response_vi(vi, delta);
    matrix_free(vi);
    return delta_br;
}

double best_response_vi(matrix_t *v_i, int *index)
{
    int i, j;
    double payoff = v_i->data[0][0];

    j = 0;
    for (i = 0; i < v_i->nrows; ++i)
        if (payoff < v_i->data[i][0]) {
            j = i;
            payoff = v_i->data[i][0];
        }

    if (index != NULL)
        *index = j;

    return payoff;
}

double best_response_i(polymatrix_t *game, strategy_profile_t *x, int *index, int i)
{
    matrix_t *vi = polymatrix_compute_vi(game, x, i);
    double payoff = best_response_vi(vi, index);
    matrix_free(vi);
    return payoff;
}

double compute_eps_i(polymatrix_t *game, strategy_profile_t *x, int i)
{
    matrix_t *vi = polymatrix_compute_vi(game, x, i);
    double us = best_response_vi(vi, NULL);
    double u = polymatrix_compute_ui_vi(vi, x, i);
    matrix_free(vi);
    return us - u;
}

double compute_eps(polymatrix_t *game, strategy_profile_t *x, double *eps_i)
{
    int i;
    double eps = 0;
    for (i = 0; i < game->players; ++i) {
        double e = compute_eps_i(game, x, i);
        if (eps_i)
            eps_i[i] = e;
        if (eps < e)
            eps = e;
    }
    return eps;
}

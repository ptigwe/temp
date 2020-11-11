#include "util.h"

/* Allocates and computes extra resources neccessary for computation */
extra_t *extra_alloc(matrix_t *R, matrix_t *C, matrix_t *Ct, matrix_t *x, matrix_t *y)
{
    extra_t *ext = malloc(sizeof(extra_t));

    ext->xt = matrix_trans(x);
    ext->Ct_x = matrix_mul_mat_vec(Ct, x);
    ext->xt_C = matrix_trans(ext->Ct_x);
    ext->C_y = matrix_mul_mat_vec(C, y);
    ext->R_y = matrix_mul_mat_vec(R, y);
    ext->xt_R = matrix_mul_vec_mat(ext->xt, R);
    ext->xt_R_y = matrix_mul_vec_mat(ext->xt, ext->R_y);
    ext->xt_C_y = matrix_mul_mat_vec(ext->xt_C, y);
    ext->br_R_y_val = best_response(ext->R_y, &(ext->br_R_y_index));
    ext->br_Ct_x_val = best_response(ext->Ct_x, &(ext->br_Ct_x_index));

    return ext;
}

/* Frees up allocated resources */
void extra_free(extra_t *ext)
{
    matrix_free(ext->xt);
    matrix_free(ext->xt_R_y);
    matrix_free(ext->xt_C_y);
    matrix_free(ext->Ct_x);
    matrix_free(ext->xt_C);
    matrix_free(ext->C_y);
    matrix_free(ext->R_y);
    matrix_free(ext->xt_R);
    free(ext);
}

/* 
 * Computes the best response given a vector of expected payoffs
 * for each pure strategy.
 */
double best_response(matrix_t *mat_s, int *index)
{
    int i, j;
    double payoff = mat_s->data[0][0];

    j = 0;
    for (i = 0; i < mat_s->nrows; ++i)
        if (payoff < mat_s->data[i][0]) {
            j = i;
            payoff = mat_s->data[i][0];
        }

    if (index != NULL)
        *index = j;

    return payoff;
}

/* 
 * Computes the set of delta best response values given a vector of expected
 * payoffs for each pure strategy.
 */
matrix_t *find_delta_br(matrix_t *mat_s, double delta)
{
    int i, j, count = 0;
    double payoff = mat_s->data[0][0];

    for (i = 0; i < mat_s->nrows; ++i)
        if (payoff < mat_s->data[i][0]) {
            payoff = mat_s->data[i][0];
        }

    for (i = 0; i < mat_s->nrows; ++i)
        if (mat_s->data[i][0] >= (payoff - delta))
            count++;

    matrix_t *delta_br = matrix_alloc(count, 1);
    j = 0;
    for (i = 0; i < mat_s->nrows; ++i)
        if (mat_s->data[i][0] >= payoff - delta)
            delta_br->data[j++][0] = i;

    return delta_br;
}

/* 
 * Computes the set of strategies not in the delta best response given a vector
 * of expeced payoffs for each pure strategy.
 */
matrix_t *not_delta_br(matrix_t *mat_s, double delta)
{
    int i, j, count = 0;
    double payoff = mat_s->data[0][0];

    for (i = 0; i < mat_s->nrows; ++i)
        if (payoff < mat_s->data[i][0]) {
            payoff = mat_s->data[i][0];
        }

    for (i = 0; i < mat_s->nrows; ++i)
        if (mat_s->data[i][0] < (payoff - delta))
            count++;

    matrix_t *ndelta_br = matrix_alloc(count, 1);
    j = 0;
    for (i = 0; i < mat_s->nrows; ++i)
        if (mat_s->data[i][0] < payoff - delta)
            ndelta_br->data[j++][0] = i;

    return ndelta_br;
}

/* 
 * Returns the expected payoff given the payoff matrix and the mixed strategies
 * of both players.
 */
double get_payoff(matrix_t *R, matrix_t *x, matrix_t *y)
{
    matrix_t *xt = matrix_trans(x);
    matrix_t *Ry = matrix_mul_mat_vec(R, y);
    matrix_t *xtRy = matrix_mul_mat_vec(xt, Ry);

    double payoff = xtRy->data[0][0];

    matrix_free(xt);
    matrix_free(Ry);
    matrix_free(xtRy);
    return payoff;
}

/* 
 * Computes the epsilon-NE for both players and stores them in eps1 and eps2
 * for player 1 and 2 respectively and returns the maximum of the two given
 * the matrices and and the mixed strategies for both players
 */
double compute_epsilon(matrix_t *R, matrix_t *C, matrix_t *x, matrix_t *y, double *eps1, double *eps2)
{
    double u, v, u1, v1, e1, e2;

    matrix_t *xt = matrix_trans(x);
    matrix_t *Ry = matrix_mul_mat_vec(R, y);
    matrix_t *xtRy = matrix_mul_vec_mat(xt, Ry);
    matrix_t *xt_C = matrix_mul_vec_mat(xt, C);
    matrix_t *Ct_x = matrix_trans(xt_C);
    matrix_t *xtCy = matrix_mul_mat_vec(xt_C, y);

    u = xtRy->data[0][0];
    v = xtCy->data[0][0];
    u1 = best_response(Ry, NULL);
    v1 = best_response(Ct_x, NULL);

    matrix_free(xt);
    matrix_free(Ry);
    matrix_free(xtRy);
    matrix_free(xt_C);
    matrix_free(Ct_x);
    matrix_free(xtCy);
    
    e1 = (u < u1) ? u1 - u : 0;
    e2 = (v < v1) ? v1 - v : 0;

    if (eps1 != NULL)
        *eps1 = e1;
    if (eps2 != NULL)
        *eps2 = e2;

    if (e1 > e2)
        return e1;

    return e2;
}

/* 
 * Computes the epsilon-NE for both players and stores them in eps1 and eps2
 * for player 1 and 2 respectively and returns the maximum of the two.
 */
double compute_epsilon_extra(extra_t *ext, double *eps1, double *eps2)
{
    double u, v, u1, v1, e1, e2;
    
    u = ext->xt_R_y->data[0][0];
    v = ext->xt_C_y->data[0][0];

    u1 = ext->br_R_y_val;
    v1 = ext->br_Ct_x_val;
    
    e1 = (u < u1) ? u1 - u : 0;
    e2 = (v < v1) ? v1 - v : 0;

    if (eps1 != NULL)
        *eps1 = e1;
    if (eps2 != NULL)
        *eps2 = e2;

    if (e1 > e2)
        return e1;

    return e2;
}

double compute_epsilon_supp(matrix_t *R, matrix_t *C, matrix_t *x, matrix_t *y, double *eps1, double *eps2)
{
    double u, v, u1, v1, e1, e2;
    e1 = e2 = 0;
    
    matrix_t *xt = matrix_trans(x);
    matrix_t *Ry = matrix_mul_mat_vec(R, y);
    matrix_t *xt_C = matrix_mul_vec_mat(xt, C);
    matrix_t *Ct_x = matrix_trans(xt_C);

    u1 = best_response(Ry, NULL);
    v1 = best_response(Ct_x, NULL);

    int i;
    for (i = 0; i < x->nrows; ++i)
    {
        if (x->data[i][0] > 0)
        {
            u = u1 - Ry->data[i][0];
            if (u > e1)
                e1= u;
        }
    }

    for (i = 0; i < y->nrows; ++i)
    {
        if (y->data[i][0] > 0)
        {
            v = v1 - xt_C->data[0][i];
            if (v > e2)
                e2 = v;
        }
    }

    matrix_free(Ct_x);
    matrix_free(xt_C);
    matrix_free(Ry);
    matrix_free(xt);
    if (eps1 != NULL)
        *eps1 = e1;
    if (eps2 != NULL)
        *eps2 = e2;
    return fmax(e1, e2);
}

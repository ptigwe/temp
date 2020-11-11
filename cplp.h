#include <stdio.h>
#include <gmp.h>
#include <stdlib.h>
#include <ilcplex/cplex.h>

#ifndef CPLP_H
#define CPLP_H

/* A structure containing all neccessary parameters to create and solve a CPLEX
 * LP
 */
typedef struct
{
    int nrows;
    int ncols;
    int objsen;
    double *obj;
    double *rhs;
    char *sense;
    int *matbeg;
    int *matcnt;
    int *matind;
    double *matval;
    double *lb;
    double *ub;
    double *rngval;
    int obj_sense;
}cplp_t;

/* A representation of the solution found by the CPLEX solver */
typedef struct{
    int nrows;
    int ncols;
    double **res;
    double obj_val;
}cplp_sol_t;

cplp_t *create_cplp(int nrows, int ncols, int non_zero, char def_sense);
void cplp_free(cplp_t *lp);
cplp_sol_t *create_cplp_sol(int nrows, int ncols);
void cplp_sol_free(cplp_sol_t *sol);
void cplp_sol_print(cplp_sol_t *sol);
cplp_sol_t *cplp_solve(cplp_t *cplp);

#endif

#include "cplp.h"

/* Creates a representation of a CPLEX LP where
 * nrows -> number of constraints
 * ncols -> number of variables
 * non_zero -> number of non-zero entries in the LP
 * def_sense -> default sense of each constraint
 */
cplp_t *create_cplp(int nrows, int ncols, int non_zero, char def_sense)
{
    int i;
    cplp_t *lp;
   
    lp = malloc(sizeof(cplp_t));
    lp->nrows = nrows;
    lp->ncols = ncols;

    lp->obj = (double *)calloc(ncols, sizeof(double));
    lp->rhs = (double *)calloc(nrows, sizeof(double));
    lp->sense = (char *)malloc(nrows * sizeof(char));
    for (i = 0; i < nrows; ++i)
        lp->sense[i] = def_sense;

    lp->matbeg = (int *)calloc(ncols, sizeof(int));
    lp->matcnt = (int *)calloc(ncols, sizeof(int));
    lp->matind = (int *)calloc(non_zero, sizeof(int));
    lp->matval = (double *)calloc(non_zero, sizeof(double));
    lp->lb = (double *)calloc(ncols, sizeof(double));
    lp->ub = (double *)calloc(ncols, sizeof(double));

    lp->obj_sense = CPX_MAX;

    return lp;
}

/* Frees the memory used by the LP representation */
void cplp_free(cplp_t *lp)
{
    free(lp->ub);
    free(lp->lb);
    free(lp->matind);
    free(lp->matval);
    free(lp->matcnt);
    free(lp->matbeg);
    free(lp->sense);
    free(lp->rhs);
    free(lp->obj);

    free(lp);
}

/* Creates a solution with all zeros */
cplp_sol_t *create_cplp_sol(int nrows, int ncols)
{
    cplp_sol_t *sol = (cplp_sol_t *)malloc(sizeof(cplp_sol_t));
    sol->nrows = nrows;
    sol->ncols = ncols;
    sol->res = (double **)malloc(2 * sizeof(double *));
    sol->res[0] = (double *)calloc(ncols, sizeof(double));
    sol->res[1] = (double *)calloc(nrows, sizeof(double));

    return sol;
}

/* Frees the memory used to store the solution to the LP */
void cplp_sol_free(cplp_sol_t *sol)
{
    free(sol->res[0]);
    free(sol->res[1]);
    free(sol->res);
    free(sol);
}

/* Prints out the solution to screen */
void cplp_sol_print(cplp_sol_t *sol)
{    
    int j;
    int count = 0;

    double sum = 0;
    printf("\n++++\n Sol\n");
    printf("Val %lf\n", sol->obj_val);
    for (j = 0; j < sol->ncols; j++) {
        printf("%lf ", sol->res[0][j]);
    }
    printf("\n+++++\n");
    for (j = 0; j < sol->nrows - 4; j++){
        sum += -sol->res[1][j];
        if(sol->res[1][j] < 0)
            count++;
    }
    //printf("dual : %lf\n", sum);

    //for (j = 0; j < sol->nrows - 4; j++)
    //    printf("%d \t %lf \n", j+1, -sol->res[1][j]);
}

/* 
 * Calls the CPLEX solver and returns the solution to the given lp 
 * Portions of code from CPLEX: lpex3.c
 */
cplp_sol_t *cplp_solve(cplp_t *cplp)
{
    CPXENVptr     env = NULL;
    CPXLPptr      lp = NULL;
    int status, lpstat;
    double objval;
    cplp_sol_t *sol;

    env = CPXopenCPLEX(&status);
    if ( env == NULL ) {
        char  errmsg[CPXMESSAGEBUFSIZE];
        fprintf (stderr, "Could not open CPLEX environment.\n");
        CPXgeterrorstring (env, status, errmsg);
        fprintf (stderr, "%s", errmsg);
        goto TERMINATE;
    }

    /* Create the problem */

    lp = CPXcreateprob (env, &status, "KS");

    if ( lp == NULL ) {
        fprintf (stderr,"Failed to create subproblem\n");
        status = 1;
        goto TERMINATE;
    }

    /* Copy network part of problem.  */

    status = CPXcopylp (env, lp, cplp->ncols, cplp->nrows, cplp->obj_sense, cplp->obj, cplp->rhs, 
            cplp->sense, cplp->matbeg, cplp->matcnt, cplp->matind, cplp->matval, 
            cplp->lb, cplp->ub, NULL);

    if ( status ) {
        fprintf (stderr, "CPXcopylp failed.\n");
        goto TERMINATE;
    }


    status = CPXsetintparam (env, CPXPARAM_LPMethod, CPX_ALG_NET);
    if ( status ) {
        fprintf (stderr, 
                "Failed to set the optimization method, error %d.\n", status);
        goto TERMINATE;
    }

    status = CPXlpopt (env, lp);
    if ( status ) {
        fprintf (stderr, "Failed to optimize LP.\n");
        goto TERMINATE;
    }

    //status = CPXwriteprob(env, lp, "lptest.lp", NULL);
    status = CPXgetobjval (env, lp, &objval);
    if ( status ) {
        fprintf (stderr,"CPXgetobjval failed\n");
        goto TERMINATE;
    }
    
    sol = create_cplp_sol(cplp->nrows, cplp->ncols);
    status = CPXsolution (env, lp, &lpstat, &(sol->obj_val), sol->res[0], sol->res[1], NULL, NULL);

TERMINATE:
    /* Free up the problem as allocated by CPXcreateprob, if necessary */

    if ( lp != NULL ) {
        status = CPXfreeprob (env, &lp);
        if ( status ) {
            fprintf (stderr, "CPXfreeprob failed, error code %d.\n", status);
        }
    }

    /* Free up the CPLEX environment, if necessary */

    if ( env != NULL ) {
        status = CPXcloseCPLEX (&env);

        /* Note that CPXcloseCPLEX produces no output,
           so the only way to see the cause of the error is to use
           CPXgeterrorstring.  For other CPLEX routines, the errors will
           be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON. */

        if ( status ) {
            char  errmsg[CPXMESSAGEBUFSIZE];
            fprintf (stderr, "Could not close CPLEX environment.\n");
            CPXgeterrorstring (env, status, errmsg);
            fprintf (stderr, "%s", errmsg);
        }
    }

    return sol;
}

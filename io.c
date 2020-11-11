#include "io.h"
/* Terminate program and print error message from string 'info'  */
void not_impl (char *info)
{
    fflush(stdout);
    fprintf(stderr, "Program terminated with error. %s\n", info);
    exit(1);
}

/* 
 * Reads a specified pattern 's' from stdin and terminates if the input
 * doesn't match the pattern.
 */
void read_conf (const char *s)
{
    int i, len = strlen(s);
    char a[MAXSTR];
    for (i=0; i<len; i++)
    {
        if (scanf("%1s", &a[i])==EOF)
            /* make sure something is in  a  for error report       */
            a[i] = '\0';
        if (a[i] != s[i])
            /* the chars in  a  from stdin do not match those in s  */
        {
            fprintf(stderr, "\"%s\"  required from input, found \"%s\"\n",
                s, a);
            not_impl("");
        }
    }
}

/* 
 * Reads the bimatrix from file using the gambit file format. 
 * Code originally from Codenotti et. al. 2008. 
 */
polymatrix_t *read_polymatrix_from_file(FILE *f, int* rdim1, int* rdim2) 
{
    int i, j;
    int players;
    int m, n;

    fscanf(f, "%d", &players);
    polymatrix_t *game = polymatrix_alloc(players);

    for (i = 0; i < players; ++i)
        for (j = 0; j < game->players; j++)
            fscanf(f, "%d", &game->graph[i][j]);

    for (i = 0; i < players; ++i){
        for (j = i + 1; j < players; j++){
            if (!game->graph[i][j])
                continue;

            fscanf(f, "%d %d", &m, &n);
            printf("Reading\n");
            matrix_t *R = matrix_alloc(m, n);
            matrix_t *C = matrix_alloc(n, m);
            matrix_read(R, f);
            matrix_read(C, f);
            polymatrix_set_bimatrix(game, R, C, i, j);
        }
    }

    return game;
}

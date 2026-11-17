#include "matrix_mult.h"

void matrix_mult(
    mat_a_t    inA[N][N],
    mat_b_t    inB[N][N],
    mat_prod_t prod[N][N])
{
    // Vong lap nhan ma tran
    Row: for (int i = 0; i < N; i++) {
        Col: for (int j = 0; j < N; j++) {
            prod[i][j] = 0;
            Product: for (int k = 0; k < N; k++) {
                prod[i][j] += inA[i][k] * inB[k][j];
            }
        }
    }
}

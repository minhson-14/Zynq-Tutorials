#ifndef MATRIX_MULT_H
#define MATRIX_MULT_H

#include "ap_int.h"

// Kích thước ma trận 5x5
#define N 5

typedef ap_int<8>  mat_a_t;    // Ma trận đầu vào A: 8-bit
typedef ap_int<8>  mat_b_t;    // Ma trận đầu vào B: 8-bit
typedef ap_int<16> mat_prod_t; // Ma trận kết quả: 16-bit

void matrix_mult(
    mat_a_t    inA[N][N],
    mat_b_t    inB[N][N],
    mat_prod_t prod[N][N]
);

#endif

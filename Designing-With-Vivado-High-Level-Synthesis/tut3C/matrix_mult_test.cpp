#include <iostream>
#include "matrix_mult.h"

int main() {
    mat_a_t    inA[N][N];
    mat_b_t    inB[N][N];
    mat_prod_t prod[N][N];
    mat_prod_t expected[N][N];

    // Khởi tạo ma trận đầu vào
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            inA[i][j] = i * N + j;
            inB[i][j] = (i == j) ? 1 : 0; // Ma trận đơn vị
        }

    // Tính toán kết quả mong đợi bằng phần mềm
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++) {
            expected[i][j] = 0;
            for (int k = 0; k < N; k++)
                expected[i][j] += inA[i][k] * inB[k][j];
        }

    // Gọi hàm HLS để kiểm tra
    matrix_mult(inA, inB, prod);

    // So sánh kết quả
    int pass = 1;
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            if (prod[i][j] != expected[i][j]) {
                std::cout << "Mismatch at [" << i << "][" << j << "]" << std::endl;
                pass = 0;
            }

    if (pass)
        std::cout << "Test passed!" << std::endl;
    else
        std::cout << "Test failed!" << std::endl;

    return (pass ? 0 : 1);
}

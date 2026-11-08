#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

#define ROWS 50U
#define COLS 50U
#define UPPER_RAND_LIMIT 9U
#define LOWER_RAND_LIMIT 1U
//init 2 matrices of 2000 x 2000 integers where each element must be initialized
//allocate sufficient space for resulting matrix
//calculate dot product of them
//verify dot product

void fillMatrix(int (*matrix)[COLS]);
void matrixMultiMethod1(int (*matrix1)[COLS], int (*matrix2)[COLS], int (*res)[COLS]);
void matrixMultiMethod2(int (*matrix1)[COLS], int (*matrix2)[COLS], int (*res)[COLS]);
bool matrixMatch(int (*mat1)[COLS], int (*mat2)[COLS]);

int main() {
   int matrix1[ROWS][COLS];
    int matrix2[ROWS][COLS];
    int matrix_res[ROWS][COLS]; 
    int matrix_verify[ROWS][COLS];

    //fill up values of the matrices
    fillMatrix(matrix1);
    fillMatrix(matrix2);

    //get timestamp before matrix multiplication
    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    //convert seconds and nanoseconds into total nanoseconds
    long long start_ns = (long long)start.tv_sec * 1000000000LL + start.tv_nsec;
    printf("START at ns: %lld\n", start_ns);

    //matrix multiplication
    matrixMultiMethod1(matrix1, matrix2, matrix_res);

    //get timestamp after multiplication
    clock_gettime(CLOCK_MONOTONIC, &end);

    long long end_ns = (long long)end.tv_sec * 1000000000LL + end.tv_nsec;
    printf("END at ns: %lld\n", end_ns);

    //calculate elapsed time
    long long elapsed_ns = end_ns - start_ns;

    printf("Took %lld ns to complete operation.\n", elapsed_ns);
    printf("Took %.6f ms to complete operation.\n",
        elapsed_ns / 1000000.0);

    //2nd matrix operation
    matrixMultiMethod2(matrix1, matrix2, matrix_verify);

    //verification
    bool verification_status = matrixMatch(matrix_verify, matrix_res);
    if (verification_status) printf("Verification process passed. OPERATION SUCCESFUL.\n");
    else printf("Verification process failed. OPERATION FAILED.\n");

    return 0;
}


void fillMatrix(int (*matrix)[COLS]){
    srand(time(NULL));
    size_t rows = 0, cols = 0;

    for (size_t i = 0; i < (size_t)(ROWS * COLS); i++){
        matrix[rows][cols] = rand() % (UPPER_RAND_LIMIT - LOWER_RAND_LIMIT + 1) + LOWER_RAND_LIMIT;
        cols++;

        if (cols == COLS){
            cols = 0;
            rows++;
        }
    }

    return;
}

void matrixMultiMethod1(int (*matrix1)[COLS], int (*matrix2)[COLS], int (*res)[COLS]){
    for (size_t i = 0; i < ROWS; i++){
        for (size_t j = 0; j < COLS; j++){
            int sum = 0;
            for (size_t z = 0; z < COLS; z++){
              sum += matrix1[i][z] * matrix2[z][j];    
            }
            res[i][j] = sum;  
        }
    }
}

void matrixMultiMethod2(int (*matrix1)[COLS], int (*matrix2)[COLS], int (*res)[COLS]){
    for (size_t i = ROWS; i > 0; i--){
        for (size_t j = COLS; j > 0; j--){
            int sum = 0;
            for (size_t z = 0; z < COLS; z++){
              sum += matrix1[i-1][z] * matrix2[z][j-1];    
            }
            res[i-1][j-1] = sum;  
        }
    }
}

bool matrixMatch(int (*mat1)[COLS], int (*mat2)[COLS]){
    for (size_t i = 0; i < COLS; i++){
        for (size_t j = 0; j < ROWS; j++){
            if (mat1[i][j] != mat2[i][j]) return false;
        }
    }
    
    return true;
}
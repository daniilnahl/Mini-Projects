#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <pthread.h>

#define ROWS 3200U
#define COLS 3200U
#define UPPER_RAND_LIMIT 9U
#define LOWER_RAND_LIMIT 1U

struct Matrices{
    int matrix1[ROWS][COLS];
    int matrix2[ROWS][COLS];
    int matrix_res[ROWS][COLS];
    int matrix_verify[ROWS][COLS];
    size_t thread_count;
};

struct Matrices matrices_struct = {0};

void fillMatrix(int (*matrix)[COLS]);
void *matrixMultiMethod1(void *arg);
void *matrixMultiMethod2(void *arg);
bool matrixMatch(void);

int main(int argc, char *argv[]) {
    //num of threads is provided as a cli arg when launching the built program
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number_of_threads>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *endptr;
    size_t n_threads = strtoul(argv[1], &endptr, 10);
    if (endptr == argv[1] || *endptr != '\0' || n_threads == 0 || n_threads > ROWS * COLS) {
        fprintf(stderr, "Invalid thread count\n");
        return EXIT_FAILURE;
    }

    //allocate threads
    pthread_t *threads = malloc(n_threads * sizeof(pthread_t));
    size_t *thread_ids = malloc(n_threads * sizeof(size_t));

    if (threads == NULL || thread_ids == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(threads);
        free(thread_ids);
        return EXIT_FAILURE;
    }

    matrices_struct.thread_count = n_threads;

    //fill the matrices
    srand(time(NULL));
    fillMatrix(matrices_struct.matrix1);
    fillMatrix(matrices_struct.matrix2);

    //timestamp before multiplication
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    //1st matrix multiplication
    size_t created = 0;
    for (size_t i = 0; i < n_threads; i++) {
        thread_ids[i] = i;
        int rc = pthread_create(&threads[i], NULL, matrixMultiMethod1, &thread_ids[i]);
        if (rc != 0) {
            fprintf(stderr, "Failed to create thread %zu (error %d)\n", i, rc);
            break;
        }
        created++;
    }

    //wait for all created threads
    for (size_t i = 0; i < created; i++) {
        pthread_join(threads[i], NULL);
    }
    if (created != n_threads) {
        free(threads);
        free(thread_ids);
        return EXIT_FAILURE;
    }

    //timestamp after multiplication
    clock_gettime(CLOCK_MONOTONIC, &end);
    long long start_ns = (long long)start.tv_sec * 1000000000LL + start.tv_nsec;
    long long end_ns = (long long)end.tv_sec * 1000000000LL + end.tv_nsec;
    long long elapsed_ns = end_ns - start_ns;

    printf("START at ns: %lld\n", start_ns);
    printf("END at ns: %lld\n", end_ns);
    printf("Took %lld ns to complete operation.\n", elapsed_ns);
    printf("Took %.6f ms to complete operation.\n", elapsed_ns / 1000000.0);

    //verification calculation
    created = 0;
    for (size_t i = 0; i < n_threads; i++) {
        thread_ids[i] = i;
        int rc = pthread_create(&threads[i], NULL, matrixMultiMethod2, &thread_ids[i]);
        if (rc != 0) {
            fprintf(stderr, "Failed to create thread %zu (error %d)\n", i, rc);
            break;
        }
        created++;
    }

    for (size_t i = 0; i < created; i++) {
        pthread_join(threads[i], NULL);
    }
    if (created != n_threads) {
        free(threads);
        free(thread_ids);
        return EXIT_FAILURE;
    }

    free(threads);
    free(thread_ids);

    //verification
    bool verification_status = matrixMatch();
    if (verification_status) printf("Verification process passed. OPERATION SUCCESSFUL.\n");
    else printf("Verification process failed. OPERATION FAILED.\n");

    return verification_status ? EXIT_SUCCESS : EXIT_FAILURE;
}

void fillMatrix(int (*matrix)[COLS]) {
    size_t rows = 0, cols = 0;

    for (size_t i = 0; i < (size_t)(ROWS * COLS); i++) {
        matrix[rows][cols] = rand() % (UPPER_RAND_LIMIT - LOWER_RAND_LIMIT + 1) + LOWER_RAND_LIMIT;
        cols++;

        if (cols == COLS) {
            cols = 0;
            rows++;
        }
    }
}

void *matrixMultiMethod1(void *arg) {
    size_t thread_id = *(size_t *)arg;

    for (size_t k = thread_id; k < ROWS * COLS; k += matrices_struct.thread_count) {
        size_t row = k / COLS;
        size_t col = k % COLS;

        int sum = 0;
        for (size_t z = 0; z < COLS; z++) {
            sum += matrices_struct.matrix1[row][z] * matrices_struct.matrix2[z][col];
        }
        matrices_struct.matrix_res[row][col] = sum;
    }
    return NULL;
}

void *matrixMultiMethod2(void *arg) {
    size_t thread_id = *(size_t *)arg;
    size_t total = ROWS * COLS;

    if (thread_id >= total)
        return NULL;

    size_t last = total - 1;
    size_t k = last - ((last - thread_id) % matrices_struct.thread_count);

    while (1) {
        size_t row = k / COLS;
        size_t col = k % COLS;

        int sum = 0;
        for (size_t z = 0; z < COLS; z++) {
            sum += matrices_struct.matrix1[row][z] * matrices_struct.matrix2[z][col];
        }
        matrices_struct.matrix_verify[row][col] = sum;

        //prevents unsigned integer underflow
        if (k < matrices_struct.thread_count)
            break;
        k -= matrices_struct.thread_count;
    }
    return NULL;
}

bool matrixMatch(void) {
    for (size_t i = 0; i < ROWS; i++) {
        for (size_t j = 0; j < COLS; j++) {
            if (matrices_struct.matrix_res[i][j] != matrices_struct.matrix_verify[i][j])
                return false;
        }
    }
    return true;
}
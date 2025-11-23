// gcc -c main.c -o main.o -O2
// nasm -f win64 calc.asm -o calc.o
// gcc main.o calc.o -o program -lm "-Wl,--subsystem,console"
// ./program

#include <stdio.h>
#include <stdlib.h>
#include <time.h>    // srand
#include <windows.h> // QueryPerformanceCounter

/*
external assembly function.
*/
extern void calculate_acceleration(float* input_matrix, int* output_array, int num_cars);

// windows timing 
double get_time() {
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    return (double)count.QuadPart / (double)freq.QuadPart;
}

// correctness check 
void check_correctness() {
    printf("CORRECTNESS CHECK \n");
    int num_cars = 3;
    
    // sample data
    float matrix[3 * 3] = {
        0.0, 62.5, 10.1,
        60.0, 122.3, 5.5,
        30.0, 160.7, 7.8
    };
    
    int results[3] = {0, 0, 0};

    // call the ASM function
    calculate_acceleration(matrix, results, num_cars);

    printf("Input (Vi, Vf, T):\n");
    printf("Row 1: %.1f, %.1f, %.1f -> Result: %d (Expected: 2)\n", matrix[0], matrix[1], matrix[2], results[0]);
    printf("Row 2: %.1f, %.1f, %.1f -> Result: %d (Expected: 3)\n", matrix[3], matrix[4], matrix[5], results[1]);
    printf("Row 3: %.1f, %.1f, %.1f -> Result: %d (Expected: 5)\n", matrix[6], matrix[7], matrix[8], results[2]);
    printf("-------------------------------------------\n\n");
}


// main function for benchmarking
int main() {
    // check if correct first
    check_correctness();

    // init random seed
    srand(time(NULL));

    int sizes[] = {10, 100, 1000, 10000};
    int num_sizes = 4;
    int num_runs = 30;

    printf("PERFORMANCE BENCHMARKS \n");

    // loop vars
    int i, j, run; 

    for (i = 0; i < num_sizes; i++) {
        int Y = sizes[i];
        
        // allocate memory
        float* input_matrix = (float*)_aligned_malloc(Y * 3 * sizeof(float), 16);
        int* output_array = (int*)_aligned_malloc(Y * sizeof(int), 16);

        if (!input_matrix || !output_array) {
            printf("Memory allocation failed!\n");
            return 1;
        }

        // populate with random data
        for (j = 0; j < Y * 3; j++) {
            input_matrix[j] = (float)rand() / (float)(RAND_MAX / 100.0); // random floats 0-100
        }

        double total_time = 0;

        // run benchmark
        for (run = 0; run < num_runs; run++) {
            double start_time = get_time();
            
            calculate_acceleration(input_matrix, output_array, Y);
            
            double end_time = get_time();
            total_time += (end_time - start_time);
        }

        double avg_time_ms = (total_time / num_runs) * 1000.0;
        printf("Average time for Y = %d: %f ms\n", Y, avg_time_ms);

        // free memory
        _aligned_free(input_matrix);
        _aligned_free(output_array);
    }

    return 0;
}
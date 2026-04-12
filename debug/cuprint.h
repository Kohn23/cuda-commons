#pragma once

#include <vector>
#include <cstdio>

#include <cuda_runtime.h>

#include "cucheck.h"

template<typename T>
void fprint_device_array(const char* filename, size_t size, const T* d_array_ptr)
{
    std::vector<T> h_array_buffer(size);

    CUDA_CHECK(cudaMemcpy(
        h_array_buffer.data(),
        d_array_ptr,
        size * sizeof(T),
        cudaMemcpyDeviceToHost
    ));

    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        printf("Error opening file %s!\n", filename);
        return;
    }

    printf("Printing array (size %zu) to %s...\n", size, filename);

    for(size_t i = 0; i < size; i++) {
        fprintf(f, "%.6f\n", (double)h_array_buffer[i]);
    }

    fclose(f);
    printf("Done printing to %s.\n", filename);
}


template<typename T>
void fprint_device_matrix(const char* filename, size_t rows, size_t cols, const T* d_matrix_ptr, size_t ld)
{
    size_t num_elements_to_copy = rows * cols;
    std::vector<T> h_matrix_buffer(num_elements_to_copy);

    CUDA_CHECK(cudaMemcpy2D(
        h_matrix_buffer.data(),     
        rows * sizeof(T),           
        d_matrix_ptr,                
        ld * sizeof(T),            
        rows * sizeof(T),           
        cols,                        
        cudaMemcpyDeviceToHost
    ));

    FILE *f = fopen(filename, "w");
    if (f == NULL) {
        printf("Error opening file %s!\n", filename);
        return;
    }

    printf("Printing matrix (%zu x %zu) to %s...\n", rows, cols, filename);

    for(size_t i = 0; i < rows; i++) {
        for(size_t j = 0; j < cols; j++) {
            fprintf(f, "%.6f", (double)h_matrix_buffer[j * rows + i]);
            if (j == cols - 1) {
                fprintf(f, "\n");
            } else {
                fprintf(f, ",");
            }
        }
    }

    fclose(f);
    printf("Done printing to %s.\n", filename);
}


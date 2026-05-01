#pragma once

#include <vector>
#include <cstdio>

#include <cuda_runtime.h>

#include "cucheck.h"

/**
 * Print a 2D array located on the device to a text file.
 * Parameters follow the convention of cudaMemcpy2D (source side only, pointless to print out paddings).
 *
 * @param filename   Output file name
 * @param src        Pointer to source device memory
 * @param spitch     Pitch of source memory in bytes
 * @param width      Width of the sub‑region to copy (in bytes, must be a multiple of sizeof(T))
 * @param height     Height of the sub‑region to copy (number of rows)
 * @param ld_rows    If true, the leading dimension of src is the number of rows, thus array is column-major.
 * @param delim      Column delimiter (default: ",")
 * @param precision  Floating‑point output precision (default: 6)
 */
template<typename T>
void fprint_device_array2d(const char* filename,
                           const T* src,
                           size_t spitch,
                           size_t width,
                           size_t height,
                           bool ld_rows = true,
                           const char* delim = ",",
                           int precision = 6) {
    
    std::vector<T> host_buffer((width / sizeof(T)) * height);
    CUDA_CHECK(cudaMemcpy2D(host_buffer.data(), width, src, spitch, width, height, cudaMemcpyDeviceToHost));

    std::FILE* file = std::fopen(filename, "w");
    if (!file) {
        std::perror("Failed to open file for writing");
        return;
    }

    size_t rows = ld_rows ? (width / sizeof(T)) : height;
    size_t cols = ld_rows ? height : (width / sizeof(T));

    for(size_t i = 0; i < rows; ++i){
        for(size_t j = 0; j < cols; ++j){
            size_t idx = ld_rows ? (i + j * rows) : (i * cols + j);
            if constexpr (std::is_floating_point_v<T>) {
                std::fprintf(file, "%.*g", precision, host_buffer[idx]);
            } else {
                std::fprintf(file, "%d", host_buffer[idx]);
            }
            if(j < cols - 1) {
                std::fprintf(file, "%s", delim);
            }
        }
        std::fprintf(file, "\n");
    }
    std::fclose(file);
}

template<typename T>
void fprint_device_array(const char* filename, size_t size, const T* d_array_ptr) {
    // Treat as 1 row, size columns, source pitch = size * sizeof(T)
    fprint_device_array2d(filename,
                          d_array_ptr,
                          size * sizeof(T),   // spitch
                          size * sizeof(T),   // width
                          1,                  // height
                          false,              // ld_rows (treat as row vector)
                          "\n",               // delim
                          6);
}

#include <stdint.h>
#include <stdlib.h>
#include "riscv.h"
#include "l_trace_encoder.h"

#define ARRAY_SIZE 4096
#define CHUNK_SIZE 64
#define NUM_ITERATIONS 10

typedef struct {
    uint32_t value;
    uint32_t next_index;
} Node;

static volatile Node array[ARRAY_SIZE];

// Fisher-Yates shuffle to randomize array access pattern
static void shuffle_indices(void) {
    for (int i = ARRAY_SIZE - 1; i > 0; i--) {
        // Use a simple PRNG for reproducibility
        uint32_t j = (i * 1103515245 + 12345) % (i + 1);
        array[i].next_index ^= array[j].next_index;
        array[j].next_index ^= array[i].next_index;
        array[i].next_index ^= array[j].next_index;
    }
}

// Sort a chunk using different algorithms based on data properties
static void sort_chunk(uint32_t start_idx, uint32_t size) {
    // Count number of unique values to determine algorithm
    uint32_t unique = 0;
    uint32_t prev = 0;
    
    for (uint32_t i = 0; i < size; i++) {
        uint32_t curr_idx = start_idx;
        for (uint32_t j = 0; j < i; j++) {
            curr_idx = array[curr_idx].next_index;
        }
        if (array[curr_idx].value != prev) {
            unique++;
            prev = array[curr_idx].value;
        }
    }

    // Choose algorithm based on data properties
    if (unique < size/4) {
        // Counting sort for low uniqueness
        uint32_t max = 0;
        uint32_t curr_idx = start_idx;
        for (uint32_t i = 0; i < size; i++) {
            if (array[curr_idx].value > max) max = array[curr_idx].value;
            curr_idx = array[curr_idx].next_index;
        }
        
        uint32_t counts[256] = {0}; // Limited range for simplicity
        curr_idx = start_idx;
        for (uint32_t i = 0; i < size; i++) {
            counts[array[curr_idx].value % 256]++;
            curr_idx = array[curr_idx].next_index;
        }
    } else {
        // Insertion sort for high uniqueness
        for (uint32_t i = 1; i < size; i++) {
            uint32_t curr_idx = start_idx;
            for (uint32_t j = 0; j < i; j++) {
                curr_idx = array[curr_idx].next_index;
            }
            uint32_t key = array[curr_idx].value;
            
            uint32_t j = i - 1;
            uint32_t prev_idx = start_idx;
            while (j >= 0 && array[prev_idx].value > key) {
                uint32_t next_idx = array[prev_idx].next_index;
                array[next_idx].value = array[prev_idx].value;
                j--;
                prev_idx = array[prev_idx].next_index;
            }
            array[curr_idx].value = key;
        }
    }
}

int main() {
    // Initialize with pseudo-random data and linked indices
    for (uint32_t i = 0; i < ARRAY_SIZE; i++) {
        array[i].value = (i * 1103515245 + 12345) & 0xFFFFFFFF;
        array[i].next_index = (i + 1) % ARRAY_SIZE;
    }

    LTraceEncoderType *encoder = l_trace_encoder_get(get_hart_id());
    l_trace_encoder_configure_target(encoder, TARGET_PRINT);
    l_trace_encoder_start(encoder);

    for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
        // Randomize access pattern
        shuffle_indices();
        
        // Sort chunks in different orders
        for (uint32_t i = 0; i < ARRAY_SIZE; i += CHUNK_SIZE) {
            sort_chunk(i, CHUNK_SIZE);
        }
    }

    l_trace_encoder_stop(encoder);
    return 0;
}

#ifndef WAVELET_UTILS_h
#define WAVELET_UTILS_h

#ifdef __cplusplus
extern "C" {
#endif

#include  "hal_mmio.h"
//#include "chip_config.h"
#include <stdint.h>

#define WAVELET_BASE 0x08810000U
#define WAVELET_EVEN_INPUT  (WAVELET_BASE + 0x00)
#define WAVELET_ODD_INPUT   (WAVELET_BASE + 0x04)
#define WAVELET_EVEN_OUTPUT (WAVELET_BASE + 0x08)
#define WAVELET_ODD_OUTPUT  (WAVELET_BASE + 0x0C)
#define WAVELET_START       (WAVELET_BASE + 0x10)
#define WAVELET_INPUT_FLAGS (WAVELET_BASE + 0x14)
#define WAVELET_SEL         (WAVELET_BASE + 0x18)

#define FORWARD 0b100
#define INVERSE 0b000

enum wavelet_sel {
	WAVELET_DB_4,
	WAVELET_BIOR_2_4,
	WAVELET_BIOR_3_5,
	WAVELET_COIF_1
};

enum wavelet_flags {
	WAVELET_FLUSH = 1,
	WAVELET_FLOAT = 1<<2,
	WAVELET_FORWARD = 1<<3
};

/* typedef struct {
   __IO uint64_t EVEN_INPUT;
   __IO uint64_t ODD_INPUT;
   __IO uint64_t EVEN_OUTPUT;
   __IO uint64_t ODD_OUTPUT;
   __IO uint64_t START;
   __IO uint64_t INPUT_FLAGS;
   __IO uint64_t SEL;
 } Wavelet_Type; */

// would need functions for each of the select bit functions?

void wavelet_init();

int wavelet_set_params(uint32_t* input_odd, uint32_t* input_even, uint32_t input_length);
 
void wavelet_read_output(uint32_t *output_odd, uint32_t *output_even, int output_len, int status, uint32_t* input);

uint64_t pack_samples(uint32_t odd_sample, uint32_t even_sample);

void unpack_samples(uint64_t packed, uint32_t* odd_sample, uint32_t* even_sample);
 
void start_wavelet();

void wavelet_forward(uint64_t *input_sample, uint8_t num_tests, uint64_t *output_sample, uint8_t sel);

void wavelet_inverse(uint64_t *input_sample, uint8_t num_tests, uint64_t *output_sample, uint8_t sel);


unsigned int num_outputs(unsigned int num_inputs, bool inverse);
void dwt_int(uint32_t* input, uint32_t* output, unsigned int size, uint8_t wavelet);
void idwt_int(uint32_t* input, uint32_t* output, unsigned int size, uint8_t wavelet);
void dwt_float(float* input, float* output, unsigned int size, uint8_t wavelet);
void idwt_float(float* input, float* output, unsigned int size, uint8_t wavelet);

#ifdef __cplusplus
}
#endif

#endif

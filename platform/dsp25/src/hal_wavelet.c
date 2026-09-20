// #include "hal_wavelet.h"
// #include "hal_mmio.h"
// #include <stdint.h>
// #include <stdbool.h>

// void wavelet_init() {
// 	reg_write32(WAVELET_EVEN_OUTPUT, 0);
// 	reg_write32(WAVELET_ODD_OUTPUT, 0);
// 	reg_write32(WAVELET_EVEN_INPUT, 0);
// 	reg_write32(WAVELET_ODD_INPUT, 0);
// 	reg_write32(WAVELET_START, 0);
// 	reg_write32(WAVELET_INPUT_FLAGS, 0);
// 	reg_write32(WAVELET_SEL, 0);
// }

// void start_wavelet() {
//     reg_write8(WAVELET_START, 1);
// }

// uint64_t pack_samples(uint32_t odd_sample, uint32_t even_sample) {
//     return ((uint64_t)odd_sample << 32) | even_sample;
// }

// void unpack_samples(uint64_t packed, uint32_t* odd_sample, uint32_t* even_sample) {
//     *even_sample = (uint32_t)(packed & 0xFFFFFFFF);
//     *odd_sample = (uint32_t)(packed >> 32);
// }


// void wavelet_forward(uint64_t *input_sample, uint8_t num_tests, uint64_t *output_sample, uint8_t sel) {
// 	reg_write32(WAVELET_INPUT_FLAGS, FORWARD);
//     for (int i = 0; i < num_tests; i += 2) {
// 		reg_write32(WAVELET_EVEN_INPUT, input_sample[i]);
// 		reg_write32(WAVELET_ODD_INPUT, input_sample[i+1]);

// 		output_sample[i] = reg_read32(WAVELET_EVEN_OUTPUT);
// 		output_sample[i+1] = reg_read32(WAVELET_ODD_OUTPUT);
// 	}
// }

// void wavelet_inverse(uint64_t *input_sample, uint8_t num_tests, uint64_t *output_sample, uint8_t sel) {
// 	reg_write32(WAVELET_INPUT_FLAGS, INVERSE);
// 	for (int i = 0; i < num_tests; i += 2) {
// 		reg_write64(WAVELET_EVEN_INPUT, input_sample[i]);
// 		reg_write64(WAVELET_ODD_INPUT, input_sample[i+1]);
    
    	
// 		start_wavelet();

// 		output_sample[i] = reg_read64(WAVELET_EVEN_OUTPUT);
// 		output_sample[i+1] = reg_read64(WAVELET_ODD_OUTPUT);
// 	}
// }

// /* Translate to the HW codes for the respective functions */
// const DWT_FLUSH_CYCLES = 5;
// const IDWT_FLUSH_CYCLES = 6;

// unsigned int num_outputs(unsigned int num_inputs, bool inverse) {size + size%2 + 2*(inverse? IDWT_FLUSH_CYCLES: DWT_FLUSH_CYCLES)}

// // OPTIMIZATIONS TO MAKE:
// // 	Combine writes (especially 0s) into one 64-bit write
// // 	Check flush process
// static void wavelet_int(uint32_t* input, uint32_t* output, unsigned int size, uint8_t wavelet, bool inverse) {
// 	const unsigned int FLUSH_CYCLES = inverse ? IDWT_FLUSH_CYCLES: DWT_FLUSH_CYCLES;
// 	uint8_t flags = 0;
// 	if (!inverse)
// 		flags |= WAVELET_FORWARD;
// 	flags |= WAVELET_FLUSH;

// 	// Flush
// 	reg_write64(WAVELET_EVEN_INPUT, 0);
// 	reg_write8(WAVELET_INPUT_FLAGS, flags);
// 	reg_write8(WAVELET_SEL, wavelet);
// 	reg_write8(WAVELET_START, 1);

// 	flags &= ~WAVELET_FLUSH;
// 	reg_write8(WAVELET_INPUT_FLAGS, flags);
// 	reg_write8(WAVELET_START, 0);

// 	unsigned int outputs = num_outputs(size, inverse);
// 	for (unsigned int i = 0; i < outputs+FLUSH_CYCLES*2; i+=2) {
// 		reg_write32(WAVELET_EVEN_INPUT, i<size ? input[i]: 0);
// 		reg_write32(WAVELET_ODD_INPUT, i+1<size ? input[i+1]: 0);
    		
// 		reg_write8(WAVELET_START, 1);
		
// 		if (i >= FLUSH_CYCLES*2) {
// 			output[i-FLUSH_CYCLES*2] = reg_read32(WAVELET_EVEN_OUTPUT);
// 			output[i-FLUSH_CYCLES*2+1] = reg_read32(WAVELET_ODD_OUTPUT);
// 		}
// 	}
// }

// void dwt_int(uint32_t* input, uint32_t* output, unsigned int size, uint8_t wavelet) {wavelet_int(input, output, size, wavelet, false);}
// void idwt_int(uint32_t* input, uint32_t* output, unsigned int size, uint8_t wavelet) {wavelet_int(input, output, size, wavelet, true);}


// static void wavelet_float(float* input, float* output, unsigned int size, uint8_t wavelet, bool inverse) {
// 	const unsigned int FLUSH_CYCLES = inverse ? IDWT_FLUSH_CYCLES: DWT_FLUSH_CYCLES;
// 	uint8_t flags = 0;
// 	if (!inverse)
// 		flags |= WAVELET_FORWARD;
// 	flags |= WAVELET_FLUSH;
// 	flags |= WAVELET_FLOAT;

// 	// Flush
// 	reg_write64(WAVELET_EVEN_INPUT, 0);
// 	reg_write8(WAVELET_INPUT_FLAGS, flags);
// 	reg_write8(WAVELET_SEL, wavelet);
// 	reg_write8(WAVELET_START, 1);

// 	flags &= ~WAVELET_FLUSH;
// 	reg_write8(WAVELET_INPUT_FLAGS, flags);
// 	reg_write8(WAVELET_START, 0);

// 	unsigned int outputs = num_outputs(size, inverse);
// 	for (unsigned int i = 0; i < outputs+FLUSH_CYCLES*2; i+=2) {
// 		reg_write32(WAVELET_EVEN_INPUT, i<size ? (uint32_t) input[i]: 0);
// 		reg_write32(WAVELET_ODD_INPUT, i+1<size ? (uint32_t) input[i+1]: 0);
    		
// 		reg_write8(WAVELET_START, 1);
		
// 		if (i >= FLUSH_CYCLES*2) {
// 			output[i-FLUSH_CYCLES*2] = (float) reg_read32(WAVELET_EVEN_OUTPUT);
// 			output[i-FLUSH_CYCLES*2+1] = (float) reg_read32(WAVELET_ODD_OUTPUT);
// 		}
// 	}
// }

// void dwt_float(float* input, float* output, unsigned int size, uint8_t wavelet) {wavelet_float(input, output, size, wavelet, false)}
// void idwt_float(float* input, float* output, unsigned int size, uint8_t wavelet) {wavelet_float(input, output, size, wavelet, true)}

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 
    MUST BE RUN WITH logMaxKernelSize = 3 OR IT WILL NOT WORK.
    
    The expectation for software is to zero-extend the kernel array to fit the maximum kernel size.
    In this case, where logMaxKernelSize = 3, the maximum kernel size is 8, so the kernel array must 
    be zero extended by 0 to be of size 8.
*/

#include <string.h>
#include <stdint.h>
#include <inttypes.h>

#define BASE_ADDR 0x08800000

#define INPUT_ADDR      0x08800000
#define OUTPUT_ADDR     0x08800020
#define KERNEL_ADDR     0x08800040
#define START_ADDR      0x0880006C
#define LENGTH_ADDR     0x08800078
#define DILATION_ADDR   0x0880007C

#define WIDTH 256
#define HEIGHT 256



#define FLOAT16_H

#include <stdint.h>

#define f16_tenth   11878
#define f16_fifth   12902
#define f16_third   13653
#define f16_half    14336
#define f16_one     15360
#define f16_two     16384
#define f16_three   16896
#define f16_five    17664
#define f16_ten     18688
#define f16_pi      16968
#define f16_half_pi 15944

short f16_add(short a,short b);
short f16_sub(short a,short b);
short f16_mul(short a,short b);
short f16_div(short a,short b);
short f16_neg(short a);
short f16_from_int(int32_t v);
int32_t f16_int(short v);

int f16_gte(short a,short b);
int f16_gt(short a,short b);
int f16_eq(short a,short b);
int f16_lte(short a,short b);
int f16_lt(short a,short b);



#define SIGN_MASK 0x8000
#define EXP_MASK 0x7C00
#define NAN_VALUE 0x7FFF
#define IS_ZERO(x) (((x) & 0x7FFF) == 0)
#define IS_INVALID(x) (((x) & EXP_MASK) == EXP_MASK)
#define IS_NAN(x) (((x) & 0x7FFF) > 0x7C00)
#define IS_INF(x) ( ((x) & 0x7FFF) == 0x7C00)
#define MANTISSA(x) (((x) & 1023) | (((x) & 0x7C00) == 0 ? 0 : 1024))
#define EXPONENT(x) (((x) & 0x7C00) >> 10)
#define SIGNED_INF_VALUE(x)  ((x & SIGN_MASK) | 0x7C00)

short f16_sub(short ain,short bin)
{
    unsigned short a=ain;
    unsigned short b=bin;
    if(((a ^ b) & 0x8000) != 0)
        return f16_add(a,b ^ 0x8000);
    unsigned short sign = a & 0x8000;
    a = a << 1;
    b = b << 1;
    if(a < b) {
        unsigned short x=a;
        a=b;
        b=x;
        sign ^= 0x8000;
    }
    unsigned short ax = a & 0xF800;
    unsigned short bx = b & 0xF800;
    if(a >=0xf800 || b>=0xf800) {
        if(a > 0xF800 || b > 0xF800 || a==b)
            return 0x7FFF; 
        unsigned short res = sign | 0x7C00;
        if(a == 0xf800)
            return res;
        else
            return res ^ 0x8000;
    }
    int exp_diff = ax - bx;
    unsigned short exp_part  = ax;
    if(exp_diff != 0) {
        int shift = exp_diff >> 11;
        if(bx != 0)
            b = ((b & 2047) | 2048) >> shift;
        else
            b >>= (shift - 1);
    }
    else {
        if(bx == 0) {
            unsigned short res = (a-b) >> 1;
            if(res == 0)
                return res;
            return res | sign;
        }
        else {
            b=(b & 2047) | 2048;
        }
    }
    unsigned short r=a - b;
    if((r & 0xF800) == exp_part) {
        return (r>>1) | sign;
    }
    unsigned short am = (a & 2047) | 2048;
    unsigned short new_m = am - b;
    if(new_m == 0)
        return 0;
    while(exp_part !=0 && !(new_m & (2048))) {
        exp_part-=0x800;
        if(exp_part!=0)
            new_m<<=1;
    }
    return (((new_m & 2047) | exp_part) >> 1) | sign;
}

short f16_add(short a,short b)
{
    if (((a ^ b) & 0x8000) != 0)
        return f16_sub(a,b ^ 0x8000);
    short sign = a & 0x8000;
    a &= 0x7FFF;
    b &= 0x7FFF;
    if(a<b) {
        short x=a;
        a=b;
        b=x;
    }
    if(a >= 0x7C00 || b>=0x7C00) {
        if(a>0x7C00 || b>0x7C00)
            return 0x7FFF;
        return 0x7C00 | sign;
    }
    short ax = (a & 0x7C00);
    short bx = (b & 0x7C00);
    short exp_diff = ax - bx;
    short exp_part = ax;
    if(exp_diff != 0) {
        int shift = exp_diff >> 10;
        if(bx != 0)
            b = ((b & 1023) | 1024) >> shift;
        else
            b >>= (shift - 1);
    }
    else {
        if(bx == 0) {
            return (a + b) | sign;
        }
        else {
            b=(b & 1023) | 1024;
        }
    }
    short r=a+b;
    if ((r & 0x7C00) != exp_part) {
        unsigned short am = (a & 1023) | 1024;
        unsigned short new_m = (am + b) >> 1;
        r =( exp_part + 0x400) | (1023 & new_m);
    }
    if((unsigned short)r >= 0x7C00u) {
        return sign | 0x7C00;
    }
    return r | sign;
}


short f16_mul(short a,short b)
{
    int sign = (a ^ b) & SIGN_MASK;

    if(IS_INVALID(a) || IS_INVALID(b)) {
        if(IS_NAN(a) || IS_NAN(b) || IS_ZERO(a) || IS_ZERO(b))
            return NAN_VALUE;
        return sign | 0x7C00;
    }

    if(IS_ZERO(a) || IS_ZERO(b))
        return 0;
    unsigned short m1 = MANTISSA(a);
    unsigned short m2 = MANTISSA(b);

    uint32_t v=m1;
    v*=m2;
    int ax = EXPONENT(a);
    int bx = EXPONENT(b);
    ax += (ax==0);
    bx += (bx==0);
    int new_exp = ax + bx - 15;

    if(v & ((uint32_t)1<<21)) {
        v >>= 11;
        new_exp++;
    }
    else if(v & ((uint32_t)1<<20)) {
        v >>= 10;
    }
    else { // denormal
        new_exp -= 10;
        while(v >= 2048) {
            v>>=1;
            new_exp++;
        }
    }
    if(new_exp <= 0) {
        v>>=(-new_exp + 1);
        new_exp = 0;
    }
    else if(new_exp >= 31) {
        return SIGNED_INF_VALUE(sign);
    }
    return (sign) | (new_exp << 10) | (v & 1023);
}

short f16_div(short a,short b)
{
    short sign = (a ^ b) & SIGN_MASK;
    if(IS_NAN(a) || IS_NAN(b) || (IS_INVALID(a) && IS_INVALID(b)) || (IS_ZERO(a) && IS_ZERO(b)))
        return 0x7FFF;
    if(IS_INVALID(a) || IS_ZERO(b))
        return sign | 0x7C00;
    if(IS_INVALID(b))
        return 0;
    if(IS_ZERO(a))
        return 0;

    unsigned short m1 = MANTISSA(a);
    unsigned short m2 = MANTISSA(b);
    uint32_t m1_shifted = m1;
    m1_shifted <<= 10;
    uint32_t v= m1_shifted / m2;
    unsigned short rem = m1_shifted % m2;

    int ax = EXPONENT(a);
    int bx = EXPONENT(b);
    ax += (ax==0);
    bx += (bx==0);
    int new_exp = ax - bx + 15 ;

    if(v == 0 && rem==0)
        return 0;

    while(v < 1024 && new_exp > 0) {
        v<<=1;
        rem<<=1;
        if(rem >= m2) {
            v++;
            rem -= m2;
        }
        new_exp--;
    }
    while(v >= 2048) {
        v>>=1;
        new_exp++;
    }

    if(new_exp <= 0) {
        v>>=(-new_exp + 1);
        new_exp = 0;
    }
    else if(new_exp >= 31) {
        return SIGNED_INF_VALUE(sign);
    }
    return sign | (v & 1023) | (new_exp << 10);
}

short f16_neg(short v)
{
    return SIGN_MASK ^ v;
}
short f16_from_int(int32_t sv)
{
    uint32_t v;
    int sig = 0;
    if(sv < 0) {
        v=-sv;
        sig=1;
    }
    else
        v=sv;
    if(v==0)
        return 0;
    int e=25;
    while(v >= 2048) {
        v>>=1;
        e++;
    }
    while(v<1024) {
        v<<=1;
        e--;
    }
    if(e>=31)
        return SIGNED_INF_VALUE(sig << 15);
    return (sig << 15) | (e << 10) | (v & 1023);
}
int32_t f16_int(short a)
{
    unsigned short value = MANTISSA(a);
    short shift = EXPONENT(a) - 25;
    if(shift > 0)
        value <<= shift;
    else if(shift < 0)
        value >>= -shift;
    if(a & SIGN_MASK)
        return -(int32_t)(value);
    return value;
}

int f16_gte(short a,short b)
{
    if(IS_ZERO(a) && IS_ZERO(b))
        return 1;
    if(IS_NAN(a) || IS_NAN(b))
        return 0;
    if((a & SIGN_MASK) == 0) {
        if((b & SIGN_MASK) == SIGN_MASK)
            return 1;
        return a >= b;
    }
    else {
        if((b & SIGN_MASK) == 0)
            return 0;
        return (a & 0x7FFF) <= (b & 0x7FFF);
    }
}

int f16_gt(short a,short b)
{
    if(IS_NAN(a) || IS_NAN(b))
        return 0;
    if(IS_ZERO(a) && IS_ZERO(b))
        return 0;
    if((a & SIGN_MASK) == 0) {
        if((b & SIGN_MASK) == SIGN_MASK)
            return 1;
        return a > b;
    }
    else {
        if((b & SIGN_MASK) == 0)
            return 0;
        return (a & 0x7FFF) < (b & 0x7FFF);
    }

}
int f16_eq(short a,short b)
{
    if(IS_NAN(a) || IS_NAN(b))
        return 0;
    if(IS_ZERO(a) && IS_ZERO(b))
        return 1;
    return a==b;
}

int f16_lte(short a,short b)
{
    if(IS_NAN(a) || IS_NAN(b))
        return 0;
    return f16_gte(b,a);
}

int f16_lt(short a,short b)
{
    if(IS_NAN(a) || IS_NAN(b))
        return 0;
    return f16_gt(b,a);
}
int f16_neq(short a,short b)
{
    return !f16_eq(a,b);
}



// Function prototypes
void gaussianBlur(unsigned char *in_arr, unsigned char *test_out, int width, int height);
void sobelOperator(unsigned char *input, int *gradientX, int *gradientY, int width, int height);
void nonMaxSuppression(int *gradientX, int *gradientY, unsigned char *output, int width, int height);
void doubleThreshold(unsigned char *input, unsigned char *output, int width, int height, int lowThreshold, int highThreshold);
void hysteresis(unsigned char *input, unsigned char *output, int width, int height);

void convolution_1D_char(unsigned char *arr, size_t arr_len, float *kernel, size_t kernel_len, size_t dilation, unsigned char *output) {
  /*

  Computes the convolution of arr with the given kernel and dilation factor and stores the result in output, specifically 
  based on the implementation of the convolution block. The first value in the output array is computed with the kernel's 
  left element aligned with the array's left element.

  arr:        pointer to input array      INT8 array
  arr_len:    length of input array       
  kernel:     pointer to kernel array     FP16 array (represented as uint16_t)
  kernel_len: length of kernel array 
  dilation:   dilation factor
  output:     pointer to output array     FP16 array (represented as uint16_t)

  Example input and output: 

  arr:        {1, 2, 3, 4}
  arr_len:    4
  kernel:     {-1, 1, -1}
  kernel_len: 3
  dilation:   1

  output: {-2, -3, -1, -4} ({-1*1 + 1*2 + -1*3, -1*2 + 1*3 + -1*4, -1*3 + 1*4 + -1*0, -1*4 + 1*0 + -1*0})

  For border values (at the end), we assume the array is zero-extended to fit the length of the kernel (including dilation).
  */

    int y = 0;
    for (int x = 1; x < WIDTH - 1; x++) {
        float sum = 0;
        // for (int ky = -1; ky <= 1; ky++) {
            for (int kx = -1; kx <= 1; kx++) {
                sum += arr[y*WIDTH + (x+kx)] * kernel[kx+1];
            }
        // }
        output[y*WIDTH + x] = (unsigned char)sum;
    }
}

void convolution_1D_int(unsigned char *arr, size_t arr_len, int *kernel, size_t kernel_len, size_t dilation, int *output) {
  /*

  Computes the convolution of arr with the given kernel and dilation factor and stores the result in output, specifically 
  based on the implementation of the convolution block. The first value in the output array is computed with the kernel's 
  left element aligned with the array's left element.

  arr:        pointer to input array      INT8 array
  arr_len:    length of input array       
  kernel:     pointer to kernel array     FP16 array (represented as uint16_t)
  kernel_len: length of kernel array 
  dilation:   dilation factor
  output:     pointer to output array     FP16 array (represented as uint16_t)

  Example input and output: 

  arr:        {1, 2, 3, 4}
  arr_len:    4
  kernel:     {-1, 1, -1}
  kernel_len: 3
  dilation:   1

  output: {-2, -3, -1, -4} ({-1*1 + 1*2 + -1*3, -1*2 + 1*3 + -1*4, -1*3 + 1*4 + -1*0, -1*4 + 1*0 + -1*0})

  For border values (at the end), we assume the array is zero-extended to fit the length of the kernel (including dilation).
  */

    int y = 0;
    for (int x = 1; x < WIDTH - 1; x++) {
        int g = 0;
        // for (int ky = -1; ky <= 1; ky++) {
            for (int kx = -1; kx <= 1; kx++) {
                g += arr[y*WIDTH + (x+kx)] * kernel[kx+1];
            }
        // }
        output[y*WIDTH + x] = g;
    }
}

void twoDimCharConv(unsigned char *in_arr, unsigned char *test_out, float kernel[3][8], int width, int height) {
    uint32_t in_len[1] = {WIDTH};
    uint16_t in_dilation[1] = {1};

    unsigned char test_out1[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out1[(HEIGHT - 2)*WIDTH + i] = 0;
        test_out1[(HEIGHT - 1)*WIDTH + i] = 0;
    }
    unsigned char test_out2[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out2[0*WIDTH + i] = 0;
        test_out2[(HEIGHT - 1)*WIDTH + i] = 0;
    }
    unsigned char test_out3[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out3[0*WIDTH + i] = 0;
        test_out3[1*WIDTH + i] = 0;
    }

    for (int y = 0; y < HEIGHT - 2; y++) {
        // puts("Starting test");
        // uint16_t in_kernel1[8] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16
        // float in_kernel1[8] = {1/16.0, 2/16.0, 1/16.0, 0, 0, 0, 0, 0};
                                                                    

        // puts("Setting values of MMIO registers");
        // for (int x = 0; x < WIDTH; x += 8) {
        //     reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr + x)));             // 64 bits: 8 INT8s
        // }
        // reg_write32(LENGTH_ADDR, in_len[0]);
        // reg_write16(DILATION_ADDR, in_dilation[0]);
        // reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel1));         // 64 bits: 4 FP16s
        // reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel1 + 4)));   // 64 bits: 4 FP16s (Total 8)

        // puts("Starting Convolution");
        // reg_write8(START_ADDR, 1);

        // puts("Waiting for convolution to complete");
        

        
        // printf("Input (INT8): ");
        // for (int i = 0; i < 8; i++) {
        //     printf("%d ", in_arr[y*WIDTH + i]);
        // }

        // printf("\nTest Output (FP16 binary): ");
        // for (int i = 0; i < 2; i++) {
        //     uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
        //     // each output is 4 stacked FP16 so do bit shifting to seperate the results
        //     for (int j = 0; j < 4; j++) {
        //         test_out1[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
        //         printf("0x%"PRIx64" ", test_out1[y*WIDTH + i*4 + j]);
        //     }
        // }
        convolution_1D_char(in_arr + y*WIDTH, in_len[0], kernel[0], 8, in_dilation[0], test_out1 + y*WIDTH);
        // for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out1[0*width + x]);
            }
            printf("\n");
        // }
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        // puts("Starting test");
        // uint16_t in_kernel2[8] = {0x3000, 0x3400, 0x3000, 0, 0, 0, 0, 0}; // {2/16, 4/16, 2/16, 0, 0, 0, 0, 0} in FP16
        float in_kernel2[8] = {2/16.0, 4/16.0, 2/16.0, 0, 0, 0, 0, 0};
                                                                    

        // puts("Setting values of MMIO registers");
        // for (int x = 0; x < WIDTH; x += 8) {
        //     reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr + y*WIDTH + x)));             // 64 bits: 8 INT8s
        // }
        // reg_write32(LENGTH_ADDR, in_len[0]);
        // reg_write16(DILATION_ADDR, in_dilation[0]);
        // reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel2));         // 64 bits: 4 FP16s
        // reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel2 + 4)));   // 64 bits: 4 FP16s (Total 8)

        // puts("Starting Convolution");
        // reg_write8(START_ADDR, 1);

        // puts("Waiting for convolution to complete");
        
        // printf("Input (INT8): ");
        // for (int i = 0; i < 8; i++) {
        //     printf("%d ", in_arr[y*WIDTH + i]);
        // }

        // printf("\nTest Output (FP16 binary): ");
        // for (int i = 0; i < 2; i++) {
        //     uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
        //     // each output is 4 stacked FP16 so do bit shifting to seperate the results
        //     for (int j = 0; j < 4; j++) {
        //         test_out2[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
        //         printf("0x%"PRIx64" ", test_out2[y*WIDTH + i*4 + j]);
        //     }
        // }
        convolution_1D_char(in_arr + y*WIDTH, in_len[0], kernel[1], 8, in_dilation[0], test_out2 + y*WIDTH);
        // for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out2[1*width + x]);
            }
            printf("\n");
        // }
    }

    for (int y = 2; y < HEIGHT; y++) {
        // puts("Starting test");
        // uint16_t in_kernel3[8] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16
        float in_kernel3[8] = {1/16.0, 2/16.0, 1/16.0, 0, 0, 0, 0, 0};
                                                                    

        // puts("Setting values of MMIO registers");
        // for (int x = 0; x < WIDTH; x += 8) {
        //     reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr + y*WIDTH + x)));             // 64 bits: 8 INT8s
        // }
        // reg_write32(LENGTH_ADDR, in_len[0]);
        // reg_write16(DILATION_ADDR, in_dilation[0]);
        // reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel3));         // 64 bits: 4 FP16s
        // reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel3 + 4)));   // 64 bits: 4 FP16s (Total 8)

        // puts("Starting Convolution");
        // reg_write8(START_ADDR, 1);

        // puts("Waiting for convolution to complete");
        
        // printf("Input (INT8): ");
        // for (int i = 0; i < 8; i++) {
        //     printf("%d ", in_arr[y*WIDTH + i]);
        // }

        // printf("\nTest Output (FP16 binary): ");
        // for (int i = 0; i < 2; i++) {
        //     uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
        //     // each output is 4 stacked FP16 so do bit shifting to seperate the results
        //     for (int j = 0; j < 4; j++) {
        //         test_out3[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
        //         printf("0x%"PRIx64" ", test_out3[y*WIDTH + i*4 + j]);
        //     }
        // }
        convolution_1D_char(in_arr + y*WIDTH, in_len[0], kernel[2], 8, in_dilation[0], test_out3 + y*WIDTH);
        // for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out3[2*width + x]);
            }
            printf("\n");
        // }
    }

    // uint16_t test_out[WIDTH * HEIGHT];
    for (int y = 1; y < height - 1; y++) {
        for (int x = 0; x < width; x++) {
            test_out[y*WIDTH+x] = test_out1[(y-1)*WIDTH+x] + test_out2[y*WIDTH+x] + test_out3[(y+1)*WIDTH+x];
        }
    }
}

void twoDimIntConv(unsigned char *in_arr, int *test_out, int kernel[3][8], int width, int height) {
    uint32_t in_len[1] = {WIDTH};
    uint16_t in_dilation[1] = {1};

    int test_out1[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out1[(HEIGHT - 2)*WIDTH + i] = 0;
        test_out1[(HEIGHT - 1)*WIDTH + i] = 0;
    }
    int test_out2[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out2[0*WIDTH + i] = 0;
        test_out2[(HEIGHT - 1)*WIDTH + i] = 0;
    }
    int test_out3[WIDTH * HEIGHT];
    for (int i = 0; i < 8; i++) {
        test_out3[0*WIDTH + i] = 0;
        test_out3[1*WIDTH + i] = 0;
    }

    for (int y = 0; y < HEIGHT - 2; y++) {
        // puts("Starting test");
        // uint16_t in_kernel1[8] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16
        // float in_kernel1[8] = {-1, 0, 1, 0, 0, 0, 0, 0};
                                                                    

        // puts("Setting values of MMIO registers");
        // for (int x = 0; x < WIDTH; x += 8) {
        //     reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr + x)));             // 64 bits: 8 INT8s
        // }
        // reg_write32(LENGTH_ADDR, in_len[0]);
        // reg_write16(DILATION_ADDR, in_dilation[0]);
        // reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel1));         // 64 bits: 4 FP16s
        // reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel1 + 4)));   // 64 bits: 4 FP16s (Total 8)

        // puts("Starting Convolution");
        // reg_write8(START_ADDR, 1);

        // puts("Waiting for convolution to complete");
        

        
        // printf("Input (INT8): ");
        // for (int i = 0; i < 8; i++) {
        //     printf("%d ", in_arr[y*WIDTH + i]);
        // }

        // printf("\nTest Output (FP16 binary): ");
        // for (int i = 0; i < 2; i++) {
        //     uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
        //     // each output is 4 stacked FP16 so do bit shifting to seperate the results
        //     for (int j = 0; j < 4; j++) {
        //         test_out1[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
        //         printf("0x%"PRIx64" ", test_out1[y*WIDTH + i*4 + j]);
        //     }
        // }
        convolution_1D_int(in_arr + y*WIDTH, in_len[0], kernel[0], 8, in_dilation[0], test_out1 + y*WIDTH);
        // for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out1[0*width + x]);
            }
            printf("\n");
        // }
    }

    for (int y = 1; y < HEIGHT - 1; y++) {
        // puts("Starting test");
        // uint16_t in_kernel2[8] = {0x3000, 0x3400, 0x3000, 0, 0, 0, 0, 0}; // {2/16, 4/16, 2/16, 0, 0, 0, 0, 0} in FP16
        // float in_kernel2[8] = {-2, 0, 2, 0, 0, 0, 0, 0};
                                                                    

        // puts("Setting values of MMIO registers");
        // for (int x = 0; x < WIDTH; x += 8) {
        //     reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr + y*WIDTH + x)));             // 64 bits: 8 INT8s
        // }
        // reg_write32(LENGTH_ADDR, in_len[0]);
        // reg_write16(DILATION_ADDR, in_dilation[0]);
        // reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel2));         // 64 bits: 4 FP16s
        // reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel2 + 4)));   // 64 bits: 4 FP16s (Total 8)

        // puts("Starting Convolution");
        // reg_write8(START_ADDR, 1);

        // puts("Waiting for convolution to complete");
        
        // printf("Input (INT8): ");
        // for (int i = 0; i < 8; i++) {
        //     printf("%d ", in_arr[y*WIDTH + i]);
        // }

        // printf("\nTest Output (FP16 binary): ");
        // for (int i = 0; i < 2; i++) {
        //     uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
        //     // each output is 4 stacked FP16 so do bit shifting to seperate the results
        //     for (int j = 0; j < 4; j++) {
        //         test_out2[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
        //         printf("0x%"PRIx64" ", test_out2[y*WIDTH + i*4 + j]);
        //     }
        // }
        convolution_1D_int(in_arr + y*WIDTH, in_len[0], kernel[1], 8, in_dilation[0], test_out2 + y*WIDTH);
        // for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out2[1*width + x]);
            }
            printf("\n");
        // }
    }

    for (int y = 2; y < HEIGHT; y++) {
        // puts("Starting test");
        // uint16_t in_kernel3[8] = {0x2D00, 0x3000, 0x2D00, 0, 0, 0, 0, 0}; // {1/16, 2/16, 1/16, 0, 0, 0, 0, 0} in FP16
        // float in_kernel3[8] = {-1, 0, -1, 0, 0, 0, 0, 0};
                                                                    

        // puts("Setting values of MMIO registers");
        // for (int x = 0; x < WIDTH; x += 8) {
        //     reg_write64(INPUT_ADDR, *((uint64_t*) (in_arr + y*WIDTH + x)));             // 64 bits: 8 INT8s
        // }
        // reg_write32(LENGTH_ADDR, in_len[0]);
        // reg_write16(DILATION_ADDR, in_dilation[0]);
        // reg_write64(KERNEL_ADDR, *((uint64_t*) in_kernel3));         // 64 bits: 4 FP16s
        // reg_write64(KERNEL_ADDR, *((uint64_t*) (in_kernel3 + 4)));   // 64 bits: 4 FP16s (Total 8)

        // puts("Starting Convolution");
        // reg_write8(START_ADDR, 1);

        // puts("Waiting for convolution to complete");
        
        // printf("Input (INT8): ");
        // for (int i = 0; i < 8; i++) {
        //     printf("%d ", in_arr[y*WIDTH + i]);
        // }

        // printf("\nTest Output (FP16 binary): ");
        // for (int i = 0; i < 2; i++) {
        //     uint64_t current_out = reg_read64(OUTPUT_ADDR);         // the OUTPUT is a queue, each read gives one FP16
        //     // each output is 4 stacked FP16 so do bit shifting to seperate the results
        //     for (int j = 0; j < 4; j++) {
        //         test_out3[y*WIDTH + i*4 + j] = (current_out >> (j*16)) & 0xFFFF;
        //         printf("0x%"PRIx64" ", test_out3[y*WIDTH + i*4 + j]);
        //     }
        // }
        convolution_1D_int(in_arr + y*WIDTH, in_len[0], kernel[2], 8, in_dilation[0], test_out3 + y*WIDTH);
        // for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                printf("%d, ", test_out3[2*width + x]);
            }
            printf("\n");
        // }
    }

    // uint16_t test_out[WIDTH * HEIGHT];
    for (int y = 1; y < height - 1; y++) {
        for (int x = 0; x < width; x++) {
            test_out[y*WIDTH+x] = test_out1[(y-1)*WIDTH+x] + test_out2[y*WIDTH+x] + test_out3[(y+1)*WIDTH+x];
        }
    }
}

int main() {
    int width, height;
    unsigned char input[WIDTH * HEIGHT];
    unsigned char output[WIDTH * HEIGHT];
    int gradientX[WIDTH * HEIGHT];
    int gradientY[WIDTH * HEIGHT];

    // Read input image (assuming PGM format)
    FILE *file = fopen("008.pgm", "rb");
    if (!file) {
        printf("Error opening file.\n");
        return 1;
    }

    fscanf(file, "P5\n%d %d\n255\n", &width, &height);
    fread(input, sizeof(unsigned char), width * height, file);
    fclose(file);

    int y = 1;
    // for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            printf("%d, ", input[y*width + x]);
        }
        printf("\n");
    // }

    // Step 1: Gaussian Blur
    gaussianBlur(input, output, width, height);

    y = 1;
    // for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            printf("%d, ", output[y*width + x]);
        }
        printf("\n");
    // }

    // Step 2: Sobel Operator
    sobelOperator(output, gradientX, gradientY, width, height);

    // Step 3: Non-maximum Suppression
    nonMaxSuppression(gradientX, gradientY, output, width, height);

    // Step 4: Double Thresholding
    doubleThreshold(output, output, width, height, 20, 60);

    // Step 5: Hysteresis
    hysteresis(output, output, width, height);

    // Write output image
    file = fopen("output_008.pgm", "wb");
    fprintf(file, "P5\n%d %d\n255\n", width, height);
    fwrite(output, sizeof(unsigned char), width * height, file);
    fclose(file);

    printf("Edge detection completed. Output saved as 'output.pgm'.\n");

    return 0;
}

void gaussianBlur(unsigned char *in_arr, unsigned char *test_out, int width, int height) {
    float kernel[3][8] = {{1/16.0, 2/16.0, 1/16.0, 0, 0, 0, 0, 0},
                          {2/16.0, 4/16.0, 2/16.0, 0, 0, 0, 0, 0},
                          {1/16.0, 2/16.0, 1/16.0, 0, 0, 0, 0, 0}};
    twoDimCharConv(in_arr, test_out, kernel, width, height);
}

void sobelOperator(unsigned char *in_arr, int *gradientX, int *gradientY, int width, int height) {
    int sobelX[3][8] = {{-1, 0, 1, 0, 0, 0, 0, 0},
                            {-2, 0, 2, 0, 0, 0, 0, 0},
                            {-1, 0, 1, 0, 0, 0, 0, 0}};
    int sobelY[3][8] = {{-1, -2, -1, 0, 0, 0, 0, 0},
                            {0, 0, 0, 0, 0, 0, 0, 0},
                            {1, 2, 1, 0, 0, 0, 0, 0}};

    // for (int y = 1; y < height - 1; y++) {
    //     for (int x = 1; x < width - 1; x++) {
    //         int gx = 0, gy = 0;
    //         for (int ky = -1; ky <= 1; ky++) {
    //             for (int kx = -1; kx <= 1; kx++) {
    //                 int pixel = in_arr[(y+ky)*width + (x+kx)];
    //                 gx += pixel * sobelX[ky+1][kx+1];
    //                 gy += pixel * sobelY[ky+1][kx+1];
    //             }
    //         }
    //         gradientX[y*width + x] = gx;
    //         gradientY[y*width + x] = gy;
    //     }
    // }
    twoDimIntConv(in_arr, gradientX, sobelX, width, height);
    twoDimIntConv(in_arr, gradientY, sobelY, width, height);
}

void nonMaxSuppression(int *gradientX, int *gradientY, unsigned char *output, int width, int height) {
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int gx = gradientX[y*width + x];
            int gy = gradientY[y*width + x];
            float magnitude = sqrt(gx*gx + gy*gy);
            float direction = atan2(gy, gx) * 180 / M_PI;

            if (direction < 0) direction += 180;

            int x1, y1, x2, y2;
            if ((direction >= 0 && direction < 22.5) || (direction >= 157.5 && direction < 180)) {
                x1 = x2 = 0; y1 = -1; y2 = 1;
            } else if (direction >= 22.5 && direction < 67.5) {
                x1 = -1; y1 = 1; x2 = 1; y2 = -1;
            } else if (direction >= 67.5 && direction < 112.5) {
                x1 = -1; y1 = 0; x2 = 1; y2 = 0;
            } else {
                x1 = -1; y1 = -1; x2 = 1; y2 = 1;
            }

            float mag1 = sqrt(gradientX[(y+y1)*width + (x+x1)] * gradientX[(y+y1)*width + (x+x1)] +
                         gradientY[(y+y1)*width + (x+x1)] * gradientY[(y+y1)*width + (x+x1)]);
            float mag2 = sqrt(gradientX[(y+y2)*width + (x+x2)] * gradientX[(y+y2)*width + (x+x2)] +
                         gradientY[(y+y2)*width + (x+x2)] * gradientY[(y+y2)*width + (x+x2)]);

            if (magnitude > mag1 && magnitude > mag2) {
                output[y*width + x] = (unsigned char)(magnitude > 255 ? 255 : magnitude);
            } else {
                output[y*width + x] = 0;
            }
        }
    }
}

void doubleThreshold(unsigned char *input, unsigned char *output, int width, int height, int lowThreshold, int highThreshold) {
    for (int i = 0; i < width * height; i++) {
        if (input[i] >= highThreshold) {
            output[i] = 255;
        } else if (input[i] >= lowThreshold) {
            output[i] = 128;
        } else {
            output[i] = 0;
        }
    }
}

void hysteresis(unsigned char *input, unsigned char *output, int width, int height) {
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            if (input[y*width + x] == 128) {
                if (input[(y-1)*width + (x-1)] == 255 || input[(y-1)*width + x] == 255 ||
                    input[(y-1)*width + (x+1)] == 255 || input[y*width + (x-1)] == 255 ||
                    input[y*width + (x+1)] == 255 || input[(y+1)*width + (x-1)] == 255 ||
                    input[(y+1)*width + x] == 255 || input[(y+1)*width + (x+1)] == 255) {
                    output[y*width + x] = 255;
                } else {
                    output[y*width + x] = 0;
                }
            } else {
                output[y*width + x] = input[y*width + x];
            }
        }
    }
}

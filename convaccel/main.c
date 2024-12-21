#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_WIDTH 249
#define MAX_HEIGHT 391

// Function prototypes
void gaussianBlur(unsigned char *input, unsigned char *output, int width, int height);
void sobelOperator(unsigned char *input, int *gradientX, int *gradientY, int width, int height);
void nonMaxSuppression(int *gradientX, int *gradientY, unsigned char *output, int width, int height);
void doubleThreshold(unsigned char *input, unsigned char *output, int width, int height, int lowThreshold, int highThreshold);
void hysteresis(unsigned char *input, unsigned char *output, int width, int height);

int main() {
    int width, height;
    unsigned char input[MAX_WIDTH * MAX_HEIGHT];
    unsigned char output[MAX_WIDTH * MAX_HEIGHT];
    int gradientX[MAX_WIDTH * MAX_HEIGHT];
    int gradientY[MAX_WIDTH * MAX_HEIGHT];

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

void gaussianBlur(unsigned char *input, unsigned char *output, int width, int height) {
    
    float kernel[3][3] = {{1/16.0, 2/16.0, 1/16.0},
                          {2/16.0, 4/16.0, 2/16.0},
                          {1/16.0, 2/16.0, 1/16.0}};

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            float sum = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    sum += input[(y+ky)*width + (x+kx)] * kernel[ky+1][kx+1];
                }
            }
            output[y*width + x] = (unsigned char)sum;
        }
    }
}

void sobelOperator(unsigned char *input, int *gradientX, int *gradientY, int width, int height) {
    int sobelX[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int sobelY[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int gx = 0, gy = 0;
            for (int ky = -1; ky <= 1; ky++) {
                for (int kx = -1; kx <= 1; kx++) {
                    int pixel = input[(y+ky)*width + (x+kx)];
                    gx += pixel * sobelX[ky+1][kx+1];
                    gy += pixel * sobelY[ky+1][kx+1];
                }
            }
            gradientX[y*width + x] = gx;
            gradientY[y*width + x] = gy;
        }
    }
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

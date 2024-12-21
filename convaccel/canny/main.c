#include "canny.h"
#include <string.h>

// Function to read a PGM image
void readPGM(const char* filename, unsigned char* img) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        printf("Error opening file: %s\n", filename);
        return;
    }

    // Skip P5\n
    char magic[3];
    fscanf(file, "%s\n", magic);

    // Skip dimensions and max value
    int width, height, maxval;
    fscanf(file, "%d %d\n%d\n", &width, &height, &maxval);

    // Read image data
    if (fread(img, sizeof(unsigned char), WIDTH * HEIGHT, file) != WIDTH * HEIGHT) {
        printf("Error reading image data\n");
        fclose(file);
        return;
    }

    fclose(file);
}

// Function to write a PGM image
void writePGM(const char* filename, unsigned char* img) {
    FILE* file = fopen(filename, "wb");
    if (!file) {
        printf("Error creating file: %s\n", filename);
        return;
    }

    fprintf(file, "P5\n%d %d\n255\n", WIDTH, HEIGHT);
    fwrite(img, sizeof(unsigned char), WIDTH * HEIGHT, file);
    fclose(file);
}

int main(int argc, char** argv) {
    if (argc != 2) {
        printf("Usage: %s <input_image.pgm>\n", argv[0]);
        return 1;
    }

    // Declare arrays for all steps
    unsigned char input[WIDTH * HEIGHT] = {0};
    unsigned char blurred[WIDTH * HEIGHT] = {0};
    unsigned char gradientMagnitude[WIDTH * HEIGHT] = {0};
    unsigned char gradientDirection[WIDTH * HEIGHT] = {0};
    unsigned char nms[WIDTH * HEIGHT] = {0};
    unsigned char threshold[WIDTH * HEIGHT] = {0};
    unsigned char output[WIDTH * HEIGHT] = {0};
    int position[2] = {0};

    // Read input image
    readPGM(argv[1], input);

    // Apply Canny edge detection
    gaussianBlur(input, blurred);
    sobelOperator(blurred, gradientMagnitude, gradientDirection);
    nonMaxSuppression(gradientMagnitude, gradientDirection, nms);
    doubleThreshold(nms, threshold, 0.5, 0.25);  // High threshold = 0.2, Low threshold = 0.1
    hysteresis(threshold, output);
    subjectIdentification(output, position);
    int x = position[0];
    int y = position[1];

    printf("Subject at %d, %d \n", x, y);

    output[y * WIDTH + x] = 255;

    // Save output
    writePGM("output.pgm", output);

    printf("Edge detection complete. Result saved as 'output.pgm'\n");
    return 0;
}

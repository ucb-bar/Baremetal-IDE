#ifndef CANNY_H
#define CANNY_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define WIDTH 256
#define HEIGHT 256

// Function declarations
void gaussianBlur(unsigned char* src, 
                 unsigned char* dst);

void sobelOperator(unsigned char* src,
                  unsigned char* gradientMagnitude,
                  unsigned char* gradientDirection);

void nonMaxSuppression(unsigned char* gradientMagnitude,
                      unsigned char* gradientDirection,
                      unsigned char* nms);

void doubleThreshold(unsigned char* src,
                    unsigned char* dst,
                    float highThreshold, float lowThreshold);

void hysteresis(unsigned char* src,
                unsigned char* dst);

void subjectIdentification(unsigned char* src, int position[2]);

#endif // CANNY_H

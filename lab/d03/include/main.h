/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  */
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

/* Provided by glossy: busy-waits for the given number of milliseconds. */
int msleep(useconds_t msec);

void app_init();
void app_main();

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

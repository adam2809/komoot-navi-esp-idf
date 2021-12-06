#ifndef _MORSE_H
#define _MORSE_H

#include <stdio.h>

#define MAX_PASSWORD_LENGTH 10
#define TIME_BETWEEN_LETTERS 2000ULL
#define TIME_BETWEEN_WORDS 5000ULL

char bin_morse_2_char(uint8_t bin_morse,uint8_t len);
#endif
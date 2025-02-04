#include "common.h"

void putchar(char ch);

void printf(const char *fmt, ...) {
    va_list vargs;
    va_start(vargs, fmt);

    while(*fmt) {
        if(*fmt == '%') {
            fmt++; // skip '%'
            switch (*fmt) { // read the next char
                case '\0': // '%' at the end of the format string
                    putchar('%');
                    goto end;

                case '%': // print '%'
                    putchar('%');
                    break;

                case 's': { // print a null terminated string '%s'
                    const char *s = va_arg(vargs, const char *);
                    while(*s) {
                        putchar(*s);
                        s++;
                    }
                    break;
                }

                case 'd': { // print an integer in decimal
                    int value = va_arg(vargs, int);
                    if(value < 0) { // adds a negative sign if num < 0
                        putchar('-');
                        value = -value;
                    }

                    int divisor = 1;
                    while(value / divisor > 9) {
                        divisor *= 10;
                    }

                    while (divisor > 0) {
                        putchar('0' + value / divisor);
                        value %= divisor;
                        divisor /= 10;
                    }

                    break;
                }

                case 'x': { // print an int in hexadecimal
                    int value = va_arg(vargs, int);
                    for(int i = 7; i>= 0; i--) {
                        int nibble = (value >> (i * 4)) & 0xf;
                        putchar("0123456789abcdef"[nibble]);
                    }
                }
            }
        } else {
            putchar(*fmt);
        }

        fmt++;
    }
end: 
    va_end(vargs);
}
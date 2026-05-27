#ifndef DISPLAY_H
#define DISPLAY_H
#include <U8g2lib.h>

extern U8G2_SH1106_128X64_NONAME_F_HW_I2C display; 

void display_init();

void display_showText(const char* text);

#endif
#ifndef LCD_H
#define LCD_H

#include <stdint.h>

#define LCD_WIDTH 800
#define LCD_HEIGHT 480
#define LCD_BUF_SIZE (LCD_WIDTH * LCD_HEIGHT * 4)

extern int *plcd;

void Lcd_Set_Buffer(int *pbuf);
void Lcd_Draw_Point(int x, int y, int color);
void Lcd_Clear(int color);
void Lcd_Draw_Word(char word[], int w, int h, int x0, int y0, int color);
void Lcd_Show_Num(int x, int y, int num, int len, uint64_t color);
int show_bmp(char *bmp_path, int pic_x, int pic_y, int *pbuf);

#endif
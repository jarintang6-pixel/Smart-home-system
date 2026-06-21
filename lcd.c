#include "lcd.h"

#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int *lcd_buffer = NULL;

int *plcd = NULL;

static void draw_point(int x, int y, int color, int *pbuf)
{
    if (x >= 0 && x < LCD_WIDTH && y >= 0 && y < LCD_HEIGHT)
    {
        *(pbuf + LCD_WIDTH * y + x) = color;
    }
}

void Lcd_Set_Buffer(int *pbuf)
{
    lcd_buffer = pbuf;
    plcd = pbuf;
}

int show_bmp(char *bmp_path, int pic_x, int pic_y, int *pbuf)
{
    int bmp_fd = open(bmp_path, O_RDONLY);
    if (bmp_fd == -1)
    {
        perror("open bmp fail");
        return -1;
    }

    int width = 0;
    lseek(bmp_fd, 0x12, SEEK_SET);
    read(bmp_fd, &width, 4);

    int height = 0;
    lseek(bmp_fd, 0x16, SEEK_SET);
    read(bmp_fd, &height, 4);

    short depth = 0;
    lseek(bmp_fd, 0x1C, SEEK_SET);
    read(bmp_fd, &depth, 2);

    int lin_effctive_bytes = abs(width) * (depth / 8);
    int laizi = 0;
    if (lin_effctive_bytes % 4 != 0)
    {
        laizi = 4 - lin_effctive_bytes % 4;
    }
    int lin_total_bytes = lin_effctive_bytes + laizi;
    int toatl_bytes = lin_total_bytes * abs(height);

    unsigned char *piex_arr = malloc(toatl_bytes);
    if (piex_arr == NULL)
    {
        close(bmp_fd);
        return -1;
    }

    lseek(bmp_fd, 0x36, SEEK_SET);
    read(bmp_fd, piex_arr, toatl_bytes);

    int a, r, g, b;
    int i = 0;
    int pic_h = abs(height);
    int pic_w = abs(width);
    for (int y = 0; y < pic_h; y++)
    {
        for (int x = 0; x < pic_w; x++)
        {
            b = piex_arr[i++];
            g = piex_arr[i++];
            r = piex_arr[i++];
            if (depth == 32)
            {
                a = piex_arr[i++];
            }
            else
            {
                a = 0xff;
            }
            int color = (a << 24) | (r << 16) | (g << 8) | b;
            draw_point(pic_x + x, pic_y + (pic_h - 1 - y), color, pbuf);
        }
        i = i + laizi;
    }

    free(piex_arr);
    close(bmp_fd);
    return 0;
}

void Lcd_Draw_Point(int x, int y, int color)
{
    if (lcd_buffer == NULL)
    {
        return;
    }
    draw_point(x, y, color, lcd_buffer);
}

void Lcd_Clear(int color)
{
    if (lcd_buffer == NULL)
    {
        return;
    }

    for (int i = 0; i < LCD_WIDTH * LCD_HEIGHT; i++)
    {
        lcd_buffer[i] = color;
    }
}

void Lcd_Draw_Word(char word[], int w, int h, int x0, int y0, int color)
{
    int i, j;
    int x, y;
    int count = 0;
    for (i = 0; i < w * h / 8; i++)
    {
        for (j = 0; j < 8; j++)
        {
            if ((((unsigned char)word[i]) & (0x01 << (7 - j))) != 0)
            {
                x = x0 + count % w;
                y = y0 + count / w;
                Lcd_Draw_Point(x, y, color);
            }
            count++;
        }
    }
}

void Lcd_Show_Num(int x, int y, int num, int len, uint64_t color)
{
    for (int i = 0; i < len; i++)
    {
        int digit = (num / (int)pow(10, len - 1 - i)) % 10;
        Lcd_Draw_Word(number[digit], 32, 64, x + i * 32, y, color);
    }
}
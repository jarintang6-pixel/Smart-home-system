#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>

#include "word.h"
#include "lcd.h"
#include "uart.h"

#ifndef FONT_DATA_ONLY
int main(void)
{
    //打开LCD屏幕设备文件
    int lcd_fd = open("/dev/fb0", O_RDWR);
    if(lcd_fd == -1)
    {
        perror("open lcd /dev/fb0 fail");
        return -1;
    }

   //内存映射屏幕显存
    plcd = mmap(NULL, LCD_BUF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, lcd_fd, 0);
    if(plcd == MAP_FAILED)
    {
        perror("mmap lcd fail");
        close(lcd_fd);
        return -1;
    }
    // 设置LCD显示缓冲区
    Lcd_Set_Buffer(plcd);
    // 清屏，白色背景
    Lcd_Clear(0xFFFFFF); 
    // 显示背景图片
    show_bmp("bg.bmp", 0, 0, plcd); 

    // Lcd_Draw_Word(zhi[0], 64, 64, 256, 0, 0x0000ff);  // 显示"智"
    // Lcd_Draw_Word(neng[0], 64, 64, 320, 0, 0x0000ff); // 显示"能"    
    // Lcd_Draw_Word(jia[0], 64, 64, 384, 0, 0x0000ff);  // 显示"家"
    // Lcd_Draw_Word(ju[0], 64, 64, 448, 0, 0x0000ff);   // 显示"居"

    Lcd_Draw_Word(wen[0], 64, 64, 0, 64, 0xffffff);      // 显示"温"
    Lcd_Draw_Word(du[0], 64, 64, 64, 64, 0xffffff);      // 显示"度"
    Lcd_Draw_Word(shi[0], 64, 64, 0, 128, 0xffffff);     // 显示"湿"
    Lcd_Draw_Word(du[0], 64, 64, 64, 128, 0xffffff);     // 显示"度"
    Lcd_Draw_Word(hai[0], 64, 64, 0, 192, 0xffffff);     // 显示"海"
    Lcd_Draw_Word(ba[0], 64, 64, 64, 192, 0xffffff);     // 显示"拔"
    Lcd_Draw_Word(qi[0], 64, 64, 0, 256, 0xffffff);      // 显示"气"
    Lcd_Draw_Word(ya[0], 64, 64, 64, 256, 0xffffff);     // 显示"压"

    // 初始化UART串口通信（/dev/ttySAC1）
    int uart_fd = uart_init("/dev/ttySAC1");
    if(uart_fd == -1)
    {
        munmap(plcd, LCD_BUF_SIZE);
        close(lcd_fd);
        return -1;
    }
    char cmd1[3] = {0xa5,0x82,0x27};
    write(uart_fd, cmd1, 3);
    usleep(100000); // 等待设备响应，延迟100ms

    // 定义变量存储传感器数据
    double T, PA, Hum;    // 温度、气压、湿度
    int H;                // 海拔高度
    char buf2[15] = {0};  // 接收数据缓冲区
    while(1)
    {
    read_data:
        read(uart_fd, buf2, 15);
        // buf1[0] = 0x5a;
        // buf1[1] = 0x5a;
        // buf1[2] = 0x45;

        int temp = 0;
        // 解析温度数据（字节4-5，单位：0.01℃）
        temp = buf2[4] << 8 | buf2[5];
        T = (double)temp / 100;
        
        // 解析气压数据（字节6-9，单位：0.00001MPa）
        temp = buf2[6] << 24 | buf2[7] << 16 | buf2[8] << 8 | buf2[9];
        PA = (double)temp / 100000;
        
        // 解析湿度数据（字节10-11，单位：0.01%）
        temp = buf2[10] << 8 | buf2[11];
        Hum = (double)temp / 100;
        
        // 解析海拔高度数据（字节12-13，单位：米）
        temp = buf2[12] << 8 | buf2[13];
        H = temp;

        if(T == 0 || PA == 0 || Hum == 0 || H == 0)
        {
            sleep(1);
            goto read_data;
        }
            
        int T_int = (int)(T * 100);
        int T_int_part = T_int / 100;
        int T_dec_part = T_int % 100;

        // 清除温度显示区域（用黑色空格覆盖）
        for(int i = 0; i < 5; i++)
        {
            Lcd_Draw_Word(space[0], 64, 64, 160 + i * 32, 64, 0x000000);
        }
        
         // 显示温度数值
        Lcd_Show_Num(160, 64, T_int_part, 2, 0xffffff);           // 显示温度整数部分
        Lcd_Draw_Word(point[0], 32, 64, 160 + 64, 64, 0xffffff);  // 显示小数点
        Lcd_Show_Num(160 + 64 + 32, 64, T_dec_part, 2, 0xffffff); // 显示温度小数部分
        Lcd_Draw_Word(t_d[0], 64, 64, 160 + 6 * 32, 64, 0xffffff);// 显示"℃"

        // 湿度数据处理：分离整数和小数部分
        int Hum_int = (int)(Hum * 100);
        int Hum_int_part = Hum_int / 100;
        int Hum_dec_part = Hum_int % 100;

        // 清除湿度显示区域
        for(int i = 0; i < 5; i++)
        {
            Lcd_Draw_Word(space[0], 64, 64, 160 + i * 32, 128, 0x000000);
        }

        // 显示湿度数值
        Lcd_Show_Num(160, 128, Hum_int_part, 2, 0xffffff);           // 显示湿度整数部分
        Lcd_Draw_Word(point[0], 32, 64, 160 + 64, 128, 0xffffff);    // 显示小数点
        Lcd_Show_Num(160 + 64 + 32, 128, Hum_dec_part, 2, 0xffffff); // 显示湿度小数部分
        Lcd_Draw_Word(hu_d[0], 32, 64, 160 + 6 * 32, 128, 0xffffff); // 显示"%"

        // 显示海拔高度
        for(int i = 0; i < 5; i++)
        {
            Lcd_Draw_Word(space[0], 64, 64, 160 + i * 32, 192, 0x000000);
        }
        Lcd_Show_Num(160, 192, H, 4, 0xffffff); // 鏄剧ず楂樺害鏁存暟閮ㄥ垎
        Lcd_Draw_Word(h_d[0], 32, 64, 160 + 6 * 32, 192, 0xffffff); // 鏄剧ず"m"

        // 气压数据处理
        int PA_int = (int)(PA * 100);
        int PA_int_part = PA_int / 100;
        int PA_dec_part = PA_int % 100;

        // 清除气压显示区域
        for(int i = 0; i < 5; i++)
        {
            Lcd_Draw_Word(space[0], 64, 64, 160 + i * 32, 256, 0x000000);
        }
        // 显示气压数值
        Lcd_Show_Num(160, 256, PA_int_part, 2, 0xffffff);           // 显示气压整数部分
        Lcd_Draw_Word(point[0], 32, 64, 160 + 64, 256, 0xffffff);   // 显示小数点
        Lcd_Show_Num(160 + 64 + 32, 256, PA_dec_part, 2, 0xffffff); // 显示气压小数部分
        Lcd_Draw_Word(h_d[0], 32, 64, 160 + 6 * 32, 256, 0xffffff); // 显示"M"
        Lcd_Draw_Word(p_d[0], 32, 64, 160 + 7 * 32, 256, 0xffffff); // 显示"Pa"

         // 在终端打印传感器数据用于调试
        printf("T: %.2f C\n", T);
        printf("PA: %.2f MP\n", PA);
        printf("Humidity: %.2f %%\n", Hum);
        printf("Height: %d m\n", H);
        sleep(1);
    }
    

    //关闭设备
    munmap(plcd, LCD_BUF_SIZE);
    close(lcd_fd);
    close(uart_fd);
    return 0;
}


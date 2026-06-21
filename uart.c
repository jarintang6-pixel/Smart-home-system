#include "uart.h"

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

int uart_init(const char *uart_name)
{
    int uart1_fd = open(uart_name, O_RDWR);
    if (uart1_fd == -1)
    {
        perror("open error:");
        return -1;
    }
    else
    {
        printf("open %s success\n", uart_name);
    }

    struct termios myserial;
    memset(&myserial, 0, sizeof(myserial));
    myserial.c_cflag |= (CLOCAL | CREAD);
    myserial.c_cflag &= ~CSIZE;
    myserial.c_cflag &= ~CRTSCTS;
    myserial.c_cflag |= CS8;
    myserial.c_cflag &= ~CSTOPB;
    myserial.c_cflag &= ~PARENB;
    cfsetospeed(&myserial, B9600);
    cfsetispeed(&myserial, B9600);
    tcflush(uart1_fd, TCIFLUSH);
    tcsetattr(uart1_fd, TCSANOW, &myserial);
    return uart1_fd;
}
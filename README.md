# 智能家居环境显示程序

这是一个基于 Linux framebuffer 和串口传感器的智能家居显示程序。

## 模块结构

- `smart_home.c`：主流程，负责屏幕初始化、串口读取和界面刷新
- `lcd.c` / `lcd.h`：LCD 显示、画点、BMP 和字模绘制
- `uart.c` / `uart.h`：串口初始化
- `word.c` / `word.h`：字模数据封装

## 构建

在 Linux 或交叉编译环境下执行：

```bash
make
```

生成可执行文件 `smart_home`。

## 运行前提

- 设备节点 `/dev/fb0` 可用
- 设备节点 `/dev/ttySAC1` 可用
- 当前目录下存在 `bg.bmp`

## 清理

```bash
make clean
```
# 智能家居系统程序

这是基于GEC6818的智能家居系统，通过GY93不断检测温度、湿度、气压、海拔数据，并在LCD上不断更新。

## 模块结构

- `smart_home.c`：主流程，负责屏幕初始化、串口读取和界面刷新
- `lcd.c` / `lcd.h`：LCD 显示、画点、BMP 和字模绘制
- `uart.c` / `uart.h`：串口初始化
- `word.c` / `word.h`：字模数据封装

## 构建

在 Linux 或交叉编译环境下执行：

arm-linux-gcc smart_home.c -o xxx.out -lm

生成可执行文件 `xxx.out`。

## 运行前提

- 设备节点 `/dev/fb0` 可用
- 设备节点 `/dev/ttySAC1` 可用
- 当前目录下存在 `bg.bmp`(可自行在网上搜索图片进行更换，需将分辨率修改为800*480，否则可能报错segmentation fault）

## 清理

clear

项目概述<br>
本项目是一个关于微型机器人的开发项目，结合了 SPI 通信、GPIO/PWM 控制以及 WiFi TCP 通信等功能，同时配备了 1.28 英寸的 LCD 显示屏用于显示信息。以下是对项目中各个文件的详细介绍。
文件列表及功能说明：<br><br>
源文件<br>
1.micro_robot.c<br>
功能：该文件是项目的核心文件，将 SPI 示例、GPIO/PWM 控制和 WiFi TCP 示例代码进行了整合，实现了微型机器人的主要功能。
详细内容：<br>
定义了多个任务的堆栈大小和优先级，包括 GPIO 任务、PWM 任务、SPI 任务和 WiFi 任务。<br>
实现了 GPIO、PWM、SPI 和 WiFi 功能的初始化和任务创建。<br>
包含了处理接收到的控制命令的函数 process_command，可以根据不同的命令控制 GPIO 电平、运行 LCD 测试等。<br>
2.LCD_1inch28.c<br>
功能：提供了 1.28 英寸 LCD 显示屏的硬件底层接口，用于屏蔽不同主控的底层差异，增强了代码的可移植性。<br>
详细内容：<br>
实现了 LCD 的硬件复位、发送命令和数据的函数。<br>
包含了初始化 LCD 寄存器和设置屏幕分辨率及扫描方向的函数。<br>
3.LCD_1inch28_test.c<br>
功能：包含了一些用于测试 LCD 显示屏的函数，例如显示特定图像等。<br>
详细内容：实现了 heiche 函数，用于在 LCD 上绘制黑色车辆的图像。<br>
4.image.c<br>
功能：可能包含了项目中使用的图像数据，为 LCD 显示提供图像资源。<br>
5.GUI_Paint.c<br>
功能：提供了图形绘制的相关函数，用于在 LCD 屏幕上绘制各种图形和文字。<br>
6.font*.c（如 font16.c）<br>
功能：包含了不同字体的字模数据，为在 LCD 上显示文字提供了字体支持。<br>
7.DEV_Config.c<br>
功能：实现了 SPI 数据写入的函数，用于与外部设备进行 SPI 通信。<br>
8.wifi_connect.c<br>
功能：实现了 WiFi 连接的相关功能，可能包括连接到热点、处理 WiFi 事件等。<br><br>
头文件<br>
9.LCD_1inch28_test.h<br>
功能：声明了一些用于 LCD 测试的函数，方便其他文件调用。<br><br>
配置文件<br>
10.CMakeLists.txt<br>
功能：用于配置项目的编译信息，指定了源文件列表和公共头文件路径。<br><br>
UI文件<br>
11.lcd_control<br>
功能：构建UI界面，实现人机交互功能。<br><br>
项目结构<br>
plaintext<br>
micro_robot_1.5/<br>
├── micro_robot.c<br>
├── LCD_1inch28.c<br>
├── LCD_1inch28_test.c<br>
├── image.c<br>
├── GUI_Paint.c<br>
├── font24CN.c<br>
├── font24.c<br>
├── font20.c<br>
├── font16.c<br>
├── font12CN.c<br>
├── font12.c<br>
├── font8.c<br>
├── DEV_Config.c<br>
├── wifi_connect.c<br>
├── LCD_1inch28_test.h<br>
├── CMakeLists.txt<br>
lcd_control<br>
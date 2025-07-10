/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2024. All rights reserved.
 *
 * Description: Combine SPI Sample Source, GPIO/PWM control and WiFi TCP sample for micro robot. \n
 *
 * History: \n
 * 2023-06-25, Create file for SPI sample. \n
 * 2025-04-26, Add GPIO and PWM control code. \n
 * 2024-XX-XX, Merge wifi_tcp_sample.c into micro_robot.c. \n
 */
#include "pinctrl.h"
#include "soc_osal.h"
#include "spi.h"
#include "gpio.h"
#include "pwm.h"
#include "osal_debug.h"
#include "cmsis_os2.h"
#include "app_init.h"
#include "GUI_Paint.h"
#include "fonts.h"
#include "image.h"
#include "LCD_1inch28.h"
#include "LCD_1inch28_test.h"
#include "tcxo.h"
#include "lwip/netifapi.h"
#include "wifi_hotspot.h"
#include "wifi_hotspot_config.h"
#include "stdlib.h"
#include "uart.h"
#include "lwip/nettool/misc.h"
#include "wifi_connect.h"
#include "wifi_event.h"
#include "lwip/sockets.h"
#include "lwip/ip4_addr.h"

#define GPIO_TASK_STACK_SIZE    0x1000
#define GPIO_TASK_PRIO          (osPriority_t)(17)

#define PWM_GPIO_5               5
#define PWM_GPIO_0               0
#define PWM_PIN_MODE             1
#define PWM_CHANNEL_5            5
#define PWM_CHANNEL_0            0
#define PWM_GROUP_ID_5           1
#define PWM_GROUP_ID_0           2
#define TEST_MAX_TIMES           5
#define DALAY_MS                 100
#define TEST_TCXO_DELAY_1000MS   1000
#define PWM_TASK_STACK_SIZE      0x1000
#define PWM_TASK_PRIO            (osPriority_t)(17)

#define SPI_SLAVE_NUM                   1
#define SPI_FREQUENCY                   2
#define SPI_CLK_POLARITY                1
#define SPI_CLK_PHASE                   1
#define SPI_FRAME_FORMAT                0
#define SPI_FRAME_FORMAT_STANDARD       0
#define SPI_FRAME_SIZE_8                0x1f
#define SPI_TMOD                        0
#define SPI_WAIT_CYCLES                 0x10

#define SPI_TASK_STACK_SIZE             0x1000
#define SPI_TASK_DURATION_MS            1000
#define SPI_TASK_PRIO                   (osPriority_t)(17)

#define WIFI_TASK_STACK_SIZE 0x2000
#define DELAY_TIME_MS 100

static const char *SEND_DATA = "TCP Test!\r\n";

static uint8_t channel_id_5;

static uint8_t channel_id_0;

static errcode_t pwm_sample_callback(uint8_t channel)
{
    osal_printk("PWM %d, cycle done. \r\n", channel);
    return ERRCODE_SUCC;
}

static void *gpio_task(const char *arg)
{
    UNUSED(arg);
    // 设置 GPIO1 复用模式为 GPIO
    uapi_pin_set_mode(CONFIG_GPIO_1, HAL_PIO_FUNC_GPIO);
    // 设置 GPIO2 复用模式为 GPIO
    uapi_pin_set_mode(CONFIG_GPIO_2, HAL_PIO_FUNC_GPIO);
    // 设置 GPIO3 复用模式为 GPIO
    uapi_pin_set_mode(CONFIG_GPIO_13, HAL_PIO_FUNC_GPIO);
    // 设置 GPIO4 复用模式为 GPIO
    uapi_pin_set_mode(CONFIG_GPIO_14, HAL_PIO_FUNC_GPIO);

    // 设置 GPIO1 方向为输出方向
    uapi_gpio_set_dir(CONFIG_GPIO_1, GPIO_DIRECTION_OUTPUT);
    // 设置 GPIO2 方向为输出方向
    uapi_gpio_set_dir(CONFIG_GPIO_2, GPIO_DIRECTION_OUTPUT);
    // 设置 GPIO3 方向为输出方向
    uapi_gpio_set_dir(CONFIG_GPIO_13, GPIO_DIRECTION_OUTPUT);
    // 设置 GPIO4 方向为输出方向
    uapi_gpio_set_dir(CONFIG_GPIO_14, GPIO_DIRECTION_OUTPUT);

    // 设置 GPIO1 电平为高电平
    uapi_gpio_set_val(CONFIG_GPIO_1, GPIO_LEVEL_LOW);
    // 设置 GPIO2 电平为低电平
    uapi_gpio_set_val(CONFIG_GPIO_2, GPIO_LEVEL_LOW);
    // 设置 GPIO3 电平为高电平
    uapi_gpio_set_val(CONFIG_GPIO_13, GPIO_LEVEL_LOW);
    // 设置 GPIO4 电平为低电平
    uapi_gpio_set_val(CONFIG_GPIO_14, GPIO_LEVEL_LOW);

    while (1) {
        osal_msleep(500);
    }

    return NULL;
}

static void *pwm_task(const char *arg)
{
    UNUSED(arg);
    pwm_config_t cfg_no_repeat = {
        50,     // 低电平
        200,    // 高电平
        0,
        0,
        true
    };

    // 配置引脚 5 的 PWM
    uapi_pin_set_mode(PWM_GPIO_5, PWM_PIN_MODE);
    uapi_pwm_deinit();
    uapi_pwm_init();
    uapi_pwm_open(PWM_CHANNEL_5, &cfg_no_repeat);

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
    uapi_pwm_unregister_interrupt(PWM_CHANNEL_5);
    uapi_pwm_register_interrupt(PWM_CHANNEL_5, pwm_sample_callback);

    channel_id_5 = PWM_CHANNEL_5;
    uapi_pwm_set_group(PWM_GROUP_ID_5, &channel_id_5, 1);
    uapi_pwm_start_group(PWM_GROUP_ID_5);

    // 配置引脚 0 的 PWM
    uapi_pin_set_mode(PWM_GPIO_0, PWM_PIN_MODE);
    uapi_pwm_open(PWM_CHANNEL_0, &cfg_no_repeat);

    uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
    uapi_pwm_unregister_interrupt(PWM_CHANNEL_0);
    uapi_pwm_register_interrupt(PWM_CHANNEL_0, pwm_sample_callback);

    channel_id_0 = PWM_CHANNEL_0;
    uapi_pwm_set_group(PWM_GROUP_ID_0, &channel_id_0, 1);
    uapi_pwm_start_group(PWM_GROUP_ID_0);

    while (1) {
        osal_msleep(500);
    }

    return NULL;
}

static void gpio_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "GPIOTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = GPIO_TASK_STACK_SIZE;
    attr.priority = GPIO_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)gpio_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

static void pwm_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "PWMTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = PWM_TASK_STACK_SIZE;
    attr.priority = PWM_TASK_PRIO;

    if (osThreadNew((osThreadFunc_t)pwm_task, NULL, &attr) == NULL) {
        /* Create task fail. */
    }
}

static void app_spi_init_pin(void)
{

    if (CONFIG_SPI_MASTER_BUS_ID == 0) {
        uapi_pin_set_mode(CONFIG_SPI_DO_MASTER_PIN, 3);
        uapi_pin_set_mode(CONFIG_SPI_CLK_MASTER_PIN, 3);
        uapi_pin_set_mode(CONFIG_SPI_CS_MASTER_PIN, 0);
        uapi_gpio_set_dir(CONFIG_SPI_CS_MASTER_PIN, GPIO_DIRECTION_OUTPUT);
        uapi_gpio_set_val(CONFIG_SPI_CS_MASTER_PIN, GPIO_LEVEL_HIGH);
        uapi_pin_set_mode(CONFIG_OLED_RES_PIN, 0);       
        uapi_gpio_set_dir(CONFIG_OLED_RES_PIN, GPIO_DIRECTION_OUTPUT);
        uapi_gpio_set_val(CONFIG_OLED_RES_PIN, GPIO_LEVEL_HIGH);
        uapi_pin_set_mode(CONFIG_OLED_DC_PIN, 0);
        uapi_gpio_set_dir(CONFIG_OLED_DC_PIN, GPIO_DIRECTION_OUTPUT);
        uapi_gpio_set_val(CONFIG_OLED_DC_PIN, GPIO_LEVEL_HIGH);  
        uapi_pin_set_ds(CONFIG_SPI_CLK_MASTER_PIN, PIN_DS_2);      
    }
}
static void app_spi_master_init_config(void)
{
    spi_attr_t config = { 0 };
    spi_extra_attr_t ext_config = { 0 };

    config.is_slave = false;
    config.slave_num = SPI_SLAVE_NUM;
    config.bus_clk = 14400000;
    config.freq_mhz = SPI_FREQUENCY;
    config.clk_polarity = SPI_CLK_POLARITY;
    config.clk_phase = SPI_CLK_PHASE;
    config.frame_format = SPI_FRAME_FORMAT;
    config.spi_frame_format = HAL_SPI_FRAME_FORMAT_STANDARD;
    config.frame_size = HAL_SPI_FRAME_SIZE_8;
    config.tmod = SPI_TMOD;
    config.sste = 1;

    ext_config.qspi_param.wait_cycles = SPI_WAIT_CYCLES;
    uapi_spi_init(CONFIG_SPI_MASTER_BUS_ID, &config, &ext_config);
}

static void *spi_master_task(const char *arg)
{
    unused(arg);
    /* SPI pinmux. */
    app_spi_init_pin();

    /* SPI master init config. */
    app_spi_master_init_config();
    
    while (1) {
        osal_msleep(500);
    }


    return NULL;
}

static void spi_master_entry(void)
{
    osThreadAttr_t attr;

    attr.name = "SpiMasterTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = SPI_TASK_STACK_SIZE;
    attr.priority = SPI_TASK_PRIO;
    printf("LCD_1IN28_test Demo0\r\n");
    if (osThreadNew((osThreadFunc_t)spi_master_task, NULL, &attr) == NULL) {
        /* Create task fail. */
        printf("[GyroExample] Failed to create LCDTask!\n");
    }
    printf("LCD_1IN28_test Demo1\r\n");
}

static void wifi_scan_state_changed(td_s32 state, td_s32 size)
{
    UNUSED(state);
    UNUSED(size);
    printf("Scan done!\r\n");
    return;
}

static void wifi_connection_changed(td_s32 state, const wifi_linked_info_stru *info, td_s32 reason_code)
{
    UNUSED(reason_code);

    if (state == WIFI_STATE_AVALIABLE)
        printf("[WiFi]:%s, [RSSI]:%d\r\n", info->ssid, info->rssi);
}

/**
 * @brief 处理接收到的控制命令
 * @param cmd 接收到的命令字符
 */
static void process_command(char cmd)
{
    pwm_config_t low_wheel_cfg = {
        100,     // 低电平
        150,    // 高电平
        0,
        0,
        true
    };

    pwm_config_t high_wheel_cfg = {
        50,     // 低电平
        200,    // 高电平
        0,
        0,
        true
    };

    switch(cmd) {
        case '0':
            printf("Running LCD_1in28_test...\n");
            LCD_1in28_test();
            printf("LCD_1in28_test completed.\n");
            break;
        case '2':
            uapi_gpio_set_val(CONFIG_GPIO_1, GPIO_LEVEL_LOW);
            uapi_gpio_set_val(CONFIG_GPIO_2, GPIO_LEVEL_LOW);
            printf("GPIO1 turned low\n");
            break;
        case '3':
            uapi_gpio_set_val(CONFIG_GPIO_13, GPIO_LEVEL_LOW);
            uapi_gpio_set_val(CONFIG_GPIO_14, GPIO_LEVEL_LOW);
            printf("GPIO3 turned low\n");
            break;
        case '4':
            uapi_gpio_set_val(CONFIG_GPIO_1, GPIO_LEVEL_HIGH);
            uapi_gpio_set_val(CONFIG_GPIO_2, GPIO_LEVEL_LOW);
            printf("GPIO1 turned high\n");
            break;
        case '5':
            uapi_gpio_set_val(CONFIG_GPIO_13, GPIO_LEVEL_HIGH);
            uapi_gpio_set_val(CONFIG_GPIO_14, GPIO_LEVEL_LOW);
            printf("GPIO13 turned high\n");
            break;
        case '6':
            heiche();
            hei();
            break;
        case '7':
            heima();
            hei();
            break;
        case '8':
            heipao();
            hei();
            break;
        case '9':
            heixiang();
            hei();
            break;
        case 'a':
            heizu();
            hei();
            break;
        case 'b':
            heishi();
            hei();
            break;
        case 'c':
            heijiang();
            hei();
            break;
        case 'd':
            heiche();
            break;
        case 'e':
            heima();
            hong();
            break;
        case 'f':
            heipao();
            hong();
            break;
        case 'g':
            heixiang();
            hong();
            break;
        case 'h':
            heizu();
            hong();
            break;
        case 'i':
            heishi();
            hong();
            break;
        case 'j':
            heijiang();
            hong();
            break;
        case 'k':// 前进

            uapi_pwm_stop_group(PWM_GROUP_ID_5);
            uapi_pwm_clear_group(PWM_GROUP_ID_5);
            uapi_pwm_close(PWM_CHANNEL_5);      
            uapi_pwm_open(PWM_CHANNEL_5, &high_wheel_cfg);
            uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
            uapi_pwm_register_interrupt(PWM_CHANNEL_5, pwm_sample_callback);
            uapi_pwm_set_group(PWM_GROUP_ID_5, &channel_id_5, 1);
            uapi_pwm_start_group(PWM_GROUP_ID_5);

            uapi_pwm_stop_group(PWM_GROUP_ID_0);
            uapi_pwm_clear_group(PWM_GROUP_ID_0);
            uapi_pwm_close(PWM_CHANNEL_0);      
            uapi_pwm_open(PWM_CHANNEL_0, &high_wheel_cfg);
            uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
            uapi_pwm_register_interrupt(PWM_CHANNEL_0, pwm_sample_callback);
            uapi_pwm_set_group(PWM_GROUP_ID_0, &channel_id_0, 1);
            uapi_pwm_start_group(PWM_GROUP_ID_0);

            uapi_gpio_set_val(CONFIG_GPIO_1, GPIO_LEVEL_HIGH);
            uapi_gpio_set_val(CONFIG_GPIO_2, GPIO_LEVEL_LOW);
            uapi_gpio_set_val(CONFIG_GPIO_13, GPIO_LEVEL_HIGH);
            uapi_gpio_set_val(CONFIG_GPIO_14, GPIO_LEVEL_LOW);
            break;
        case 'l':// 后退

            uapi_pwm_stop_group(PWM_GROUP_ID_5);
            uapi_pwm_clear_group(PWM_GROUP_ID_5);
            uapi_pwm_close(PWM_CHANNEL_5);      
            uapi_pwm_open(PWM_CHANNEL_5, &high_wheel_cfg);
            uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
            uapi_pwm_register_interrupt(PWM_CHANNEL_5, pwm_sample_callback);
            uapi_pwm_set_group(PWM_GROUP_ID_5, &channel_id_5, 1);
            uapi_pwm_start_group(PWM_GROUP_ID_5);

            uapi_pwm_stop_group(PWM_GROUP_ID_0);
            uapi_pwm_clear_group(PWM_GROUP_ID_0);
            uapi_pwm_close(PWM_CHANNEL_0);      
            uapi_pwm_open(PWM_CHANNEL_0, &high_wheel_cfg);
            uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
            uapi_pwm_register_interrupt(PWM_CHANNEL_0, pwm_sample_callback);
            uapi_pwm_set_group(PWM_GROUP_ID_0, &channel_id_0, 1);
            uapi_pwm_start_group(PWM_GROUP_ID_0);

            uapi_gpio_set_val(CONFIG_GPIO_1, GPIO_LEVEL_LOW);
            uapi_gpio_set_val(CONFIG_GPIO_2, GPIO_LEVEL_HIGH);
            uapi_gpio_set_val(CONFIG_GPIO_13, GPIO_LEVEL_LOW);
            uapi_gpio_set_val(CONFIG_GPIO_14, GPIO_LEVEL_HIGH);
            break;
        case 'm':// 左前

            uapi_pwm_stop_group(PWM_GROUP_ID_5);
            uapi_pwm_clear_group(PWM_GROUP_ID_5);
            uapi_pwm_close(PWM_CHANNEL_5);      
            uapi_pwm_open(PWM_CHANNEL_5, &low_wheel_cfg);
            uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
            uapi_pwm_register_interrupt(PWM_CHANNEL_5, pwm_sample_callback);
            uapi_pwm_set_group(PWM_GROUP_ID_5, &channel_id_5, 1);
            uapi_pwm_start_group(PWM_GROUP_ID_5);

            uapi_pwm_stop_group(PWM_GROUP_ID_0);
            uapi_pwm_clear_group(PWM_GROUP_ID_0);
            uapi_pwm_close(PWM_CHANNEL_0);      
            uapi_pwm_open(PWM_CHANNEL_0, &high_wheel_cfg);
            uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
            uapi_pwm_register_interrupt(PWM_CHANNEL_0, pwm_sample_callback);
            uapi_pwm_set_group(PWM_GROUP_ID_0, &channel_id_0, 1);
            uapi_pwm_start_group(PWM_GROUP_ID_0);        

            uapi_gpio_set_val(CONFIG_GPIO_1, GPIO_LEVEL_HIGH);
            uapi_gpio_set_val(CONFIG_GPIO_2, GPIO_LEVEL_LOW);
            uapi_gpio_set_val(CONFIG_GPIO_13, GPIO_LEVEL_HIGH);
            uapi_gpio_set_val(CONFIG_GPIO_14, GPIO_LEVEL_LOW);
            break;
        case 'n':// 右前

            uapi_pwm_stop_group(PWM_GROUP_ID_5);
            uapi_pwm_clear_group(PWM_GROUP_ID_5);
            uapi_pwm_close(PWM_CHANNEL_5);      
            uapi_pwm_open(PWM_CHANNEL_5, &high_wheel_cfg);
            uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
            uapi_pwm_register_interrupt(PWM_CHANNEL_5, pwm_sample_callback);
            uapi_pwm_set_group(PWM_GROUP_ID_5, &channel_id_5, 1);
            uapi_pwm_start_group(PWM_GROUP_ID_5);

            uapi_pwm_stop_group(PWM_GROUP_ID_0);
            uapi_pwm_clear_group(PWM_GROUP_ID_0);
            uapi_pwm_close(PWM_CHANNEL_0);      
            uapi_pwm_open(PWM_CHANNEL_0, &low_wheel_cfg);
            uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
            uapi_pwm_register_interrupt(PWM_CHANNEL_0, pwm_sample_callback);
            uapi_pwm_set_group(PWM_GROUP_ID_0, &channel_id_0, 1);
            uapi_pwm_start_group(PWM_GROUP_ID_0);

            uapi_gpio_set_val(CONFIG_GPIO_1, GPIO_LEVEL_HIGH);
            uapi_gpio_set_val(CONFIG_GPIO_2, GPIO_LEVEL_LOW);
            uapi_gpio_set_val(CONFIG_GPIO_13, GPIO_LEVEL_HIGH);
            uapi_gpio_set_val(CONFIG_GPIO_14, GPIO_LEVEL_LOW);
            break;
        case 'o':// 左后

            uapi_pwm_stop_group(PWM_GROUP_ID_5);
            uapi_pwm_clear_group(PWM_GROUP_ID_5);
            uapi_pwm_close(PWM_CHANNEL_5);      
            uapi_pwm_open(PWM_CHANNEL_5, &low_wheel_cfg);
            uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
            uapi_pwm_register_interrupt(PWM_CHANNEL_5, pwm_sample_callback);
            uapi_pwm_set_group(PWM_GROUP_ID_5, &channel_id_5, 1);
            uapi_pwm_start_group(PWM_GROUP_ID_5);

            uapi_pwm_stop_group(PWM_GROUP_ID_0);
            uapi_pwm_clear_group(PWM_GROUP_ID_0);
            uapi_pwm_close(PWM_CHANNEL_0);      
            uapi_pwm_open(PWM_CHANNEL_0, &high_wheel_cfg);
            uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
            uapi_pwm_register_interrupt(PWM_CHANNEL_0, pwm_sample_callback);
            uapi_pwm_set_group(PWM_GROUP_ID_0, &channel_id_0, 1);
            uapi_pwm_start_group(PWM_GROUP_ID_0);

            uapi_gpio_set_val(CONFIG_GPIO_1, GPIO_LEVEL_LOW);
            uapi_gpio_set_val(CONFIG_GPIO_2, GPIO_LEVEL_HIGH);
            uapi_gpio_set_val(CONFIG_GPIO_13, GPIO_LEVEL_LOW);
            uapi_gpio_set_val(CONFIG_GPIO_14, GPIO_LEVEL_HIGH);
            break;
        case 'p':// 右后

            uapi_pwm_stop_group(PWM_GROUP_ID_5);
            uapi_pwm_clear_group(PWM_GROUP_ID_5);
            uapi_pwm_close(PWM_CHANNEL_5);      
            uapi_pwm_open(PWM_CHANNEL_5, &high_wheel_cfg);
            uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
            uapi_pwm_register_interrupt(PWM_CHANNEL_5, pwm_sample_callback);
            uapi_pwm_set_group(PWM_GROUP_ID_5, &channel_id_5, 1);
            uapi_pwm_start_group(PWM_GROUP_ID_5);

            uapi_pwm_stop_group(PWM_GROUP_ID_0);
            uapi_pwm_clear_group(PWM_GROUP_ID_0);
            uapi_pwm_close(PWM_CHANNEL_0);      
            uapi_pwm_open(PWM_CHANNEL_0, &low_wheel_cfg);
            uapi_tcxo_delay_ms((uint32_t)TEST_TCXO_DELAY_1000MS);
            uapi_pwm_register_interrupt(PWM_CHANNEL_0, pwm_sample_callback);
            uapi_pwm_set_group(PWM_GROUP_ID_0, &channel_id_0, 1);
            uapi_pwm_start_group(PWM_GROUP_ID_0);

            uapi_gpio_set_val(CONFIG_GPIO_1, GPIO_LEVEL_LOW);
            uapi_gpio_set_val(CONFIG_GPIO_2, GPIO_LEVEL_HIGH);
            uapi_gpio_set_val(CONFIG_GPIO_13, GPIO_LEVEL_LOW);
            uapi_gpio_set_val(CONFIG_GPIO_14, GPIO_LEVEL_HIGH);
            break;            
        default:
            printf("Unknown command: %c\n", cmd);
            break;
    }
}

int sta_sample_init(const char *argument)
{
    argument = argument;
    int sock_fd;
    // 服务器的地址信息
    struct sockaddr_in send_addr;
    char recv_buf[512];
    wifi_event_stru wifi_event_cb = {0};

    wifi_event_cb.wifi_event_scan_state_changed = wifi_scan_state_changed;
    wifi_event_cb.wifi_event_connection_changed = wifi_connection_changed;
    /* 注册事件回调 */
    if (wifi_register_event_cb(&wifi_event_cb) != 0) {
        printf("wifi_event_cb register fail.\r\n");
        return -1;
    }
    printf("wifi_event_cb register succ.\r\n");

    wifi_connect();

    printf("create socket start! \r\n");
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) != ERRCODE_SUCC) {
        printf("create socket failed!\r\n");
        return 0;
    }
    printf("create socket succ!\r\n");

    /* 初始化预连接的服务端地址 */
    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(CONFIG_SERVER_PORT);
    send_addr.sin_addr.s_addr = inet_addr(CONFIG_SERVER_IP);
    if (connect(sock_fd, (struct sockaddr *)&send_addr, sizeof(send_addr)) != 0) {
        printf("Failed to connect to the server\n");
        return 0;
    }
    printf("Connection to server successful\n");

    while (1) {
        memset(recv_buf, 0, sizeof(recv_buf));
        /* 发送数据到服务端 */
        printf("sendto...\r\n");
        send(sock_fd, SEND_DATA, strlen(SEND_DATA), 0);
        osDelay(DELAY_TIME_MS);

        int recv_len = recv(sock_fd, recv_buf, sizeof(recv_buf), 0);
        if (recv_len > 0) {
            printf("recvfrom:%s\n", recv_buf);
            // 处理接收到的命令
            process_command(recv_buf[0]);
        }
    }
    lwip_close(sock_fd);
    return 0;
}

static void sta_sample(void)
{
    osThreadAttr_t attr;
    attr.name = "sta_sample_task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = WIFI_TASK_STACK_SIZE;
    attr.priority = osPriorityNormal;
    if (osThreadNew((osThreadFunc_t)sta_sample_init, NULL, &attr) == NULL) {
        printf("Create sta_sample_task fail.\r\n");
    }
    printf("Create sta_sample_task succ.\r\n");
}

/* Run the gpio_entry, pwm_entry, spi_master_entry and sta_sample. */
app_run(gpio_entry);
app_run(pwm_entry);
app_run(spi_master_entry);
app_run(sta_sample);
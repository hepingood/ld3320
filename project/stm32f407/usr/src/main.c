/**
 * Copyright (c) 2015 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * @file      main.c
 * @brief     main source file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2021-05-12
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2021/05/12  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#include "driver_ld3320_register_test.h"
#include "driver_ld3320_asr_test.h"
#include "driver_ld3320_mp3_test.h"
#include "driver_ld3320_asr.h"
#include "driver_ld3320_mp3.h"
#include "shell.h"
#include "clock.h"
#include "gpio.h"
#include "delay.h"
#include "uart.h"
#include <stdlib.h>

/**
 * @brief global var definition
 */
uint8_t g_buf[256];                        /**< uart buffer */
uint16_t g_len;                            /**< uart buffer length */
uint8_t g_flag;                            /**< interrupt flag */
static uint8_t gs_flag;                    /**< global flag */
uint8_t (*g_gpio_irq)(void) = NULL;        /**< gpio irq */

/**
 * @brief exti 0 irq
 * @note  none
 */
void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

/**
 * @brief     gpio exti callback
 * @param[in] pin is the gpio pin
 * @note      none
 */
void HAL_GPIO_EXTI_Callback(uint16_t pin)
{
    if (pin == GPIO_PIN_0)
    {
        if (g_gpio_irq)
        {
            g_gpio_irq();
        }
    }
}

/**
 * @brief     asr callback
 * @param[in] type is the irq type
 * @param[in] i is the asr index
 * @param[in] *text points to a asr result buffer
 * @note      none
 */
static void a_asr_callback(uint8_t type, uint8_t i, char *text)
{
    uint8_t res;
    
    if (type == LD3320_STATUS_ASR_FOUND_OK)
    {
        ld3320_interface_debug_print("ld3320: detect index %d %s.\n", i, text);
        
        /* flag found */
        gs_flag = 1;
    }
    else if (type == LD3320_STATUS_ASR_FOUND_ZERO)
    {
        ld3320_interface_debug_print("ld3320: irq zero.\n");
        
        /* start */
        res = ld3320_asr_start();
        if (res != 0)
        {
            ld3320_interface_debug_print("ld3320: start failed.\n");
        }
    }
    else
    {
        ld3320_interface_debug_print("ld3320: irq unknow type.\n");
    }
}

/**
 * @brief     mp3 callback
 * @param[in] type is the irq type
 * @param[in] i is the asr index
 * @param[in] *text points to a asr result buffer
 * @note      none
 */
static void a_mp3_callback(uint8_t type, uint8_t i, char *text)
{
    if (type == LD3320_STATUS_MP3_LOAD)
    {
        ld3320_interface_debug_print("ld3320: irq mp3 load.\n");
    }
    else if (type == LD3320_STATUS_MP3_END)
    {
        /* flag end */
        gs_flag = 1;
        ld3320_interface_debug_print("ld3320: irq mp3 end.\n");
    }
    else
    {
        ld3320_interface_debug_print("ld3320: irq unknow type.\n");
    }
}

/**
 * @brief     ld3320 full function
 * @param[in] argc is arg numbers
 * @param[in] **argv is the arg address
 * @return    status code
 *             - 0 success
 *             - 1 run failed
 *             - 5 param is invalid
 * @note      none
 */
uint8_t ld3320(uint8_t argc, char **argv)
{
    if (argc == 1)
    {
        goto help;
    }
    else if (argc == 2)
    {
        if (strcmp("-i", argv[1]) == 0)
        {
            ld3320_info_t info;
            
            /* print ld3320 info */
            ld3320_info(&info);
            ld3320_interface_debug_print("ld3320: chip is %s.\n", info.chip_name);
            ld3320_interface_debug_print("ld3320: manufacturer is %s.\n", info.manufacturer_name);
            ld3320_interface_debug_print("ld3320: interface is %s.\n", info.interface);
            ld3320_interface_debug_print("ld3320: driver version is %d.%d.\n", info.driver_version/1000, (info.driver_version%1000)/100);
            ld3320_interface_debug_print("ld3320: min supply voltage is %0.1fV.\n", info.supply_voltage_min_v);
            ld3320_interface_debug_print("ld3320: max supply voltage is %0.1fV.\n", info.supply_voltage_max_v);
            ld3320_interface_debug_print("ld3320: max current is %0.2fmA.\n", info.max_current_ma);
            ld3320_interface_debug_print("ld3320: max temperature is %0.1fC.\n", info.temperature_max);
            ld3320_interface_debug_print("ld3320: min temperature is %0.1fC.\n", info.temperature_min);
            
            return 0;
        }
        else if (strcmp("-p", argv[1]) == 0)
        {
            /* print pin connection */
            ld3320_interface_debug_print("ld3320: SCK connected to GPIOA PIN5.\n");
            ld3320_interface_debug_print("ld3320: MISO connected to GPIOA PIN6.\n");
            ld3320_interface_debug_print("ld3320: MOSI connected to GPIOA PIN7.\n");
            ld3320_interface_debug_print("ld3320: CS connected to GPIOA PIN4.\n");
            ld3320_interface_debug_print("ld3320: SCK connected to P2.\n");
            ld3320_interface_debug_print("ld3320: MISO connected to P1.\n");
            ld3320_interface_debug_print("ld3320: MOSI connected to P0.\n");
            ld3320_interface_debug_print("ld3320: CS connected to CSB.\n");
            ld3320_interface_debug_print("ld3320: MD connected to VCC.\n");
            ld3320_interface_debug_print("ld3320: WRB connected to GND.\n");
            ld3320_interface_debug_print("ld3320: RSTB connected to GPIOA PIN8.\n");
            ld3320_interface_debug_print("ld3320: INTB connected to GPIOB PIN0.\n");
            
            return 0;
        }
        else if (strcmp("-h", argv[1]) == 0)
        {
            /* show ld3320 help */
            
            help:
            
            ld3320_interface_debug_print("ld3320 -i\n\tshow ld3320 chip and driver information.\n");
            ld3320_interface_debug_print("ld3320 -h\n\tshow ld3320 help.\n");
            ld3320_interface_debug_print("ld3320 -p\n\tshow ld3320 pin connections of the current board.\n");
            ld3320_interface_debug_print("ld3320 -t reg\n\trun ld3320 register test.\n");
            ld3320_interface_debug_print("ld3320 -t asr\n\trun ld3320 asr test.\n");
            ld3320_interface_debug_print("ld3320 -t mp3 -f <filepath>\n\trun ld3320 mp3 test.filepath is the mp3 music file path.\n");
            ld3320_interface_debug_print("ld3320 -c asr -k <keywords>\n\trun ld3320 asr function.keywords is the asr key words.\n");
            ld3320_interface_debug_print("ld3320 -c mp3 -f <filepath>\n\trun ld3320 mp3 function.filepath is mp3 music file path.\n");
            
            return 0;
        }
        else
        {
            return 5;
        }
    }
    else if (argc == 3)
    {
        /* run test */
        if (strcmp("-t", argv[1]) == 0)
        {
            /* reg test */
            if (strcmp("reg", argv[2]) == 0)
            {
                uint8_t res;
                
                res = ld3320_register_test();
                if (res != 0)
                {
                    return 1;
                }
                
                return 0;
            }
            
            /* asr test */
            else if (strcmp("asr", argv[2]) == 0)
            {
                uint8_t res;
                
                res = gpio_interrupt_init();
                if (res != 0)
                {
                    return 1;
                }
                g_gpio_irq = ld3320_asr_test_irq_handler;
                res = ld3320_asr_test();
                if (res != 0)
                {
                    g_gpio_irq = NULL;
                    (void)gpio_interrupt_deinit();
                    
                    return 1;
                }
                g_gpio_irq = NULL;
                (void)gpio_interrupt_deinit();
                
                return 0;
            }
            
            /* param is invalid */
            else
            {
                return 5;
            }
        }
        
        /* param is invalid */
        else
        {
            return 5;
        }
    }
    else if (argc == 5)
    {
        /* run test */
        if (strcmp("-t", argv[1]) == 0)
        {
            /* mp3 test */
            if (strcmp("mp3", argv[2]) == 0)
            {
                /* -f */
                if (strcmp("-f", argv[3]) == 0)
                {
                    uint8_t res;
                    uint16_t i, len;
                    
                    len = (uint16_t)strlen(argv[4]);
                    for (i = 0; i < len; i++)
                    {
                        if (argv[4][i] == '-')
                        {
                            argv[4][i] = ' ';
                        }
                    }
                    
                    res = gpio_interrupt_init();
                    if (res != 0)
                    {
                        return 1;
                    }
                    g_gpio_irq = ld3320_mp3_test_irq_handler;
                    res = ld3320_mp3_test(argv[4]);
                    if (res != 0)
                    {
                        g_gpio_irq = NULL;
                        (void)gpio_interrupt_deinit();
                        
                        return 1;
                    }
                    g_gpio_irq = NULL;
                    (void)gpio_interrupt_deinit();
                    
                    return 0;
                }
                
                /* param is invalid */
                else
                {
                    return 5;
                }
            }
            
            /* param is invalid */
            else
            {
                return 5;
            }
        }
        
        /* run function */
        else if (strcmp("-c", argv[1]) == 0)
        {
            /* mp3 function */
            if (strcmp("mp3", argv[2]) == 0)
            {
                /* -f */
                if (strcmp("-f", argv[3]) == 0)
                {
                    uint8_t res;
                    uint16_t i, len;
                    uint32_t timeout;
                    
                    len = (uint16_t)strlen(argv[4]);
                    for (i = 0; i < len; i++)
                    {
                        if (argv[4][i] == '-')
                        {
                            argv[4][i] = ' ';
                        }
                    }
                    
                    res = gpio_interrupt_init();
                    if (res != 0)
                    {
                        return 1;
                    }
                    g_gpio_irq = ld3320_mp3_irq_handler;
                    ld3320_interface_debug_print("ld3320: play %s.\n", argv[4]);
                    res = ld3320_mp3_init(argv[4], a_mp3_callback);
                    if (res != 0)
                    {
                        g_gpio_irq = NULL;
                        (void)gpio_interrupt_deinit();
                        
                        return 1;
                    }
                    gs_flag = 0;
                    res = ld3320_mp3_start();
                    if (res != 0)
                    {
                        (void)ld3320_mp3_deinit();
                        g_gpio_irq = NULL;
                        (void)gpio_interrupt_deinit();
                        
                        return 1;
                    }
                    timeout = 1000 * 60 * 10;
                    while (timeout != 0)
                    {
                        if (gs_flag != 0)
                        {
                            break;
                        }
                        timeout--;
                        ld3320_interface_delay_ms(1);
                    }
                    if (timeout == 0)
                    {
                        ld3320_interface_debug_print("ld3320: wait timeout.\n");
                        (void)ld3320_mp3_deinit();
                        g_gpio_irq = NULL;
                        (void)gpio_interrupt_deinit();
                        
                        return 1;
                    }
                    ld3320_interface_debug_print("ld3320: play end.\n");
                    (void)ld3320_mp3_deinit();
                    g_gpio_irq = NULL;
                    (void)gpio_interrupt_deinit();
                    
                    return 0;
                }
                
                /* param is invalid */
                else
                {
                    return 5;
                }
            }
            
            /* asr function */
            else if (strcmp("asr", argv[2]) == 0)
            {
                /* -k */
                if (strcmp("-k", argv[3]) == 0)
                {
                    uint8_t res;
                    char text[1][50];
                    uint16_t i, len;
                    uint32_t timeout;
                    
                    len = (uint16_t)strlen(argv[4]);
                    for (i = 0; i < len; i++)
                    {
                        if (argv[4][i] == '-')
                        {
                            argv[4][i] = ' ';
                        }
                    }
                    
                    res = gpio_interrupt_init();
                    if (res != 0)
                    {
                        return 1;
                    }
                    g_gpio_irq = ld3320_asr_irq_handler; 
                    ld3320_interface_debug_print("ld3320: key word is %s.\n", argv[4]);
                    res = ld3320_asr_init(a_asr_callback);
                    if (res != 0)
                    {
                        g_gpio_irq = NULL;
                        (void)gpio_interrupt_deinit();
                        
                        return 1;
                    }
                    memset(text, 0, sizeof(char) * 50);
                    memcpy(text, argv[4], strlen(argv[4]));
                    res = ld3320_asr_set_keys(text, 1);
                    if (res != 0)
                    {
                        (void)ld3320_asr_deinit();
                        g_gpio_irq = NULL;
                        (void)gpio_interrupt_deinit();
                        
                        return 1;
                    }
                    gs_flag = 0;
                    res = ld3320_asr_start();
                    if (res != 0)
                    {
                        (void)ld3320_asr_deinit();
                        g_gpio_irq = NULL;
                        (void)gpio_interrupt_deinit();
                        
                        return 1;
                    }
                    timeout = 1000 * 10;
                    while (timeout != 0)
                    {
                        if (gs_flag != 0)
                        {
                            break;
                        }
                        timeout--;
                        ld3320_interface_delay_ms(1);
                    }
                    if (timeout == 0)
                    {
                        ld3320_interface_debug_print("ld3320: wait timeout.\n");
                        (void)ld3320_asr_deinit();
                        g_gpio_irq = NULL;
                        (void)gpio_interrupt_deinit();

                        return 1;
                    }
                    ld3320_interface_debug_print("ld3320: found key word.\n");
                    (void)ld3320_asr_deinit();
                    g_gpio_irq = NULL;
                    (void)gpio_interrupt_deinit();
                    
                    return 0;
                }
                
                /* param is invalid */
                else
                {
                    return 5;
                }
            }
            
            /* param is invalid */
            else
            {
                return 5;
            }
        }
        
        /* param is invalid */
        else
        {
            return 5;
        }
    }
    
    /* param is invalid */
    else
    {
        return 5;
    }
}

/**
 * @brief main function
 * @note  none
 */
int main(void)
{
    uint8_t res;
    
    /* stm32f407 clock init and hal init */
    clock_init();
    
    /* delay init */
    delay_init();
    
    /* uart1 init */
    uart1_init(115200);
    
    /* shell init && register ld3320 fuction */
    shell_init();
    shell_register("ld3320", ld3320);
    uart1_print("ld3320: welcome to libdriver ld3320.\n");

    while (1)
    {
        /* read uart */
        g_len = uart1_read(g_buf, 256);
        if (g_len)
        {
            /* run shell */
            res = shell_parse((char *)g_buf, g_len);
            if (res == 0)
            {
                /* run success */
            }
            else if (res == 1)
            {
                uart1_print("ld3320: run failed.\n");
            }
            else if (res == 2)
            {
                uart1_print("ld3320: unknow command.\n");
            }
            else if (res == 3)
            {
                uart1_print("ld3320: length is too long.\n");
            }
            else if (res == 4)
            {
                uart1_print("ld3320: pretreat failed.\n");
            }
            else if (res == 5)
            {
                uart1_print("ld3320: param is invalid.\n");
            }
            else
            {
                uart1_print("ld3320: unknow status code.\n");
            }
            uart1_flush();
        }
        delay_ms(100);
    }
}

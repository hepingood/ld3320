[English](/README.md) | [ 简体中文](/README_zh-Hans.md) | [繁體中文](/README_zh-Hant.md)

<div align=center>
<img src="/doc/image/logo.png"/>
</div>

## LibDriver LD3320

[![API](https://img.shields.io/badge/api-reference-blue)](https://www.libdriver.com/docs/ld3320/index.html) [![License](https://img.shields.io/badge/license-MIT-brightgreen.svg)](/LICENSE)

Ld3320 is a special chip for speech recognition. The chip integrates speech recognition processor and some external circuits, including AD, DA converter, microphone interface, voice output interface and so on. This chip does not need any auxiliary chips such as FLASH, RAM, etc. it can be directly integrated into the existing products to realize the voice recognition / voice control / human-computer dialogue function. The key words list can be edited dynamically. The chip is used in induction cooker, microwave oven, smart home appliance operation, navigator, MP3, MP4, vending machine, public lighting system, health system and smart home voice control.

LibDriver LD3320 is the full function driver of LD3320 launched by LibDriver.It provides voice recognition, MP3 playback and other functions.

### Table of Contents

  - [Instruction](#Instruction)
  - [Install](#Install)
  - [Usage](#Usage)
    - [example asr](#example-asr)
    - [example mp3](#example-mp3)
  - [Document](#Document)
  - [Contributing](#Contributing)
  - [License](#License)
  - [Contact Us](#Contact-Us)

### Instruction

/src includes LibDriver LD3320 source files.

/interface includes LibDriver LD3320 SPI platform independent template。

/test includes LibDriver LD3320 driver test code and this code can test the chip necessary function simply。

/example includes LibDriver LD3320 sample code.

/doc includes LibDriver LD3320 offline document.

/datasheet includes LD3320 datasheet。

/project includes the common Linux and MCU development board sample code. All projects use the shell script to debug the driver and the detail instruction can be found in each project's README.md.

### Install

Reference /interface SPI platform independent template and finish your platform SPI driver.

Add /src, /interface and /example to your project.

### Usage

#### example asr

```C
volatile uint8_t res;
volatile uint32_t timeout;
volatile uint8_t g_flag;
volatile char text[50];

static uint8_t _asr_callback(uint8_t type, uint8_t index, char *text)
{
    volatile uint8_t res;
    
    if (type == LD3320_STATUS_ASR_FOUND_OK)
    {
        ld3320_interface_debug_print("ld3320: detect index %d %s.\n", index, text);
        gs_flag = 1;
    }
    else if (type == LD3320_STATUS_ASR_FOUND_ZERO)
    {
        ld3320_interface_debug_print("ld3320: irq zero.\n");
        ld3320_asr_start();
    }
    else
    {
        ld3320_interface_debug_print("ld3320: irq unknow type.\n");
    }
    
    return 0;
}

res = gpio_interrupt_init();
if (res)
{
    return 1;
}
res = ld3320_asr_init(_asr_callback);
if (res)
{
    gpio_interrupt_deinit();

    return 1;
}
memset(text, 0, sizeof(char) * 50);
memcpy(text, "ni hao", strlen("ni hao"));
res = ld3320_asr_set_keys(text, 1);
if (res)
{
    ld3320_asr_deinit();
    gpio_interrupt_deinit();

    return 1;
}
gs_flag = 0;
res = ld3320_asr_start();
if (res)
{
    ld3320_asr_deinit();
    gpio_interrupt_deinit();

    return 1;
}

...

timeout = 1000 * 10;
while (timeout)
{
    if (gs_flag)
    {
        break;
    }
    timeout--;
    ld3320_interface_delay_ms(1);
}
if (timeout == 0)
{
    ld3320_interface_debug_print("ld3320: wait timeout.\n");
    ld3320_asr_deinit();
    gpio_interrupt_deinit();

    return 1;
}

...

ld3320_interface_debug_print("ld3320: found key word.\n");
ld3320_asr_deinit();
gpio_interrupt_deinit();

...

return 0;
```

#### example mp3

```C
volatile uint8_t res;
volatile uint32_t timeout;
volatile uint8_t g_flag;

static uint8_t _mp3_callback(uint8_t type, uint8_t index, char *text)
{
    if (type == LD3320_STATUS_MP3_LOAD)
    {
        ld3320_interface_debug_print("ld3320: irq mp3 load.\n");
    }
    else if (type == LD3320_STATUS_MP3_END)
    {
        gs_flag = 1;
        ld3320_interface_debug_print("ld3320: irq mp3 end.\n");
    }
    else if (type == LD3320_STATUS_MP3_LOAD)
    {
        ld3320_interface_debug_print("ld3320: irq mp3 load.\n");
    }
    else
    {
        ld3320_interface_debug_print("ld3320: irq unknow type.\n");
    }
    
    return 0;
}

res = gpio_interrupt_init();
if (res)
{
    return 1;
}
res = ld3320_mp3_init("xxx.mp3", _mp3_callback);
if (res)
{
    gpio_interrupt_deinit();

    return 1;
}
gs_flag = 0;
res = ld3320_mp3_start();
if (res)
{
    ld3320_mp3_deinit();
    gpio_interrupt_deinit();

    return 1;
}

...
    
timeout = 1000 * 60 * 10;
while (timeout)
{
    if (gs_flag)
    {
        break;
    }
    timeout--;
    ld3320_interface_delay_ms(1);
}
if (timeout == 0)
{
    ld3320_interface_debug_print("ld3320: wait timeout.\n");
    ld3320_mp3_deinit();
    gpio_interrupt_deinit();

    return 1;
}

...

ld3320_interface_debug_print("ld3320: play end.\n");
ld3320_mp3_deinit();
gpio_interrupt_deinit();

...

return 0;
```

### Document

Online documents: https://www.libdriver.com/docs/ld3320/index.html

Offline documents: /doc/html/index.html

### Contributing

Please sent an e-mail to lishifenging@outlook.com

### License

Copyright (c) 2015 - present LibDriver All rights reserved



The MIT License (MIT) 



Permission is hereby granted, free of charge, to any person obtaining a copy

of this software and associated documentation files (the "Software"), to deal

in the Software without restriction, including without limitation the rights

to use, copy, modify, merge, publish, distribute, sublicense, and/or sell

copies of the Software, and to permit persons to whom the Software is

furnished to do so, subject to the following conditions: 



The above copyright notice and this permission notice shall be included in all

copies or substantial portions of the Software. 



THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR

IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,

FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE

AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER

LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,

OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE

SOFTWARE. 

### Contact Us

Please sent an e-mail to lishifenging@outlook.com
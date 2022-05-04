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
 * @file      sdio.h
 * @brief     sdio header file
 * @version   1.0.0
 * @author    Shifeng Li
 * @date      2021-2-12
 *
 * <h3>history</h3>
 * <table>
 * <tr><th>Date        <th>Version  <th>Author      <th>Description
 * <tr><td>2021/02/12  <td>1.0      <td>Shifeng Li  <td>first upload
 * </table>
 */

#ifndef SDIO_H
#define SDIO_H

#include "stm32f4xx_hal.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @defgroup sdio sdio function
 * @brief    sdio function modules
 * @{
 */

/**
 * @brief sdio var definition
 */
extern SD_HandleTypeDef  g_sd_handle;
extern DMA_HandleTypeDef g_sd_tx_dma_handle;
extern DMA_HandleTypeDef g_sd_rx_dma_handle;
extern HAL_SD_CardInfoTypeDef  g_card_info;
extern uint8_t g_sd_tx_done; 
extern uint8_t g_sd_rx_done;

/**
 * @brief  sdio bus init
 * @return status code
 *         - 0 success
 *         - 1 init failed
 *         - 2 get card information failed
 *         - 3 set 4 wire bus failed
 * @note   none
 */
uint8_t sdio_init(void);

/**
 * @brief  sdio bus deinit
 * @return status code
 *         - 0 success
 *         - 1 deinit failed
 * @note   none
 */
uint8_t sdio_deinit(void);

/**
 * @brief      sdio read
 * @param[in]  sector is the read first sector
 * @param[out] *buf points to a data buffer
 * @param[in]  cnt is the data length
 * @return     status code
 *             - 0 success
 *             - 1 read failed
 *             - 2 read timeout
 * @note       none
 */
uint8_t sdio_read(uint32_t sector, uint8_t *buf, uint32_t cnt);

/**
 * @brief     sdio write
 * @param[in] sector is the written first sector
 * @param[in] *buf points to a data buffer
 * @param[in] cnt is the data length
 * @return    status code
 *            - 0 success
 *            - 1 write failed
 *            - 2 write timeout
 * @note      none
 */
uint8_t sdio_write(uint32_t sector, uint8_t *buf, uint32_t cnt);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif

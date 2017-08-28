/**
 * \file
 *
 * \brief SD/MMC stack configuration file.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_SD_MMC_H_INCLUDED
#define CONF_SD_MMC_H_INCLUDED

/* Define it to enable the SPI mode instead of Multimedia Card interface mode */
#define SD_MMC_SPI_MODE

/** Defines required by SD MMC Stack */
#define SD_MMC_SPI_MEM_CNT          1

/** HSMCI pins definition. */
/*! Number of slot connected on HSMCI interface */
#define SD_MMC_HSMCI_MEM_CNT      1
#define SD_MMC_HSMCI_SLOT_0_SIZE  4
#define PINS_HSMCI   {0x3fUL << 26, PIOA, ID_PIOA, PIO_PERIPH_C, PIO_PULLUP}
/** HSMCI MCCDA pin definition. */
#define PIN_HSMCI_MCCDA_GPIO            (PIO_PA28_IDX)
#define PIN_HSMCI_MCCDA_FLAGS           (PIO_PERIPH_C | PIO_DEFAULT)
/** HSMCI MCCK pin definition. */
#define PIN_HSMCI_MCCK_GPIO             (PIO_PA29_IDX)
#define PIN_HSMCI_MCCK_FLAGS            (PIO_PERIPH_C | PIO_DEFAULT)
/** HSMCI MCDA0 pin definition. */
#define PIN_HSMCI_MCDA0_GPIO            (PIO_PA30_IDX)
#define PIN_HSMCI_MCDA0_FLAGS           (PIO_PERIPH_C | PIO_DEFAULT)
/** HSMCI MCDA1 pin definition. */
#define PIN_HSMCI_MCDA1_GPIO            (PIO_PA31_IDX)
#define PIN_HSMCI_MCDA1_FLAGS           (PIO_PERIPH_C | PIO_DEFAULT)
/** HSMCI MCDA2 pin definition. */
#define PIN_HSMCI_MCDA2_GPIO            (PIO_PA26_IDX)
#define PIN_HSMCI_MCDA2_FLAGS           (PIO_PERIPH_C | PIO_DEFAULT)
/** HSMCI MCDA3 pin definition. */
#define PIN_HSMCI_MCDA3_GPIO            (PIO_PA27_IDX)
#define PIN_HSMCI_MCDA3_FLAGS           (PIO_PERIPH_C | PIO_DEFAULT)
/** SD/MMC card detect pin definition. */


/* Define it to enable the SDIO support */
//#define SDIO_SUPPORT_ENABLE

#define SD_MMC_SPI                  	SPI2

#define CONF_BOARD_SD_MMC_SPI

/*
 * Definitions the SD card SPI hardware installations.
 *
 */

/* ----------------- SD card I/O-s --------------------------------*/

#define SD_MMC_CS_GPIO	// SD_MMC_CS defined instead of NSS SPI port define

#define SPI2_SIGNAL_PORT	GPIOB

#define SPI2_SIGNAL_PORT_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define SPI2_CLK_PIN	GPIO_PIN_13
#define SPI2_MOSI_PIN	GPIO_PIN_15
#define SPI2_MISO_PIN	GPIO_PIN_14

#define SD_CARD_CS_PORT	GPIOB
#define SD_CARD_CS_PORT_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()

#define SD_CARD_CS_PIN	GPIO_PIN_11


#define SELECT_SD()		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_RESET)
#define DESELECT_SD()	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_11, GPIO_PIN_SET)


/* Define it to enable the debug trace to the current standard output (stdio) */
//#define SD_MMC_DEBUG

#endif /* CONF_SD_MMC_H_INCLUDED */


/*
 * spi_master.h
 *
 *  Created on: 2017. aug. 26.
 *      Author: Apa
 */

#ifndef SD_SPI_SPI_MASTER_H_
#define SD_SPI_SPI_MASTER_H_

#include "spi.h"
#include "stm32f1xx_hal.h"
#include "conf_sd_mmc.h"
#include "compiler.h"


//! SPI Flags Definition
typedef uint8_t spi_flags_t;

//! Board SPI Select Id Definition
typedef uint32_t board_spi_select_id_t;


//! \brief Polled SPI device definition.
struct spi_device {
	//! Board specific select id
	board_spi_select_id_t id;
};

#define SPI_MASTER_READ_TIMEOUT	10000

/** \brief Initialize the SPI in master mode.
 *
 * \param p_spi  Base address of the SPI instance.
 *
 */
void spi_master_init(SPI_HandleTypeDef* handle);

void spi_master_setup_device(SPI_HandleTypeDef* handle, struct spi_device *device, spi_flags_t flags,
		uint32_t baud_rate, board_spi_select_id_t sel_id);

void spi_select_device(SPI_HandleTypeDef* handle, struct spi_device* device);
void spi_deselect_device(SPI_HandleTypeDef* handle, struct spi_device* device);

HAL_StatusTypeDef spi_read_packet(SPI_HandleTypeDef* handle, uint8_t *data, size_t len);
HAL_StatusTypeDef spi_write_packet(SPI_HandleTypeDef* handle, uint8_t *data, size_t len);

bool spi_is_enabled(SPI_HandleTypeDef* handle);
void spi_enable(SPI_HandleTypeDef* handle);

#endif /* SD_SPI_SPI_MASTER_H_ */

/*
 * spi_master.c
 *
 *  Created on: 2017. aug. 26.
 *      Author: Apa
 */

#include "spi_master.h"

void spi_master_init(SPI_HandleTypeDef* handle)
{

	/* Generate the CRC7 checksum table. */
#if defined (SD_CRC7)
	GenerateCRCTable();
#endif

  GPIO_InitTypeDef  gpioinitstruct = {0};

		/* Enable SPI2, and signal ports clock. */
	__HAL_RCC_SPI2_CLK_ENABLE();
	SPI2_SIGNAL_PORT_CLK_ENABLE();
	SD_CARD_CS_PORT_CLK_ENABLE();

  /* configure SPI2 SCK (GPIOB 13), and SPI2 MOSI (GPIOB 15)*/

  gpioinitstruct.Pin        = SPI2_CLK_PIN | SPI2_MOSI_PIN;
  gpioinitstruct.Mode       = GPIO_MODE_AF_PP;
  gpioinitstruct.Pull       = GPIO_NOPULL;
  gpioinitstruct.Speed      = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SPI2_SIGNAL_PORT, &gpioinitstruct);

	/* configure SD chip select line (GPIOB 11)*/

  gpioinitstruct.Pin        = SD_CARD_CS_PIN;
  gpioinitstruct.Mode       = GPIO_MODE_OUTPUT_PP;
  gpioinitstruct.Pull       = GPIO_NOPULL;
  gpioinitstruct.Speed      = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SD_CARD_CS_PORT, &gpioinitstruct);

  /* Configure SPI MISO to input GPIOB 14 .
	 * Note! The line must be configuring pull up mode, or pulling up physically
	 * with resistor , because if there is not SD card, float the input line,
	 * might cause disturbance input datas.   */

  gpioinitstruct.Pin        = SPI2_MISO_PIN;
  gpioinitstruct.Mode       = GPIO_MODE_AF_INPUT;
  gpioinitstruct.Pull       = GPIO_PULLUP;
  gpioinitstruct.Speed      = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SPI2_SIGNAL_PORT, &gpioinitstruct);


/*	spi_enable_clock(p_spi);

	spi_reset(p_spi);
	spi_set_master_mode(p_spi);
	spi_disable_mode_fault_detect(p_spi);
	spi_disable_loopback(p_spi);
	spi_set_peripheral_chip_select_value(p_spi, DEFAULT_CHIP_ID);
	spi_set_fixed_peripheral_select(p_spi);
	spi_disable_peripheral_select_decode(p_spi);
	spi_set_delay_between_chip_select(p_spi, CONFIG_SPI_MASTER_DELAY_BCS);*/
}



/**
 * \brief Calculate the baudrate divider.
 *
 * \param baudrate Baudrate value.
 * \param mck      SPI module input clock frequency (MCK clock, Hz).
 * /*	nearest baud rate to clock.
 * 000: fPCLK/2
 * 001: fPCLK/4
 * 010: fPCLK/8
 * 011: fPCLK/16
 * 100: fPCLK/32
 * 101: fPCLK/64
 * 110: fPCLK/128
 * 111: fPCLK/256
 *
 */
uint32_t spi_calc_baudrate_div(const uint32_t baudrate, uint32_t mck)
{	uint32_t div_code = 0;
	mck /= 2;
	do
	{
		if (mck > baudrate)
		{
			mck /= 2;
			div_code += 1;
		} else break;
	} while (1);

//	int baud_div = div_ceil(mck, baudrate);

	/* The value of baud_div is from 1 to 255 in the SCBR field. */
/*	if (baud_div <= 0 || baud_div > 255) {
		return -1;
	} else
	{

	}*/
	if (div_code > 7) return -1;

	return div_code;
}


/**
 * \brief Set up an SPI device.
 *
 */
void spi_master_setup_device(SPI_HandleTypeDef* handle, struct spi_device *device,
		spi_flags_t flags, uint32_t baud_rate, board_spi_select_id_t sel_id)
{
	uint32_t baud_div = spi_calc_baudrate_div(baud_rate, HAL_RCC_GetSysClockFreq());


	/* avoid Cppcheck Warning */
	UNUSED(sel_id); UNUSED(flags);
	if (-1 == baud_div) {
		Assert(0 == "Failed to find baudrate divider");
	}

	handle->Instance 				= SD_MMC_SPI;
	handle->Init.BaudRatePrescaler  = (baud_div << 3);
//	handle->Init.BaudRatePrescaler  = SPI_BAUDRATEPRESCALER_256;
	handle->Init.Direction          = SPI_DIRECTION_2LINES;
	handle->Init.CLKPhase           = SPI_PHASE_1EDGE;
	handle->Init.CLKPolarity        = SPI_POLARITY_LOW;
	handle->Init.CRCCalculation     = SPI_CRCCALCULATION_DISABLE;
	handle->Init.CRCPolynomial      = 7;
	handle->Init.DataSize           = SPI_DATASIZE_8BIT;
	handle->Init.FirstBit           = SPI_FIRSTBIT_MSB;
	handle->Init.NSS                = SPI_NSS_SOFT;
	handle->Init.TIMode             = SPI_TIMODE_DISABLE;
	handle->Init.Mode               = SPI_MODE_MASTER;

	SPI_Init(handle);

/*	spi_set_transfer_delay(p_spi, device->id, CONFIG_SPI_MASTER_DELAY_BS,
			CONFIG_SPI_MASTER_DELAY_BCT);
	spi_set_bits_per_transfer(p_spi, device->id,
			CONFIG_SPI_MASTER_BITS_PER_TRANSFER);
	spi_set_baudrate_div(p_spi, device->id, baud_div);
	spi_configure_cs_behavior(p_spi, device->id, SPI_CS_KEEP_LOW);
	spi_set_clock_polarity(p_spi, device->id, flags >> 1);
	spi_set_clock_phase(p_spi, device->id, ((flags & 0x1) ^ 0x1));*/
}

/**
 * \brief Receive a sequence of bytes from an SPI device.
 *
 * All bytes sent out on SPI bus are sent as value 0.
 *
 * \param p_spi     Base address of the SPI instance.
 * \param data      Data buffer to read.
 * \param len       Length of data to be read.
 *
 * \pre SPI device must be selected with spi_select_device() first.
 */
HAL_StatusTypeDef spi_read_packet(SPI_HandleTypeDef* handle, uint8_t *data, size_t len)
{

	return HAL_SPI_Receive(handle, data, len, SPI_MASTER_READ_TIMEOUT);

/*	uint32_t timeout = SPI_TIMEOUT;
	uint8_t val;
	uint32_t i = 0;

	while (len) {
		timeout = SPI_TIMEOUT;
		while (!spi_is_tx_ready(p_spi)) {
			if (!timeout--) {
				return ERR_TIMEOUT;
			}
		}
		spi_write_single(p_spi, CONFIG_SPI_MASTER_DUMMY);

		timeout = SPI_TIMEOUT;
		while (!spi_is_rx_ready(p_spi)) {
			if (!timeout--) {
				return ERR_TIMEOUT;
			}
		}
		spi_read_single(p_spi, &val);

		data[i] = val;
		i++;
		len--;
	}

	return STATUS_OK;*/
}

/**
 * \brief Send a sequence of bytes to an SPI device.
 *
 *
 * \pre SPI device must be selected with spi_select_device() first.
 */
HAL_StatusTypeDef spi_write_packet(SPI_HandleTypeDef* handle, uint8_t *data, size_t len)
{

	return HAL_SPI_Transmit(handle, data, len, SPI_MASTER_READ_TIMEOUT);

/*	uint32_t timeout = SPI_TIMEOUT;
	uint32_t i = 0;
	uint8_t val;

	while (len) {
		timeout = SPI_TIMEOUT;
		while (!spi_is_tx_ready(p_spi)) {
			if (!timeout--) {
				return ERR_TIMEOUT;
			}
		}
		val = data[i];
		spi_write_single(p_spi, val);
		i++;
		len--;
	}

	return STATUS_OK;*/
}

void spi_select_device(SPI_HandleTypeDef* handle, struct spi_device* device)
{
	UNUSED(handle);
	UNUSED(device);

//	SELECT_SD();

	/**
	 * \brief Select the given device on the SPI bus.
	 *
	 * Set device specific setting and call board chip select.
	 *
	 * \param p_spi   Base address of the SPI instance.
	 * \param device  SPI device.
	 *
	 */
/*		if (spi_get_peripheral_select_decode_setting(p_spi)) {
			if (device->id < MAX_NUM_WITH_DECODER) {
				spi_set_peripheral_chip_select_value(p_spi, device->id);
			}
		} else {
			if (device->id < MAX_NUM_WITHOUT_DECODER) {
				spi_set_peripheral_chip_select_value(p_spi, (~(1 << device->id)));
			}
		}*/

}

/**
 * \brief Deselect the given device on the SPI bus.
 *
 * Call board chip deselect.
 *
 * \param p_spi   Base address of the SPI instance.
 * \param device  SPI device.
 *
 * \pre SPI device must be selected with spi_select_device() first.
 */
void spi_deselect_device(SPI_HandleTypeDef* handle, struct spi_device* device)
{

	UNUSED(handle);
	UNUSED(device);

//	DESELECT_SD();
	/* avoid Cppcheck Warning
	UNUSED(device);
	while (!spi_is_tx_empty(p_spi)) {
	}

	// Assert all lines; no peripheral is selected.
	spi_set_peripheral_chip_select_value(p_spi, NONE_CHIP_SELECT_ID);

	// Last transfer, so de-assert the current NPCS if CSAAT is set.
	spi_set_lastxfer(p_spi);*/

}


bool spi_is_enabled(SPI_HandleTypeDef* handle)
{
	return false;
}

void spi_enable(SPI_HandleTypeDef* handle)
{

}

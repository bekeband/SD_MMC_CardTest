

/* The indicator LED connect to C port 13-th PIN */

#include <stdlib.h>
#include "stm32f1xx_hal.h"
#include "interrupts.h"
#include "init.h"
#include "sd_spi.h"
#include "ili9341_spi.h"
#include "sd_mmc.h"

//! \name Read/write access configurations
//! @{

//! Offset before the end of memory card to start the test
#define TEST_MEM_START_OFFSET  (1024lu * 1024lu * 4lu) // 4MB

//! Memory area size dedicated for the read/write test
#define TEST_MEM_AREA_SIZE (1024lu * 1024lu * 1lu) // 1MB
/**
 * Size of each read or write access.
 * Increasing this number can get higher R/W performance.
 */
#define TEST_MEM_ACCESS_SIZE  (8lu * SD_MMC_BLOCK_SIZE)

//! Buffer used by read/write tests
COMPILER_WORD_ALIGNED
static uint8_t buf_test[TEST_MEM_ACCESS_SIZE];

//! The value used to generate test data
#define TEST_FILL_VALUE_U32     (0x5500AAFFU)

#define TICK_US 1000

//extern s_image button;


static void main_display_info_card(uint8_t slot);
static void main_test_memory(uint8_t slot);
/*static void main_test_sdio(uint8_t slot);
static void main_dump_buffer(uint8_t *data_buffer, uint32_t length);*/


/**
 System Clock, and osc configuration for 8000000 Hz Minimal STM 32 board.
  */
void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef clkinitstruct = {0};
  RCC_OscInitTypeDef oscinitstruct = {0};
  
  /* Enable HSE Oscillator and activate PLL with HSE as source */
  oscinitstruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
  oscinitstruct.HSEState        = RCC_HSE_ON;
  oscinitstruct.HSEPredivValue  = RCC_HSE_PREDIV_DIV1;
  oscinitstruct.PLL.PLLState    = RCC_PLL_ON;
  oscinitstruct.PLL.PLLSource   = RCC_PLLSOURCE_HSE;
  oscinitstruct.PLL.PLLMUL      = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&oscinitstruct)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 
     clocks dividers */
  clkinitstruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  clkinitstruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  clkinitstruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  clkinitstruct.APB2CLKDivider = RCC_HCLK_DIV1;
  clkinitstruct.APB1CLKDivider = RCC_HCLK_DIV2;  
  if (HAL_RCC_ClockConfig(&clkinitstruct, FLASH_LATENCY_2)!= HAL_OK)
  {
    /* Initialization Error */
    while(1);
  }
}


uint8_t buffer[1000];
uint8_t i = 0;

char cid_string[6];

t_color color;

t_color colors[8] = {	{0b11111000, 0b00000000},
						{0b00000111, 0b11100000},
						{0b00000000, 0b00011111}};

int main()
{
	uint8_t slot = 0;
	sd_mmc_err_t err;
	/* Enable the DIVZERO exception. */
  SCB->CCR |= SCB_CCR_DIV_0_TRP;
	HAL_Init();
	SystemClock_Config();
	
	Init_ErrLED();
	
//	SD_SPI2_Init(); 	// Initialize SPI2 for SD card.

	// Initialize SD MMC stack
	sd_mmc_init();


	DISPLAY_SPI1_Init();	// Initialize SPI1 for display.
	ILI9341_Init();

	color[0] = 0b00100000;
	color[1] = 0b10000100;
	color[2] = 0b10000100;
	ILI9341_fillrectangle(0, 0, 240, 320, color);
//	ILI9341_displaybitmap(0, 0, 80, 60, &button);

//	uint8_t BF[SCR_BUFFER_SIZE];
//	ILI9341_getpixels(0, 0, 80, 60, &BF[0]);

	if (slot == sd_mmc_nb_slot()) {
		slot = 0;
	}

	while (1) {
//		printf("Please plug an SD, MMC or SDIO card in slot %d.\n\r", slot+1);

		// Wait for a card and ready
		if ((SD_CARD_CHECK_FLAG == true))
		{
			SD_CARD_CHECK_FLAG = false;
			err = sd_mmc_check(slot);
			if ((SD_MMC_ERR_NO_CARD != err)
					&& (SD_MMC_INIT_ONGOING != err)
					&& (SD_MMC_OK != err)) {
				printf("Card install FAILED\n\r");
				printf("Please unplug and re-plug the card.\n\r");
/*				while (SD_MMC_ERR_NO_CARD != sd_mmc_check(slot)) {
				}*/




			} else
			{
				// Display basic card information
				main_display_info_card(slot);

				/* Test the card */
				if (sd_mmc_get_type(slot) & CARD_TYPE_SDIO) {
					// Test CIA of SDIO card
//					main_test_sdio(slot);
				}
				if (sd_mmc_get_type(slot) & (CARD_TYPE_SD | CARD_TYPE_MMC)) {
					// SD/MMC Card R/W
					main_test_memory(slot);
				}
			}





		}


			HAL_Delay(150);

			DISPLAY_ON();
			ILI9341_fillrectangle(0, 0, 240, 320, colors[i]);
			if (++i > 2) i = 0;
//			DisplaySoftOn();
			HAL_Delay(150);
//			DisplaySoftOff();


	};

/* Main program loop. */
	while (1)
	{	


		
/*		while (1)
		{

			HAL_Delay(50);

//			DISPLAY_ON();
//			ILI9341_fillrectangle(0, 0, 240, 320, colors[i]);
			if (++i > 2) i = 0;
			DisplaySoftOn();
			HAL_Delay(150);
			DisplaySoftOff();
//			DISPLAY_OFF();
		}*/

		if (GetSDCardCheckFlag())
		{

			SD_Card_SPI_Select();
			if (ResetCard() == SD_SPI_OK)
			{	
				ForceErrorNumber(4);
				if (SPIModeInitialize() == SD_SPI_OK)
				{
					SetFastSPI();

					GetCIDRegister(cid_string);
	//				GetCSDRegister();
					ReadDataBlock(i++, (uint8_t*)&buffer);
				} else
				{
					ForceErrorNumber(3);
				}
			} else
			{
				ForceErrorNumber(2);				
			}
				
			ClearSDCardCheckFlag();
		}

	}
	
	return 0;
	}

uint32_t time_tick_calc_delay(uint32_t tick_start, uint32_t tick_end)
{
	if (tick_end >= tick_start) {
		return (tick_end - tick_start) * (1000 / TICK_US);
	} else {
		/* In the case of 32-bit couter number overflow */
		return (tick_end + (0xFFFFFFFF - tick_start)) * (1000 / TICK_US);
	}
}


/**
 * \brief Display basic information of the card.
 * \note This function should be called only after the card has been
 *       initialized successfully.
 *
 * \param slot   SD/MMC slot to test
 */
static void main_display_info_card(uint8_t slot)
{
	printf("Card information:\n\r");

	printf("    ");
	switch (sd_mmc_get_type(slot)) {
	case CARD_TYPE_SD | CARD_TYPE_HC:
		printf("SDHC");
		break;
	case CARD_TYPE_SD:
		printf("SD");
		break;
	case CARD_TYPE_MMC | CARD_TYPE_HC:
		printf("MMC High Density");
		break;
	case CARD_TYPE_MMC:
		printf("MMC");
		break;
	case CARD_TYPE_SDIO:
		printf("SDIO\n\r");
		return;
	case CARD_TYPE_SD_COMBO:
		printf("SD COMBO");
		break;
	case CARD_TYPE_UNKNOWN:
	default:
		printf("Unknow\n\r");
		return;
	}
	printf("\n\r    %d MB\n\r", (uint16_t)(sd_mmc_get_capacity(slot)/1024));
}

/**
 * \brief Card R/W tests
 *
 * \param slot   SD/MMC slot to test
 */
static void main_test_memory(uint8_t slot)
{
	uint32_t last_blocks_addr, i, nb_trans;
	uint32_t tick_start, time_ms;

	// Compute the last address
	last_blocks_addr = sd_mmc_get_capacity(slot) *
			(1024 / SD_MMC_BLOCK_SIZE);
	if (last_blocks_addr < (TEST_MEM_START_OFFSET / 512lu)) {
		printf("[Memory is too small.]\n\r");
		return;
	}
	last_blocks_addr -= (TEST_MEM_START_OFFSET / SD_MMC_BLOCK_SIZE);

	printf("Card R/W test:\n\r");

	// Read the last block
	printf("    Read... ");
	tick_start = HAL_GetTick();
	if (SD_MMC_OK != sd_mmc_init_read_blocks(slot,
			last_blocks_addr,
			TEST_MEM_AREA_SIZE / SD_MMC_BLOCK_SIZE)) {
		printf("[FAIL]\n\r");
		return;
	}
	for (nb_trans = 0; nb_trans < (TEST_MEM_AREA_SIZE / TEST_MEM_ACCESS_SIZE);
			nb_trans++) {
		if (SD_MMC_OK != sd_mmc_start_read_blocks(buf_test,
					TEST_MEM_ACCESS_SIZE / SD_MMC_BLOCK_SIZE)) {
			printf("[FAIL]\n\r");
			return;
		}
		if (SD_MMC_OK != sd_mmc_wait_end_of_read_blocks(false)) {
			printf("[FAIL]\n\r");
			return;
		}
	}
	time_ms = time_tick_calc_delay(tick_start, HAL_GetTick());
	if (time_ms) { // Valid time_ms
		printf(" %d KBps ", (int)(((TEST_MEM_AREA_SIZE
				* 1000lu) / 1024lu) / time_ms));
	}
	printf("[OK]\n\r");

	if (sd_mmc_is_write_protected(slot)) {
		printf("Card is write protected [WRITE TEST SKIPPED]\n\r");
		return;
	}

	// Fill buffer with a pattern
	for (i = 0; i < (TEST_MEM_ACCESS_SIZE / sizeof(uint32_t)); i++) {
		((uint32_t*)buf_test)[i] = TEST_FILL_VALUE_U32;
	}

	printf("    Write pattern... ");
	if (SD_MMC_OK != sd_mmc_init_write_blocks(slot,
			last_blocks_addr,
			TEST_MEM_AREA_SIZE / SD_MMC_BLOCK_SIZE)) {
		printf("[FAIL]\n\r");
		return;
	}
	tick_start = HAL_GetTick();
	for (nb_trans = 0; nb_trans < (TEST_MEM_AREA_SIZE / TEST_MEM_ACCESS_SIZE);
			nb_trans++) {
		((uint32_t*)buf_test)[0] = nb_trans; // Unique value for each area
		if (SD_MMC_OK != sd_mmc_start_write_blocks(buf_test,
				TEST_MEM_ACCESS_SIZE / SD_MMC_BLOCK_SIZE)) {
			printf("[FAIL]\n\r");
			return;
		}
		if (SD_MMC_OK != sd_mmc_wait_end_of_write_blocks(false)) {
			printf("[FAIL]\n\r");
			return;
		}
	}
	time_ms = time_tick_calc_delay(tick_start, HAL_GetTick());
	if (time_ms) { // Valid time_ms
		printf(" %d KBps ", (int)(((TEST_MEM_AREA_SIZE
				* 1000lu) / 1024lu) / time_ms));
	}
	printf("[OK]\n\r");

	printf("    Read and check pattern... ");
	if (SD_MMC_OK != sd_mmc_init_read_blocks(slot,
			last_blocks_addr,
			TEST_MEM_AREA_SIZE / SD_MMC_BLOCK_SIZE)) {
		printf("Read [FAIL]\n\r");
		return;
	}
	for (nb_trans = 0; nb_trans < (TEST_MEM_AREA_SIZE / TEST_MEM_ACCESS_SIZE);
			nb_trans++) {
		// Clear buffer
		for (i = 0; i < (TEST_MEM_ACCESS_SIZE / sizeof(uint32_t)); i++) {
			((uint32_t*)buf_test)[i] = 0xFFFFFFFF;
		}
		// Fill buffer
		if (SD_MMC_OK != sd_mmc_start_read_blocks(buf_test,
					TEST_MEM_ACCESS_SIZE / SD_MMC_BLOCK_SIZE)) {
			printf("Read [FAIL]\n\r");
			return;
		}
		if (SD_MMC_OK != sd_mmc_wait_end_of_read_blocks(false)) {
			printf("Read [FAIL]\n\r");
			return;
		}
		// Check the unique value of the area
		if (((uint32_t*)buf_test)[0] != nb_trans) {
			printf("Check [FAIL]\n\r");
			return;
		}
		// Check buffer
		for (i = 1; i < (TEST_MEM_ACCESS_SIZE / sizeof(uint32_t)); i++) {
			if (((uint32_t*)buf_test)[i] != TEST_FILL_VALUE_U32) {
				printf("Check [FAIL]\n\r");
				return;
			}
		}
	}
	printf("[OK]\n\r");
}


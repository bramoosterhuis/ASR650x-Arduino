/*!
 * \file      spi-board.c
 *
 * \brief     Target board SPI driver implementation
 *
 * \copyright Revised BSD License, see section \ref LICENSE.
 *
 * \code
 *                ______                              _
 *               / _____)             _              | |
 *              ( (____  _____ ____ _| |_ _____  ____| |__
 *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
 *               _____) ) ____| | | || |_| ____( (___| | | |
 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
 *              (C)2013-2017 Semtech
 *
 * \endcode
 *
 * \author    Miguel Luis ( Semtech )
 *
 * \author    Gregory Cristian ( Semtech )
 */
#include <project.h>
#include "utilities.h"
#include "board.h"
#include "debug.h"
#include "gpio.h"
#include "spi-board.h"

//#include "asr_project.h"

static reg32 ClockDividerRegisterBackup = 0;

// Reset all values to their working defaults.
void SetDefaultSPISettings(){
	// Recover clock register
	SPI_1_SCBCLK_DIV_REG = ClockDividerRegisterBackup;

	// CPHA 0, CPOL 0
	SPI_1_SPI_CTRL_REG &= ~(SPI_1_SPI_CTRL_CPHA);
	SPI_1_SPI_CTRL_REG &= ~(SPI_1_SPI_CTRL_CPOL);

	// Set MSB First
    SPI_1_TX_CTRL_REG |= (SPI_1_TX_CTRL_MSB_FIRST);
    SPI_1_RX_CTRL_REG |= (SPI_1_RX_CTRL_MSB_FIRST);
}

void SpiInit( )
{
	SPI_1_Start();
	ClockDividerRegisterBackup = (SPI_1_SCBCLK_DIV_REG & 0xffffff00);
}

void SpiDeInit( Spi_t *obj )
{
	SPI_1_Stop();
}

void SpiFormat( Spi_t *obj, int8_t bits, int8_t cpol, int8_t cpha, int8_t slave )
{
	
}

void SpiFrequency( Spi_t *obj, uint32_t hz )
{
}

#define SPI_RX_TIMEOUT	500
uint16_t SpiInOut( Spi_t *obj, uint16_t outData )
{
	uint32 data;
	uint32 timeout = 0;

	while(SPI_1_SpiUartGetTxBufferSize() != 0);
	
	SetDefaultSPISettings();
	
	SPI_1_SpiUartWriteTxData(outData);
//	trace("Send Data 0x%x...\n", outData);
	
	while(SPI_1_SpiUartGetRxBufferSize() == 0)
	{
		timeout++;
		if(timeout > SPI_RX_TIMEOUT )break;
	}
	if(timeout > SPI_RX_TIMEOUT )
		DBG_PRINTF("Receive Data timeout!.\r\n");
	else
	{
		data = SPI_1_SpiUartReadRxData();
//		trace("Receive Data:0x%x.\n", data);
        return data;
	}
    
    return -1;
}

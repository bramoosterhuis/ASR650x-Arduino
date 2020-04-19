/* 
 SPI.cpp - SPI library for esp8266

 Copyright (c) 2015 Hristo Gochkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "SPI.h"
#include "project.h"


#define spi_TIMEOUT	500


SPIClass::SPIClass(int8_t spiNum)
	: _spi_num(spiNum)
	{}


void SPIClass::begin()
{	
	if(_spi_num == 0)
	{
		SPI_1_Start();
	}
	else
	{
		SPI_2_Start();
	}
}

void SPIClass::end()
{
	if(_spi_num == 0)
	{
		SPI_1_Stop();
	}
	else
	{
		SPI_2_Stop();
	}
}

void SPIClass::setFrequency(uint32_t freq)
{	
	if(_spi_num == 0)
	{
		uint32_t div = (float)CYDEV_BCLK__HFCLK__HZ / SPI_1_SPI_OVS_FACTOR / ((freq > 6000000) ? 6000000 : freq) - 1;
		SPI_1_SCBCLK_DIV_REG = div << 8 ;
	}
	else
	{
		uint32_t div = (float)CYDEV_BCLK__HFCLK__HZ / SPI_2_SPI_OVS_FACTOR / ((freq > 6000000) ? 6000000 : freq) - 1;
		SPI_2_SCBCLK_DIV_REG = div << 8 ;
	}
}

void SPIClass::usingInterrupt(int interruptNumber)
{
	//TODO: to be implemented.

	/*
	From https://www.arduino.cc/en/Reference/SPIusingInterrupt

	If your program will perform SPI transactions within an interrupt, call this function to register 
	the interrupt number or name with the SPI library. This allows SPI.beginTransaction() to prevent 
	usage conflicts. Note that the interrupt specified in the call to usingInterrupt() will be 
	disabled on a call to beginTransaction() and re-enabled in endTransaction().
	*/
}

void SPIClass::beginTransaction(SPISettings settings)
{
	if(_spi_num == 0)
	{
		while(SPI_1_SpiUartGetTxBufferSize() != 0);
	} 
	else 
	{
		while(SPI_2_SpiUartGetTxBufferSize() != 0);
	}

	setFrequency(settings._clock);
	setBitOrder(settings._bitOrder);
	setDataMode(settings._dataMode);
}

void SPIClass::endTransaction()
{
}

void SPIClass::setDataMode(uint8_t dataMode) {

    /**
     SPI_MODE0 0x00 - CPOL: 0  CPHA: 0
     SPI_MODE1 0x01 - CPOL: 0  CPHA: 1
     SPI_MODE2 0x10 - CPOL: 1  CPHA: 0
     SPI_MODE3 0x11 - CPOL: 1  CPHA: 1
     */

    bool CPOL = (dataMode & 0x10); ///< CPOL (Clock Polarity)
    bool CPHA = (dataMode & 0x01); ///< CPHA (Clock Phase)

    if(CPOL)          // Ensure same behavior as
        CPHA ^= 1;    // SAM, AVR and Intel Boards
	
	if(_spi_num == 0)
	{
		if(CPHA) {
			SPI_1_SPI_CTRL_REG |= (SPI_1_SPI_CTRL_CPHA);
		} else {
			SPI_1_SPI_CTRL_REG &= ~(SPI_1_SPI_CTRL_CPHA);
		}

		if(CPOL) {
			SPI_1_SPI_CTRL_REG |= (SPI_1_SPI_CTRL_CPOL);
		} else {
			SPI_1_SPI_CTRL_REG &= ~(SPI_1_SPI_CTRL_CPOL);
		}
	}
	else
	{
		if(CPHA) {
			SPI_2_SPI_CTRL_REG |= (SPI_2_SPI_CTRL_CPHA);
		} else {
			SPI_2_SPI_CTRL_REG &= ~(SPI_2_SPI_CTRL_CPHA);
		}

		if(CPOL) {
			SPI_2_SPI_CTRL_REG |= (SPI_2_SPI_CTRL_CPOL);
		} else {
			SPI_2_SPI_CTRL_REG &= ~(SPI_2_SPI_CTRL_CPOL);
		}
	}
}

void SPIClass::setBitOrder(uint8_t bitOrder) {
	if(_spi_num == 0)
	{
		if(bitOrder == MSBFIRST) {
			SPI_1_TX_CTRL_REG |= (SPI_1_TX_CTRL_MSB_FIRST);
			SPI_1_RX_CTRL_REG |= (SPI_1_RX_CTRL_MSB_FIRST);
		} else {
			SPI_1_TX_CTRL_REG &= ~(SPI_1_TX_CTRL_MSB_FIRST);
			SPI_1_RX_CTRL_REG &= ~(SPI_1_RX_CTRL_MSB_FIRST);
		}
	}
	else
	{
		if(bitOrder == MSBFIRST) {
			SPI_2_TX_CTRL_REG |= (SPI_2_TX_CTRL_MSB_FIRST);
			SPI_2_RX_CTRL_REG |= (SPI_2_RX_CTRL_MSB_FIRST);
		} else {
			SPI_2_TX_CTRL_REG &= ~(SPI_2_TX_CTRL_MSB_FIRST);
			SPI_2_RX_CTRL_REG &= ~(SPI_2_RX_CTRL_MSB_FIRST);
		}
	}
}

uint8_t SPIClass::transfer(uint8_t data)
{
	uint32 rxdata;
	uint32 timeout = 0;

	if(_spi_num == 0)
	{
		while(SPI_1_SpiUartGetTxBufferSize() != 0);
		SPI_1_SpiUartWriteTxData(data);
		
		while(SPI_1_SpiUartGetRxBufferSize() == 0)
		{
			timeout++;
			if(timeout > spi_TIMEOUT )break;
		}
		if(timeout > spi_TIMEOUT ){}
		else
		{
			rxdata = SPI_1_SpiUartReadRxData();
			return (uint8_t)rxdata;
		}
	}
	else
	{
		while(SPI_2_SpiUartGetTxBufferSize() != 0);
		SPI_2_SpiUartWriteTxData(data);
		
		while(SPI_2_SpiUartGetRxBufferSize() == 0)
		{
			timeout++;
			if(timeout > spi_TIMEOUT )break;
		}
		if(timeout > spi_TIMEOUT ){}
		else
		{
			rxdata = SPI_2_SpiUartReadRxData();
			return (uint8_t)rxdata;
		}
	}
}

void SPIClass::transfer(uint8_t * data, uint32_t size) 
{
	for(uint32_t i;i<size;i++)
	{
		if(_spi_num == 0)
		{
			while(SPI_1_SpiUartGetTxBufferSize() != 0);
			SPI_1_SpiUartWriteTxData(*data++);
		}
		else
		{
			while(SPI_2_SpiUartGetTxBufferSize() != 0);
			SPI_2_SpiUartWriteTxData(*data++);
		}
	}
}


/* @param data uint8_t * data buffer. can be NULL for Read Only operation
 * @param out  uint8_t * output buffer. can be NULL for Write Only operation
 * @param size uint32_t
 */
void SPIClass::transferBytes(uint8_t * data, uint8_t * out, uint32_t size)
{
	uint32 timeout = 0;
	for(uint32_t i;i<size;i++)
	{
		if(_spi_num == 0)
		{
			while(SPI_1_SpiUartGetTxBufferSize() != 0);
			SPI_1_SpiUartWriteTxData(*data++);
			
			while(SPI_1_SpiUartGetRxBufferSize() == 0)
			{
				timeout++;
				if(timeout > spi_TIMEOUT )break;
			}
			
			if(timeout > spi_TIMEOUT ){}
			else
			{
				* out++ = (uint8_t)SPI_1_SpiUartReadRxData();
			}
		}
		else
		{
			while(SPI_2_SpiUartGetTxBufferSize() != 0);
			SPI_2_SpiUartWriteTxData(*data++);

			while(SPI_2_SpiUartGetRxBufferSize() == 0)
			{
				timeout++;
				if(timeout > spi_TIMEOUT )break;
			}
			
			if(timeout > spi_TIMEOUT ){}
			else
			{
				* out++ = (uint8_t)SPI_2_SpiUartReadRxData();
			}
		}
	}
}

SPIClass SPI(SPI_NUM_0);
SPIClass SPI1(SPI_NUM_1);


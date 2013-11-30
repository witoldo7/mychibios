/*
    ChibiOS/GFX - Copyright (C) 2012
                 Joel Bodenmann aka Tectu <joel@unormal.org>

    This file is part of ChibiOS/GFX.

    ChibiOS/GFX is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    ChibiOS/GFX is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    drivers/ginput/touch/XPT2046/ginput_lld_mouse_board_example.h
 * @brief   GINPUT Touch low level driver source for the XPT2046 on the example board.
 *
 * @addtogroup GINPUT_MOUSE
 * @{
 */

#ifndef _GINPUT_LLD_MOUSE_BOARD_H
#define _GINPUT_LLD_MOUSE_BOARD_H

#define TP_CS_PORT          GPIOD
#define TP_IRQ_PORT         GPIOD

#define TP_IRQ_PIN          6
#define TP_CS_PIN           3

// Peripherial Clock 42MHz SPI2 SPI3
// Peripherial Clock 84MHz SPI1                                SPI1        SPI2/3
#define SPI_BaudRatePrescaler_2         ((uint16_t)0x0000) //  42 MHz      21 MHZ
#define SPI_BaudRatePrescaler_4         ((uint16_t)0x0008) //  21 MHz      10.5 MHz
#define SPI_BaudRatePrescaler_8         ((uint16_t)0x0010) //  10.5 MHz    5.25 MHz
#define SPI_BaudRatePrescaler_16        ((uint16_t)0x0018) //  5.25 MHz    2.626 MHz
#define SPI_BaudRatePrescaler_32        ((uint16_t)0x0020) //  2.626 MHz   1.3125 MHz
#define SPI_BaudRatePrescaler_64        ((uint16_t)0x0028) //  1.3125 MHz  656.25 KHz
#define SPI_BaudRatePrescaler_128       ((uint16_t)0x0030) //  656.25 KHz  328.125 KHz
#define SPI_BaudRatePrescaler_256       ((uint16_t)0x0038) //  328.125 KHz 164.06 KHz

static const SPIConfig spicfg = { NULL, TP_CS_PORT, TP_CS_PIN, SPI_BaudRatePrescaler_32 };

/**
 * @brief   Initialise the board for the touch.
 *
 * @notapi
 */
static __inline void init_board(void) {
    // Configure SPI3
    palSetPadMode(TP_CS_PORT, TP_CS_PIN, PAL_MODE_OUTPUT_PUSHPULL | PAL_STM32_OSPEED_HIGHEST); /* NSS.     */
    palSetPad(TP_CS_PORT, TP_CS_PIN);
    palSetPadMode(GPIOB, 3, PAL_MODE_ALTERNATE(6) | PAL_STM32_OSPEED_HIGHEST);    /* SCK.     */
    palSetPadMode(GPIOB, 4, PAL_MODE_ALTERNATE(6) | PAL_STM32_OSPEED_HIGHEST);    /* MISO.    */
    palSetPadMode(GPIOB, 5, PAL_MODE_ALTERNATE(6) | PAL_STM32_OSPEED_HIGHEST);    /* MOSI.    */

    spiStart(&SPID3, &spicfg);
}

/**
 * @brief   Check whether the surface is currently touched
 * @return	TRUE if the surface is currently touched
 *
 * @notapi
 */
static __inline bool_t getpin_pressed(void) {
	return (!palReadPad(TP_IRQ_PORT, TP_IRQ_PIN));
}
/**
 * @brief   Aquire the bus ready for readings
 *
 * @notapi
 */
static __inline void aquire_bus(void) {
	spiAcquireBus(&SPID3);
    //TOUCHSCREEN_SPI_PROLOGUE();
    palClearPad(TP_CS_PORT, TP_CS_PIN);
}

/**
 * @brief   Release the bus after readings
 *
 * @notapi
 */
static __inline void release_bus(void) {
	palSetPad(TP_CS_PORT, TP_CS_PIN);
	spiReleaseBus(&SPID3);
    //TOUCHSCREEN_SPI_EPILOGUE();
}

/**
 * @brief   Read a value from touch controller
 * @return	The value read from the controller
 *
 * params[in] port	The controller port to read.
 *
 * @notapi
 */
static __inline uint16_t read_value(uint16_t port) {
    static uint8_t txbuf[3] = {0};
    static uint8_t rxbuf[3] = {0};
    uint16_t ret;

    txbuf[0] = port;

    spiExchange(&SPID3, 3, txbuf, rxbuf);

    ret = (rxbuf[1] << 5) | (rxbuf[2] >> 3); 
    
    return ret;
}

#endif /* _GINPUT_LLD_MOUSE_BOARD_H */
/** @} */

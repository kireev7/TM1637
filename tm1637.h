/*
 * tm1637.h
 *
 *  Created on: May 26, 2025
 *      Author: Sergiy Kirieiev
 */

#ifndef TM1637_H
#define TM1637_H

#include "stm32f4xx_hal.h"  // Підключення бібліотеки HAL для STM32 (Include STM32 HAL library)


// Функції для ініціалізації та керування дисплеєм TM1637 (Functions for initializing and controlling the TM1637 display)

// Ініціалізація дисплея з зазначеними пінами CLK і DIO (Initializes the display with specified CLK and DIO pins)
// Параметри: порти і піні для синхронізації і передачі даних (Parameters: GPIO ports and pins for synchronization and data transfer)
void TM1637_Init(GPIO_TypeDef *clkPort, uint16_t clkPin, GPIO_TypeDef *dioPort, uint16_t dioPin);

// Відображення однієї цифри (0-9) на зазначеній позиції дисплея (Displays a single digit (0-9) at the specified position (0-3) on the display)
void TM1637_DisplayDigit(uint8_t digit, uint8_t position);

// Відображення числа (від -999 до 9999) на дисплеї (Displays a number (from -999 to 9999) on the display)
void TM1637_DisplayNumber(int16_t number);

// Встановлення рівня яскравості дисплея (від 0 до 7) (Sets the display brightness level (from 0 to 7))
void TM1637_SetBrightness(uint8_t brightness);

// Очищення дисплея, вимикаючи всі сегменти (Clears the display by turning off all segments)
void TM1637_ClearDisplay(void);

#endif

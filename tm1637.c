/*
 * tm1637.c
 *
 *  Created on: May 26, 2025
 *      Author: Sergiy
 */
#include "tm1637.h"

// Затримки для протоколу TM1637 (Delays for TM1637 protocol)
#define TM1637_DELAY_US 10

// Сегменти для цифр 0-9 (Segments for digits 0-9)
static const uint8_t segmentMap[] = {
    0x3F, // 0
    0x06, // 1
    0x5B, // 2
    0x4F, // 3
    0x66, // 4
    0x6D, // 5
    0x7D, // 6
    0x07, // 7
    0x7F, // 8
    0x6F  // 9
};

// Змінні для зберігання пінів (Variables to store pin information)
static GPIO_TypeDef *TM1637_CLK_PORT;
static uint16_t TM1637_CLK_PIN;
static GPIO_TypeDef *TM1637_DIO_PORT;
static uint16_t TM1637_DIO_PIN;

// Функція затримки в мікросекундах (Function for microsecond delay)
void TM1637_DelayUs(uint32_t us) {
    volatile uint32_t count = us * (SystemCoreClock / 1000000) / 8;
    while (count--) {}
}

// Налаштування піна DIO як вихід (Set DIO pin as output)
void TM1637_SetDIOOutput(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TM1637_DIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; // Open-drain для TM1637 (Open-drain for TM1637)
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TM1637_DIO_PORT, &GPIO_InitStruct);
}

// Налаштування піна DIO як вхід (Set DIO pin as input)
void TM1637_SetDIOInput(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = TM1637_DIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(TM1637_DIO_PORT, &GPIO_InitStruct);
}

// Початок передачі (Start transmission)
void TM1637_Start(void) {
    HAL_GPIO_WritePin(TM1637_CLK_PORT, TM1637_CLK_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(TM1637_DIO_PORT, TM1637_DIO_PIN, GPIO_PIN_SET);
    TM1637_DelayUs(TM1637_DELAY_US);
    HAL_GPIO_WritePin(TM1637_DIO_PORT, TM1637_DIO_PIN, GPIO_PIN_RESET);
    TM1637_DelayUs(TM1637_DELAY_US);
    HAL_GPIO_WritePin(TM1637_CLK_PORT, TM1637_CLK_PIN, GPIO_PIN_RESET);
    TM1637_DelayUs(TM1637_DELAY_US);
}

// Кінець передачі (Stop transmission)
void TM1637_Stop(void) {
    HAL_GPIO_WritePin(TM1637_CLK_PORT, TM1637_CLK_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TM1637_DIO_PORT, TM1637_DIO_PIN, GPIO_PIN_RESET);
    TM1637_DelayUs(TM1637_DELAY_US);
    HAL_GPIO_WritePin(TM1637_CLK_PORT, TM1637_CLK_PIN, GPIO_PIN_SET);
    TM1637_DelayUs(TM1637_DELAY_US);
    HAL_GPIO_WritePin(TM1637_DIO_PORT, TM1637_DIO_PIN, GPIO_PIN_SET);
}

// Відправка байта (Send byte)
void TM1637_WriteByte(uint8_t data) {
    for (uint8_t i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(TM1637_CLK_PORT, TM1637_CLK_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(TM1637_DIO_PORT, TM1637_DIO_PIN, (data & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        TM1637_DelayUs(TM1637_DELAY_US);
        HAL_GPIO_WritePin(TM1637_CLK_PORT, TM1637_CLK_PIN, GPIO_PIN_SET);
        TM1637_DelayUs(TM1637_DELAY_US);
        data >>= 1;
    }
    // Пропустити перевірку ACK для спрощення (Skip ACK check for simplicity)
    HAL_GPIO_WritePin(TM1637_CLK_PORT, TM1637_CLK_PIN, GPIO_PIN_RESET);
    TM1637_DelayUs(TM1637_DELAY_US);
    HAL_GPIO_WritePin(TM1637_CLK_PORT, TM1637_CLK_PIN, GPIO_PIN_SET);
    TM1637_DelayUs(TM1637_DELAY_US);
}

// Ініціалізація дисплея (Initialize display)
void TM1637_Init(GPIO_TypeDef *clkPort, uint16_t clkPin, GPIO_TypeDef *dioPort, uint16_t dioPin) {
    // Збереження параметрів пінів (Save pin parameters)
    TM1637_CLK_PORT = clkPort;
    TM1637_CLK_PIN = clkPin;
    TM1637_DIO_PORT = dioPort;
    TM1637_DIO_PIN = dioPin;

    // Ініціалізація пінів (Initialize pins)
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE(); // Увімкнення тактування порту (Enable clock for port)

    GPIO_InitStruct.Pin = clkPin | dioPin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD; // Open-drain для TM1637 (Open-drain for TM1637)
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(clkPort, &GPIO_InitStruct);

    // Встановлення максимальної яскравості (Set maximum brightness)
    TM1637_SetBrightness(7);
}

// Встановлення яскравості (Set brightness)
void TM1637_SetBrightness(uint8_t brightness) {
    if (brightness > 7) brightness = 7;
    TM1637_Start();
    TM1637_WriteByte(0x88 | brightness); // Команда для яскравості (Command for brightness)
    TM1637_Stop();
}

// Відображення однієї цифри (Display one digit)
void TM1637_DisplayDigit(uint8_t digit, uint8_t position) {
    if (digit > 9) digit = 0;
    TM1637_Start();
    TM1637_WriteByte(0x44); // Команда запису даних (Command to write data)
    TM1637_Stop();
    TM1637_Start();
    TM1637_WriteByte(0xC0 + position); // Адреса позиції (Position address)
    TM1637_WriteByte(segmentMap[digit]); // Дані для цифри (Data for the digit)
    TM1637_Stop();
}

// Очищення дисплея (Clear display)
void TM1637_ClearDisplay(void) {
    TM1637_Start();
    TM1637_WriteByte(0x44); // Команда запису даних (Command to write data)
    TM1637_Stop();

    for (uint8_t position = 0; position < 4; position++) {
        TM1637_Start();
        TM1637_WriteByte(0xC0 + position); // Адреса позиції (Position address)
        TM1637_WriteByte(0x00); // Вимкнути всі сегменти (Turn off all segments)
        TM1637_Stop();
    }
}

// Відображення числа (Display number)
void TM1637_DisplayNumber(int16_t number) {
    if (number > 9999 || number < -999) return;

    uint8_t digits[4] = {0};
    uint8_t pos = 0;
    uint8_t isNegative = 0;

    if (number < 0) {
        isNegative = 1;
        number = -number;
    }

    // Очищення дисплея (Clear display)
    TM1637_ClearDisplay();

    // Розбиваємо число на цифри (Break number into digits)
    do {
        digits[pos++] = number % 10;
        number /= 10;
    } while (number > 0 && pos < 4);

    // Відображаємо цифри зліва направо (Display digits from left to right)
    for (uint8_t i = 0; i < 4; i++) {
        uint8_t displayPos = 3 - i; // Зміна порядку: 0 -> 3, 1 -> 2, 2 -> 1, 3 -> 0 (Change order: 0 -> 3, 1 -> 2, 2 -> 1, 3 -> 0)
        TM1637_DisplayDigit(digits[i], displayPos);
    }
}

//Source: https://github.com/STM32-pio-libs/I2C-LCD-PCF8574
//Author of the driver: Anurag Kumar Singh

#include "i2c_lcd.h"

#define LCD_RS_BIT 0U
#define LCD_EN_BIT 2U
#define LCD_BL_BIT 3U
#define LCD_D4_BIT 4U

static uint8_t i2c_lcd_is_valid_config(const I2C_LCD_Config *config) {
    if (config == NULL) {
        return 0U;
    }

    if ((config->write_fn == NULL) || (config->delay_ms_fn == NULL)) {
        return 0U;
    }

    if ((config->row_count == 0U) || (config->row_count > 4U) || (config->col_count == 0U)) {
        return 0U;
    }

    return 1U;
}

static uint8_t i2c_lcd_is_ready(const I2C_LCD_HandleTypeDef *lcd) {
    if ((lcd == NULL) || (lcd->initialized == 0U)) {
        return 0U;
    }

    return i2c_lcd_is_valid_config(&lcd->config);
}

static void i2c_lcd_delay_ms(I2C_LCD_HandleTypeDef *lcd, uint32_t delay_ms) {
    lcd->config.delay_ms_fn(lcd->config.user_context, delay_ms);
}

static int32_t i2c_lcd_write_raw(I2C_LCD_HandleTypeDef *lcd, uint8_t value) {
    return lcd->config.write_fn(lcd->config.user_context,
                                lcd->config.i2c_address_7bit,
                                &value,
                                1U);
}

static uint8_t i2c_lcd_expander_value(const I2C_LCD_HandleTypeDef *lcd,
                                      uint8_t nibble,
                                      uint8_t rs) {
    uint8_t value = (uint8_t)((nibble & 0x0FU) << LCD_D4_BIT);

    if (rs != 0U) {
        value |= (uint8_t)(1U << LCD_RS_BIT);
    }

    if (lcd->config.backlight_on != 0U) {
        value |= (uint8_t)(1U << LCD_BL_BIT);
    }

    return value;
}

static int32_t i2c_lcd_pulse_enable(I2C_LCD_HandleTypeDef *lcd, uint8_t value) {
    int32_t status;

    status = i2c_lcd_write_raw(lcd, (uint8_t)(value | (uint8_t)(1U << LCD_EN_BIT)));
    if (status != I2C_LCD_OK) {
        return I2C_LCD_ERR_IO;
    }

    i2c_lcd_delay_ms(lcd, 1U);

    status = i2c_lcd_write_raw(lcd, (uint8_t)(value & (uint8_t)~(1U << LCD_EN_BIT)));
    if (status != I2C_LCD_OK) {
        return I2C_LCD_ERR_IO;
    }

    i2c_lcd_delay_ms(lcd, 1U);
    return I2C_LCD_OK;
}

static int32_t i2c_lcd_write_nibble(I2C_LCD_HandleTypeDef *lcd, uint8_t nibble, uint8_t rs) {
    return i2c_lcd_pulse_enable(lcd, i2c_lcd_expander_value(lcd, nibble, rs));
}

static int32_t i2c_lcd_send(I2C_LCD_HandleTypeDef *lcd, uint8_t value, uint8_t rs) {
    int32_t status;

    status = i2c_lcd_write_nibble(lcd, (uint8_t)(value >> 4U), rs);
    if (status != I2C_LCD_OK) {
        return status;
    }

    status = i2c_lcd_write_nibble(lcd, (uint8_t)(value & 0x0FU), rs);
    if (status != I2C_LCD_OK) {
        return status;
    }

    return I2C_LCD_OK;
}

void I2C_LCD_SetDefaultRowOffsets(I2C_LCD_HandleTypeDef *lcd) {
    if (lcd == NULL) {
        return;
    }

    lcd->row_offsets[0] = 0x00U;
    lcd->row_offsets[1] = 0x40U;
    lcd->row_offsets[2] = lcd->config.col_count;
    lcd->row_offsets[3] = (uint8_t)(0x40U + lcd->config.col_count);
}

int32_t I2C_LCD_SetRowOffsets(I2C_LCD_HandleTypeDef *lcd,
                              uint8_t row0,
                              uint8_t row1,
                              uint8_t row2,
                              uint8_t row3) {
    if (i2c_lcd_is_ready(lcd) == 0U) {
        return I2C_LCD_ERR_INVALID_ARG;
    }

    lcd->row_offsets[0] = row0;
    lcd->row_offsets[1] = row1;
    lcd->row_offsets[2] = row2;
    lcd->row_offsets[3] = row3;

    return I2C_LCD_OK;
}

int32_t I2C_LCD_Command(I2C_LCD_HandleTypeDef *lcd, uint8_t command) {
    if (i2c_lcd_is_ready(lcd) == 0U) {
        return I2C_LCD_ERR_INVALID_ARG;
    }

    if (i2c_lcd_send(lcd, command, 0U) != I2C_LCD_OK) {
        return I2C_LCD_ERR_IO;
    }

    if ((command == LCD_CLEARDISPLAY) || (command == LCD_RETURNHOME)) {
        i2c_lcd_delay_ms(lcd, 2U);
    }

    return I2C_LCD_OK;
}

int32_t I2C_LCD_Init(I2C_LCD_HandleTypeDef *lcd, const I2C_LCD_Config *config) {
    if ((lcd == NULL) || (i2c_lcd_is_valid_config(config) == 0U)) {
        return I2C_LCD_ERR_INVALID_ARG;
    }

    lcd->config = *config;
    lcd->display_function = (uint8_t)(LCD_FUNCTIONSET | LCD_4BITMODE | LCD_5x8DOTS);
    if (lcd->config.row_count > 1U) {
        lcd->display_function |= LCD_2LINE;
    }
    else {
        lcd->display_function |= LCD_1LINE;
    }

    lcd->display_control = (uint8_t)(LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF);
    lcd->display_mode = (uint8_t)(LCD_ENTRYMODESET | LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT);
    lcd->initialized = 1U;
    I2C_LCD_SetDefaultRowOffsets(lcd);

    i2c_lcd_delay_ms(lcd, 50U);

    if (i2c_lcd_write_nibble(lcd, 0x03U, 0U) != I2C_LCD_OK) {
        lcd->initialized = 0U;
        return I2C_LCD_ERR_IO;
    }
    i2c_lcd_delay_ms(lcd, 5U);

    if (i2c_lcd_write_nibble(lcd, 0x03U, 0U) != I2C_LCD_OK) {
        lcd->initialized = 0U;
        return I2C_LCD_ERR_IO;
    }
    i2c_lcd_delay_ms(lcd, 1U);

    if (i2c_lcd_write_nibble(lcd, 0x03U, 0U) != I2C_LCD_OK) {
        lcd->initialized = 0U;
        return I2C_LCD_ERR_IO;
    }
    i2c_lcd_delay_ms(lcd, 1U);

    if (i2c_lcd_write_nibble(lcd, 0x02U, 0U) != I2C_LCD_OK) {
        lcd->initialized = 0U;
        return I2C_LCD_ERR_IO;
    }

    if (I2C_LCD_Command(lcd, lcd->display_function) != I2C_LCD_OK) {
        lcd->initialized = 0U;
        return I2C_LCD_ERR_IO;
    }

    if (I2C_LCD_Command(lcd, lcd->display_control) != I2C_LCD_OK) {
        lcd->initialized = 0U;
        return I2C_LCD_ERR_IO;
    }

    if (I2C_LCD_Clear(lcd) != I2C_LCD_OK) {
        lcd->initialized = 0U;
        return I2C_LCD_ERR_IO;
    }

    if (I2C_LCD_Command(lcd, lcd->display_mode) != I2C_LCD_OK) {
        lcd->initialized = 0U;
        return I2C_LCD_ERR_IO;
    }

    return I2C_LCD_OK;
}

int32_t I2C_LCD_Clear(I2C_LCD_HandleTypeDef *lcd) {
    return I2C_LCD_Command(lcd, LCD_CLEARDISPLAY);
}

int32_t I2C_LCD_Home(I2C_LCD_HandleTypeDef *lcd) {
    return I2C_LCD_Command(lcd, LCD_RETURNHOME);
}

int32_t I2C_LCD_SetCursor(I2C_LCD_HandleTypeDef *lcd, uint8_t row, uint8_t column) {
    if (i2c_lcd_is_ready(lcd) == 0U) {
        return I2C_LCD_ERR_INVALID_ARG;
    }

    if ((row >= lcd->config.row_count) || (column >= lcd->config.col_count)) {
        return I2C_LCD_ERR_INVALID_ARG;
    }

    return I2C_LCD_Command(lcd, (uint8_t)(LCD_SETDDRAMADDR | (lcd->row_offsets[row] + column)));
}

int32_t I2C_LCD_WriteChar(I2C_LCD_HandleTypeDef *lcd, char value) {
    if (i2c_lcd_is_ready(lcd) == 0U) {
        return I2C_LCD_ERR_INVALID_ARG;
    }

    if (i2c_lcd_send(lcd, (uint8_t)value, 1U) != I2C_LCD_OK) {
        return I2C_LCD_ERR_IO;
    }

    return I2C_LCD_OK;
}

int32_t I2C_LCD_WriteString(I2C_LCD_HandleTypeDef *lcd, const char *str) {
    size_t index;

    if ((i2c_lcd_is_ready(lcd) == 0U) || (str == NULL)) {
        return I2C_LCD_ERR_INVALID_ARG;
    }

    for (index = 0U; str[index] != '\0'; index++) {
        if (I2C_LCD_WriteChar(lcd, str[index]) != I2C_LCD_OK) {
            return I2C_LCD_ERR_IO;
        }
    }

    return I2C_LCD_OK;
}

int32_t I2C_LCD_Display(I2C_LCD_HandleTypeDef *lcd, uint8_t enable) {
    if (i2c_lcd_is_ready(lcd) == 0U) {
        return I2C_LCD_ERR_INVALID_ARG;
    }

    if (enable != 0U) {
        lcd->display_control |= LCD_DISPLAYON;
    }
    else {
        lcd->display_control &= (uint8_t)~LCD_DISPLAYON;
    }

    return I2C_LCD_Command(lcd, lcd->display_control);
}

int32_t I2C_LCD_Cursor(I2C_LCD_HandleTypeDef *lcd, uint8_t enable) {
    if (i2c_lcd_is_ready(lcd) == 0U) {
        return I2C_LCD_ERR_INVALID_ARG;
    }

    if (enable != 0U) {
        lcd->display_control |= LCD_CURSORON;
    }
    else {
        lcd->display_control &= (uint8_t)~LCD_CURSORON;
    }

    return I2C_LCD_Command(lcd, lcd->display_control);
}

int32_t I2C_LCD_Blink(I2C_LCD_HandleTypeDef *lcd, uint8_t enable) {
    if (i2c_lcd_is_ready(lcd) == 0U) {
        return I2C_LCD_ERR_INVALID_ARG;
    }

    if (enable != 0U) {
        lcd->display_control |= LCD_BLINKON;
    }
    else {
        lcd->display_control &= (uint8_t)~LCD_BLINKON;
    }

    return I2C_LCD_Command(lcd, lcd->display_control);
}

int32_t I2C_LCD_Backlight(I2C_LCD_HandleTypeDef *lcd, uint8_t enable) {
    if (i2c_lcd_is_ready(lcd) == 0U) {
        return I2C_LCD_ERR_INVALID_ARG;
    }

    lcd->config.backlight_on = (enable != 0U) ? 1U : 0U;

    if (i2c_lcd_write_raw(lcd, i2c_lcd_expander_value(lcd, 0U, 0U)) != I2C_LCD_OK) {
        return I2C_LCD_ERR_IO;
    }

    return I2C_LCD_OK;
}

int32_t I2C_LCD_CreateChar(I2C_LCD_HandleTypeDef *lcd, uint8_t location, const uint8_t bitmap[8]) {
    size_t index;

    if ((i2c_lcd_is_ready(lcd) == 0U) || (bitmap == NULL) || (location >= 8U)) {
        return I2C_LCD_ERR_INVALID_ARG;
    }

    if (I2C_LCD_Command(lcd, (uint8_t)(LCD_SETCGRAMADDR | (location << 3U))) != I2C_LCD_OK) {
        return I2C_LCD_ERR_IO;
    }

    for (index = 0U; index < 8U; index++) {
        if (I2C_LCD_WriteChar(lcd, (char)bitmap[index]) != I2C_LCD_OK) {
            return I2C_LCD_ERR_IO;
        }
    }

    return I2C_LCD_OK;
}

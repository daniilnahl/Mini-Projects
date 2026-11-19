#ifndef I2C_LCD_H
#define I2C_LCD_H

#include <stddef.h>
#include <stdint.h>

#define I2C_LCD_OK 0
#define I2C_LCD_ERR_INVALID_ARG (-1)
#define I2C_LCD_ERR_IO (-2)

/* Commands */
#define LCD_CLEARDISPLAY 0x01U
#define LCD_RETURNHOME 0x02U
#define LCD_ENTRYMODESET 0x04U
#define LCD_DISPLAYCONTROL 0x08U
#define LCD_CURSORSHIFT 0x10U
#define LCD_FUNCTIONSET 0x20U
#define LCD_SETCGRAMADDR 0x40U
#define LCD_SETDDRAMADDR 0x80U

/* Flags for display entry mode */
#define LCD_ENTRYRIGHT 0x00U
#define LCD_ENTRYLEFT 0x02U
#define LCD_ENTRYSHIFTINCREMENT 0x01U
#define LCD_ENTRYSHIFTDECREMENT 0x00U

/* Flags for display on/off control */
#define LCD_DISPLAYON 0x04U
#define LCD_DISPLAYOFF 0x00U
#define LCD_CURSORON 0x02U
#define LCD_CURSOROFF 0x00U
#define LCD_BLINKON 0x01U
#define LCD_BLINKOFF 0x00U

/* Flags for display/cursor shift */
#define LCD_DISPLAYMOVE 0x08U
#define LCD_CURSORMOVE 0x00U
#define LCD_MOVERIGHT 0x04U
#define LCD_MOVELEFT 0x00U

/* Flags for function set */
#define LCD_8BITMODE 0x10U
#define LCD_4BITMODE 0x00U
#define LCD_2LINE 0x08U
#define LCD_1LINE 0x00U
#define LCD_5x10DOTS 0x04U
#define LCD_5x8DOTS 0x00U

typedef int32_t (*I2C_LCD_WriteFn)(void *user_context,
                                   uint8_t i2c_address_7bit,
                                   const uint8_t *data,
                                   size_t length);

typedef void (*I2C_LCD_DelayMsFn)(void *user_context, uint32_t delay_ms);

typedef struct {
    uint8_t i2c_address_7bit;
    uint8_t row_count;
    uint8_t col_count;
    uint8_t backlight_on;
    void *user_context;
    I2C_LCD_WriteFn write_fn;
    I2C_LCD_DelayMsFn delay_ms_fn;
} I2C_LCD_Config;

typedef struct {
    I2C_LCD_Config config;
    uint8_t display_function;
    uint8_t display_control;
    uint8_t display_mode;
    uint8_t row_offsets[4];
    uint8_t initialized;
} I2C_LCD_HandleTypeDef;

int32_t I2C_LCD_Init(I2C_LCD_HandleTypeDef *lcd, const I2C_LCD_Config *config);
int32_t I2C_LCD_Clear(I2C_LCD_HandleTypeDef *lcd);
int32_t I2C_LCD_Home(I2C_LCD_HandleTypeDef *lcd);
int32_t I2C_LCD_SetCursor(I2C_LCD_HandleTypeDef *lcd, uint8_t row, uint8_t column);
int32_t I2C_LCD_WriteChar(I2C_LCD_HandleTypeDef *lcd, char value);
int32_t I2C_LCD_WriteString(I2C_LCD_HandleTypeDef *lcd, const char *str);
int32_t I2C_LCD_Display(I2C_LCD_HandleTypeDef *lcd, uint8_t enable);
int32_t I2C_LCD_Cursor(I2C_LCD_HandleTypeDef *lcd, uint8_t enable);
int32_t I2C_LCD_Blink(I2C_LCD_HandleTypeDef *lcd, uint8_t enable);
int32_t I2C_LCD_Backlight(I2C_LCD_HandleTypeDef *lcd, uint8_t enable);
int32_t I2C_LCD_CreateChar(I2C_LCD_HandleTypeDef *lcd, uint8_t location, const uint8_t bitmap[8]);
int32_t I2C_LCD_Command(I2C_LCD_HandleTypeDef *lcd, uint8_t command);
void I2C_LCD_SetDefaultRowOffsets(I2C_LCD_HandleTypeDef *lcd);
int32_t I2C_LCD_SetRowOffsets(I2C_LCD_HandleTypeDef *lcd,
                              uint8_t row0,
                              uint8_t row1,
                              uint8_t row2,
                              uint8_t row3);

/*
 * Lowercase wrappers for compatibility with older examples.
 * Prefer the explicit I2C_LCD_* API in new code.
 */
static inline int32_t lcd_init(I2C_LCD_HandleTypeDef *lcd, const I2C_LCD_Config *config) {
    return I2C_LCD_Init(lcd, config);
}

static inline int32_t lcd_write_string(I2C_LCD_HandleTypeDef *lcd, const char *str) {
    return I2C_LCD_WriteString(lcd, str);
}

static inline int32_t lcd_clear(I2C_LCD_HandleTypeDef *lcd) {
    return I2C_LCD_Clear(lcd);
}

static inline int32_t lcd_set_cursor(I2C_LCD_HandleTypeDef *lcd, uint8_t row, uint8_t column) {
    return I2C_LCD_SetCursor(lcd, row, column);
}

#endif

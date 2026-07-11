/**
 ******************************************************************************
 * @file    lcd_i2c.c
 * @brief   16x2 character LCD driver over I2C (PCF8574 backpack, HD44780)
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 * PCF8574 bit mapping (standard "blue backpack" wiring):
 *   P0 = RS   P1 = RW   P2 = E   P3 = Backlight
 *   P4..P7    = D4..D7 (4-bit mode)
 ******************************************************************************
 */

#include "lcd_i2c.h"
#include <string.h>

#define LCD_BIT_RS        0x01U
#define LCD_BIT_RW         0x02U
#define LCD_BIT_EN         0x04U
#define LCD_BIT_BACKLIGHT  0x08U

/* HD44780 command set (subset used here) */
#define LCD_CMD_CLEAR_DISPLAY      0x01U
#define LCD_CMD_RETURN_HOME        0x02U
#define LCD_CMD_ENTRY_MODE_SET     0x06U /* increment, no shift */
#define LCD_CMD_DISPLAY_ON         0x0CU /* display on, cursor off, blink off */
#define LCD_CMD_FUNCTION_SET_4BIT  0x28U /* 4-bit, 2 line, 5x8 font */

static I2C_HandleTypeDef *s_hi2c = NULL;
static bool                s_backlight_on = true;
static bool                s_fault = false;

static const uint8_t s_row_offsets[2] = { 0x00U, 0x40U };

static bool lcd_i2c_write_raw(uint8_t data)
{
    HAL_StatusTypeDef status =
        HAL_I2C_Master_Transmit(s_hi2c, (uint16_t)(LCD_I2C_ADDRESS << 1),
                                 &data, 1U, 20U);

    s_fault = (status != HAL_OK);
    return (status == HAL_OK);
}

static void lcd_i2c_pulse_enable(uint8_t data)
{
    lcd_i2c_write_raw(data | LCD_BIT_EN);
    HAL_Delay(1U);
    lcd_i2c_write_raw(data & (uint8_t)(~LCD_BIT_EN));
    HAL_Delay(1U);
}

static void lcd_i2c_write4(uint8_t nibble, uint8_t control_bits)
{
    uint8_t backlight = s_backlight_on ? LCD_BIT_BACKLIGHT : 0U;
    uint8_t data = (uint8_t)((nibble & 0xF0U) | control_bits | backlight);
    lcd_i2c_pulse_enable(data);
}

static void lcd_i2c_send_byte(uint8_t value, bool is_data)
{
    uint8_t control = is_data ? LCD_BIT_RS : 0x00U;
    lcd_i2c_write4((uint8_t)(value & 0xF0U), control);
    lcd_i2c_write4((uint8_t)((value << 4) & 0xF0U), control);
}

static void lcd_i2c_send_command(uint8_t cmd)
{
    lcd_i2c_send_byte(cmd, false);
}

static void lcd_i2c_send_data(uint8_t data)
{
    lcd_i2c_send_byte(data, true);
}

bool lcd_i2c_init(I2C_HandleTypeDef *hi2c)
{
    s_hi2c = hi2c;
    s_fault = false;
    s_backlight_on = true;

    /* Probe device presence first */
    if (HAL_I2C_IsDeviceReady(s_hi2c, (uint16_t)(LCD_I2C_ADDRESS << 1), 3U, 50U) != HAL_OK)
    {
        s_fault = true;
        return false;
    }

    HAL_Delay(50U); /* HD44780 power-on settle time */

    /* Force 4-bit mode bring-up sequence (per HD44780 datasheet fig.24) */
    lcd_i2c_write4(0x30U, 0x00U);
    HAL_Delay(5U);
    lcd_i2c_write4(0x30U, 0x00U);
    HAL_Delay(1U);
    lcd_i2c_write4(0x30U, 0x00U);
    HAL_Delay(1U);
    lcd_i2c_write4(0x20U, 0x00U); /* switch to 4-bit interface */
    HAL_Delay(1U);

    lcd_i2c_send_command(LCD_CMD_FUNCTION_SET_4BIT);
    lcd_i2c_send_command(LCD_CMD_DISPLAY_ON);
    lcd_i2c_send_command(LCD_CMD_CLEAR_DISPLAY);
    HAL_Delay(2U);
    lcd_i2c_send_command(LCD_CMD_ENTRY_MODE_SET);

    return !s_fault;
}

void lcd_i2c_clear(void)
{
    lcd_i2c_send_command(LCD_CMD_CLEAR_DISPLAY);
    HAL_Delay(2U);
}

void lcd_i2c_set_cursor(uint8_t row, uint8_t col)
{
    if (row >= LCD_ROWS) row = (uint8_t)(LCD_ROWS - 1U);
    if (col >= LCD_COLUMNS) col = (uint8_t)(LCD_COLUMNS - 1U);

    uint8_t address = (uint8_t)(s_row_offsets[row] + col);
    lcd_i2c_send_command((uint8_t)(0x80U | address));
}

void lcd_i2c_print(const char *str)
{
    while (*str != '\0')
    {
        lcd_i2c_send_data((uint8_t)*str);
        str++;
    }
}

void lcd_i2c_print_line(uint8_t row, const char *str)
{
    char padded[LCD_COLUMNS + 1U];
    memset(padded, ' ', LCD_COLUMNS);
    padded[LCD_COLUMNS] = '\0';

    size_t len = strlen(str);
    if (len > LCD_COLUMNS)
    {
        len = LCD_COLUMNS;
    }
    memcpy(padded, str, len);

    lcd_i2c_set_cursor(row, 0U);
    lcd_i2c_print(padded);
}

void lcd_i2c_set_backlight(bool on)
{
    s_backlight_on = on;
    /* Re-send a no-op nibble so the backlight bit takes effect immediately */
    lcd_i2c_write_raw(s_backlight_on ? LCD_BIT_BACKLIGHT : 0x00U);
}

bool lcd_i2c_has_fault(void)
{
    return s_fault;
}

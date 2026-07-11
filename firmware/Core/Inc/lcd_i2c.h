/**
 ******************************************************************************
 * @file    lcd_i2c.h
 * @brief   16x2 character LCD driver over I2C (PCF8574 backpack, HD44780)
 * @project Smart-Parking-Assistance-System
 ******************************************************************************
 */

#ifndef LCD_I2C_H
#define LCD_I2C_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32g0xx_hal.h"
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the LCD: 4-bit mode bring-up sequence per HD44780
 *        datasheet, then clear display and turn on with cursor hidden.
 *
 * @param hi2c    I2C handle connected to the PCF8574 backpack.
 * @return true on success, false if the device NACKed (wiring/address fault).
 */
bool lcd_i2c_init(I2C_HandleTypeDef *hi2c);

/**
 * @brief Clear the entire display and return cursor to (0,0).
 */
void lcd_i2c_clear(void);

/**
 * @brief Move the cursor to a specific row/column (0-indexed).
 */
void lcd_i2c_set_cursor(uint8_t row, uint8_t col);

/**
 * @brief Print a null-terminated string at the current cursor position.
 *        Strings longer than the remaining columns on the row are truncated.
 */
void lcd_i2c_print(const char *str);

/**
 * @brief Convenience helper: writes a string at a specific row/col, padding
 *        the remainder of the row with spaces so stale characters from a
 *        previous, longer string are overwritten.
 */
void lcd_i2c_print_line(uint8_t row, const char *str);

/**
 * @brief Turn the backlight on or off.
 */
void lcd_i2c_set_backlight(bool on);

/**
 * @brief Returns true if the last I2C transaction to the LCD failed.
 *        Used by the FSM to raise FAULT_I2C_LCD_NACK.
 */
bool lcd_i2c_has_fault(void);

#ifdef __cplusplus
}
#endif

#endif /* LCD_I2C_H */

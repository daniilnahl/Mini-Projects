# STM32F746 — Keypad Safe Lock

A register-level C project for **TCES 430 Microprocessors, Lab 5**. Uses a keypad to enter a four-key PIN and an I2C LCD to display masked input and lockout time.

## Behavior

- Default PIN: `1234`.
- Each accepted key shows `*` and briefly flashes the LED.
- Correct PIN displays `Access Granted!` and lights the LED for two seconds.
- Three wrong attempts trigger a 30-second countdown with alternating LED flashes. Keypad input is ignored during lockout.

The LED replaces the lab's buzzer.

## Hardware

STM32F746G-DISCO, 4×4 keypad, I2C 1602 LCD, LED with a series resistor, and jumper wires.

## Implementation

GPIO registers, EXTI keypad interrupts, polled timers, and register-level I2C. Main application: [main.c](Core/Src/main.c).

LCD driver: [STM32 I2C-LCD](https://github.com/STM32-pio-libs/I2C-LCD).

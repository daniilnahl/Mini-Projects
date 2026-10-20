# STM32F746 — 7-Segment Calculator

## Overview

Developed a basic calculator using the STM32F746G-DISCO development board and a four-digit seven-segment display as part of TCES 430 Microprocessors, Lab 2.

The project demonstrates GPIO interfacing, display multiplexing, and switches input handling using register-level C programming.

## Implementation

- Configured GPIO outputs to control seven LED segments and four common-anode digit lines.
- Implemented display multiplexing using a 1 ms SysTick interrupt.
- Configured four switches with internal pull-up resistors and software debouncing.
- Implemented addition, subtraction, and previous-result memory functionality.
- Used current-limiting resistors to drive the display directly from GPIO pins.

## Hardware

- STM32F746G-DISCO (ARM Cortex-M7)
- LTC-4627JR four-digit seven-segment display
- 4 switches component
- 2.2 kΩ resistors
- Breadboard and jumper wires

## Technologies

**Bare-metal C | STM32F746 | ARM Cortex-M7 | GPIO | SysTick**

## Demonstration

[YouTube Short Link](https://youtube.com/shorts/5lr_JKF9Ua8?si=BYsB8jXs40kzr8TI)

## Learning Outcomes

Gained practical experience with GPIO configuration, timer interrupts, LED multiplexing, switch debouncing, and troubleshooting hardware & software integration on an ARM Cortex-M7 microcontroller.
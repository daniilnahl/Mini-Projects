# STM32H743 GPIO — RGB LED & Button Control

## Overview

Developed an embedded C application for the STM32H743 microcontroller as part of TCES 430 — Microprocessors, Lab 1.

The project demonstrates direct memory-mapped register programming to configure GPIO peripherals, control RGB LED outputs, and read a push-button input.

## Implementation

- Enabled the GPIOA peripheral clock through the RCC_AHB4ENR register.
- Configured PA0–PA2 as general-purpose push-pull outputs.
- Configured PA3 as a digital input with an internal pull-up resistor.
- Used GPIOA_ODR to control LED output states.
- Used GPIOA_IDR to detect an active-low push button.
- Implemented an LED color sequence using C arrays and software timing.

## Hardware

- STM32H743 microcontroller (custom development board)
- RGB LED
- External tactile push button
- Breadboard and jumper wires

## Technologies

**Embedded C | STM32H743 | ARM Cortex-M7 | GPIO | Register-Level Programming | STM32CubeIDE**

## Demonstration

[YouTube Shorts](https://youtube.com/shorts/9dJCH0697pM?si=devEvuCu58ATW0_-)

## Learning Outcomes

Gained practical experience working with memory-mapped peripheral registers, bitwise operations, GPIO input/output configuration, and hardware-software integration on an ARM Cortex-M7 microcontroller.

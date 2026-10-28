# STM32F746 — Nested Interrupts & LCD Status Display

## Overview

An STM32F746G-DISCO application developed for **TCES 430 Microprocessors, Lab 3**. The firmware demonstrates prioritized external interrupts, Cortex-M7 NVIC preemption, timer-driven LED sequencing, and a character LCD connected through a 4-bit GPIO interface.

Normal operation alternates **five red blinks** and **eight green blinks**, followed by a two-second pause. Two active-low push buttons introduce interrupt-driven patterns:

- **SW1 (priority 5):** Three blue blinks; the interrupted normal sequence resumes afterward.
- **SW2 (priority 2):** Five blinks with all three RGB channels on (`LGHT`); it can preempt SW1. When SW2 finishes, SW1 resumes and then returns to the normal sequence.

The 1602A LCD shows the current operating mode, LED color, and blink number, for example `NOR > RED > 3`, `INT1 > BLUE > 2`, or `INT2 > LGHT > 4`.

## Implementation

- Configured **EXTI2** on PI2 and **EXTI6** on PG6 for falling-edge events using `SYSCFG`, `EXTI`, and NVIC registers.
- Assigned **priority 5 to EXTI2** and **priority 2 to EXTI9_5** so the second interrupt can preempt the first.
- Used **TIM2** as a polled 250-ms sequencer and **TIM5** as a free-running 1-MHz delay counter.
- Paused the TIM2 counter while interrupt routines execute; measured the higher-priority ISR duration to preserve the remaining SW1 blink timing.
- Built a **4-bit, write-only 1602A LCD driver** using GPIO outputs, initialization commands, and fixed execution delays.
- Serialized shared LCD writes with brief global-interrupt masking to prevent nested routines from mixing bytes on the LCD bus.
- Drove a **four-pin RGB LED** with individually controlled red, green, and blue channels plus a shared D5/PA8 common connection. The `RGB_COMMON_ANODE` setting selects the electrical polarity.

## Hardware

- STM32F746G-DISCO (Arm Cortex-M7)
- 1602A LCD 
- 3 LEDs 
- 2 buttons/switches
- 3 resistors for each LED (470Ω)
- 10kΩ potentiometer for LCD contrast
- LCD backlight resistor as required for the specific module
- Breadboard and jumper wires

GPIO connections: **[Wiring Guide](docs/WIRING.md)**.

## Technologies
**Embedded C | STM32F746 | Arm Cortex-M7 | STM32CubeIDE | NVIC | EXTI | Timers | GPIO | LCD1602A**

## Repository Contents

```text
STM32F746-Nested-Interrupts-LCD/
├── README.md
├── Core/
│   └── Src/
│       └── main.c
└── docs/
    └── WIRING.md
```

`Core/Src/main.c` is intended for use inside an STM32CubeIDE **STM32F746G-DISCO HAL project**. Project-generated startup, HAL, and `main.h` files are not included.

## Demonstration
[YouTube Demonstration](https://youtube.com/shorts/SNVj-a5_by0?si=hl1ygIaFJ0p-HNcq)



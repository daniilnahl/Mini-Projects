# Wiring Guide for STM32F746G-DISCO + 1602A + 3 LEDs + 2 Switches

## STM32F746G-DISCO Arduino connector mapping
| Device | Arduino header | STM32 GPIO | Use |
|---|---|---|---|
| RGB Red | D3 | PB4 | Normal 5-blink phase |
| RGB Green | D4 | PG7 | Normal 8-blink phase |
| RGB Blue | D7 | PI3 | SW1 interrupt phase |
| RGB Common | D5 | **PA8** | Shared RGB anode/cathode terminal |
| SW1 | D8 | PI2 | EXTI2, active-low input, NVIC priority 5 |
| SW2 | D2 | PG6 | EXTI6/IRQ EXTI9_5, active-low input, NVIC priority 2 |

**Correction to the earlier wiring illustration:** D5 is **PA8**. PI0 is Arduino **D10**. See ST UM1907, *STM32F746G-DISCO user manual*, Table 3.

## LCD1602A (4-bit write-only mode)

| LCD pin | LCD signal | STM32F746G-DISCO connection |
|---|---|---|
| 1 | VSS | GND |
| 2 | VDD | 5 V (for a 5-V module) |
| 3 | V0 | 10-kΩ potentiometer wiper (outer terminals 5 V and GND) |
| 4 | RS | A0 = PA0 |
| 5 | R/W | GND (write only) |
| 6 | E | A1 = PF10 |
| 7–10 | DB0–DB3 | Not connected |
| 11 | DB4 | A2 = PF9 |
| 12 | DB5 | A3 = PF8 |
| 13 | DB6 | A4 = PF7 (check board routing) |
| 14 | DB7 | A5 = PF6 (check board routing) |
| 15 | LED+ | 5 V via appropriate series resistor if the module has none |
| 16 | LED− | GND |

The LCD supports both 4- and 8-bit transfers; this implementation selects 4-bit mode using the `Function Set` instruction (`0x28`) after the required startup handshake. Each byte is transmitted high nibble first.

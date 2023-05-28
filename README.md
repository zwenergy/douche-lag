# douche lag
A modular lag testing device.

## Idea
The idea of douche lag is to have a simple and cheap, yet modular lag testing device to test the complete "lag chain" of a gaming system.
By "complete lag chain" I mean the time between button input and visual change on the screen.
Douche lag is doing that by emulating a button press and waiting for a visual change on screen.

## BOM
### Main board
| **Reference** | **Part** | **Link** |
|---------|------|------|
| U6 | Raspberry Pi Pico | [Digikey](https://www.digikey.com/en/products/detail/raspberry-pi/SC0915/13624793)|
| R1, R2 | 10k Ohm resistor 0805 | [LCSC](https://www.lcsc.com/product-detail/Chip-Resistor-Surface-Mount_UNI-ROYAL-Uniroyal-Elec-0805W8F1002T5E_C17414.html)|
| SW1, SW2 | Tactile switch | [LCSC](https://www.lcsc.com/product-detail/Tactile-Switches_BZCN-TSB008A2518A_C2888455.html)|
| PS1, PS2 | Mini DIN 6 connector | [LCSC](https://www.lcsc.com/product-detail/Circular-Connectors-Cable-Connectors_CONNFLY-Elec-DS1093-01-PN60_C77848.html)|
| PCB1 | 128x64 OLED display | [AliExpress] (https://de.aliexpress.com/item/1005004242677199.html)

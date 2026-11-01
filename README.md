# Logitech Extreme 3D Pro on ESP32-WROOM

**Badger Deep Lab #001**: a Logitech Extreme 3D Pro joystick read by a plain ESP32-WROOM
with a pure software USB host. No USB host chip, no shield.

[Demo video](https://www.youtube.com/watch?v=1kNk3TOf-N8)

## The story

I'm not a software pro. I'm a tower crane operator and worked on domestic and
international projects. I don't want to do that job anymore, so I'm putting all my field
experience into robotic firefighting systems. I didn't want a toy controller for the robot,
so I chose the X3D Pro. I couldn't find a public demo of it on a plain ESP32-WROOM with a
software USB host, so I had to solve it first. After one month, I did. At 04:17 AM, servos moved.

## What it does

- 12 buttons -> 12 LEDs (each button lights its own LED while pressed)
- 4 axes: X, Y, twist, rear slider (printed over Serial when they change)
- Hat switch -> flash pattern on the on-board LED (GPIO 2)
- This is an example sketch: use it, change it, learn from it
- No debounce on purpose: LEDs mirror the raw button state

## Wiring

| Joystick USB | ESP32 |
|---|---|
| D+ | GPIO16 |
| D- | GPIO17 |
| 5V | 5V |
| GND | GND |

Button LEDs: GPIO 4, 13, 14, 18, 19, 21, 22, 23, 25, 26, 27, 32.
Hat flash LED: GPIO 2 (on-board).
Test setup: all LED cathodes share one 1 kΩ resistor to GND.

## Build

- Board: ESP32 Dev Module
- Core: esp32 by Espressif **2.0.17** (3.x does not work with the library)
- Library: [ESP32-USB-Soft-Host](https://github.com/tobozo/ESP32-USB-Soft-Host) by tobozo
- Serial monitor: 115200

## Thank you chain

**Code I built on**

- **Oleg Mazurov** ([felis](https://github.com/felis)): documented the Logitech Extreme 3D Pro
  report layout in 2012 and wrote the `le3dp` example for USB Host Shield 2.0.
  [Article](https://chome.nerpa.tech/mcu/using-logitech-extreme-3d-pro-joystick-with-arduino-hid-library/)
- **Dmitry Samsonov** ([sdima1357](https://github.com/sdima1357/esp32_usb_soft_host)):
  pure-software USB host on ESP32 GPIO pins
- **tobozo** ([ESP32-USB-Soft-Host](https://github.com/tobozo/ESP32-USB-Soft-Host)):
  Arduino wrapper of Dmitry's work

**Inspiration**

- **LEDFACIL** (Damián G. Lasso): [the video that opened my eyes](https://www.youtube.com/watch?v=kusJIDDmLes)
- **Steve Wozniak**
- **Owen**

Developed with AI assistance (Qwen); hardware testing and validation by me.

## License

AGPL-3.0, see [LICENSE](LICENSE). This project is not affiliated with or endorsed by Logitech.

## Author

Harun Er (HobbyHardwareMaker) / RAMS-FIRE Labs, Türkiye
Website: https://rams-fire.com | YouTube: https://www.youtube.com/@badger_deep_lab
If you use this, please credit Harun Er / RAMS-FIRE and link this repo.

#badgerdeeplab #esp32 #reverseengineering #logitech #hackaday

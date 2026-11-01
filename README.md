# Logitech Extreme 3D Pro on ESP32-WROOM
<img width="1190" height="896" alt="esp32 work" src="https://github.com/user-attachments/assets/ed840ff9-a953-4a31-9c7d-7084b3b98093" />


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
<img width="460" height="460" alt="oleg mazurov" src="https://github.com/user-attachments/assets/4a1e6b38-4778-436b-9d1d-5d005ef5f6b0" />
- **Oleg Mazurov** ([felis](https://github.com/felis)): documented the Logitech Extreme 3D Pro
  report layout in 2012 and wrote the `le3dp` example for USB Host Shield 2.0.
  
  [Article](https://chome.nerpa.tech/mcu/using-logitech-extreme-3d-pro-joystick-with-arduino-hid-library/)
  <img width="600" height="745" alt="dmitry samsonov1" src="https://github.com/user-attachments/assets/6eb6710f-cbf8-4fa7-a1d0-4c7f9dce3dd7" />
- **Dmitry Samsonov** ([sdima1357](https://github.com/sdima1357/esp32_usb_soft_host)):
  pure-software USB host on ESP32 GPIO pins
  
  <img width="1437" height="716" alt="tobozo kalp1" src="https://github.com/user-attachments/assets/9eed25dc-5df7-45ef-942f-048b092b0c06" />
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
This is not my personal victory. This is the result of the brilliant light they shed on the open-source world. I just followed their path and closed the chain.

Harun Er (HobbyHardwareMaker) / RAMS-FIRE Labs, Türkiye
Website: https://rams-fire.com | YouTube: https://www.youtube.com/@badger_deep_lab
If you use this, please credit Harun Er / RAMS-FIRE and link this repo.

#badgerdeeplab #esp32 #reverseengineering #logitech #hackaday

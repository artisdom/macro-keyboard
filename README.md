# Macro Keyboard

A Macro keyboard built on ESP32.


## Features

- Layout modifier: change the layout with a button or combination of buttons.
- Shift modifier: hold the shift key to access a new layout.
- Macros: fakes multiple key presses on a single key.

- Deep sleep
- Support for multiple BLE hosts and specific key layout to switch between them.


## Inspirations

- [QMK](https://github.com/qmk/qmk_firmware/) for AVR devices.
- [MK32](https://github.com/Galzai/MK32) somewhat of a port of QMK.


## Todo

- Support for multiple saved BLE hosts.
- Cleanup keyboard layouts in separate files.
- Put BLE stuff in it's own component/folder.
- KC_TRANSPARENT as MK32 does it, instead of having to duplicate the key in different layouts?
- Better NVS stuff (layouts, configs, ...etc).
- Battery management and reports.
- LEDs?
- USB HID (with ESP32-S3).
- Toggle switch between BLE and USB.
- MIDI device?
- Mutiple BLE hosts in parallel?


## Issues

- Potential issue when switching BLE peer: the hid report array is not propably reset.
Should not be an issue as BLE changes are supposed to happen without other keys being pressed.
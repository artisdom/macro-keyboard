# Macro Keyboard

A Macro keyboard built on ESP32-S3.    

Using esp-idf v5.0-beta1


## Features

- Layout modifier: change the layout with a button or combination of buttons.
- Shift modifier: hold the shift key to access a new layout.
- Macros: fakes multiple key presses on a single key.
- Deep sleep
- Battery level reports
- Support for multiple BLE hosts and specific key layout to switch between them.


## Todo

- Put BLE stuff in it's own component/folder.
- Create an HID level to create packets to be route to BLE or USB later on.
- KC_TRANSPARENT as MK32 does it, instead of having to duplicate the key in different layouts?
- Better NVS stuff (layouts, configs, ...etc).
- Better Battery reports (when init ble connection use read level + adc calibration)
- LEDs? To be finalised and better once final proto is done (use event handler).
- USB HID (with ESP32-S3).
- Toggle switch between BLE and USB (partially done).
- MIDI device?
- Mutiple BLE hosts in parallel? + BLE 5 features?
- If design is kept as BLE only, shift implementation to NimBLE stack (not Bluedroid).


## Issues

- Potential issue when switching BLE host: the hid report array is not propably reset.  
Should not be an issue as BLE host changes are supposed to happen without other keys being pressed.


## Docs / Referencs

- ESP-IDF [Guide](https://docs.espressif.com/projects/esp-idf/en/v4.4.2/esp32s3/index.html).
- ESP32-S3 [Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf).
- ESP32-S3 [Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf).
- [TinyS3](https://esp32s3.com/index.html#home) board.
- [Blog](http://blog.komar.be/how-to-make-a-keyboard-the-matrix/) on matrix keyboard design. 

### Inspirations

- [QMK](https://github.com/qmk/qmk_firmware/) for AVR devices.
- [MK32](https://github.com/Galzai/MK32) somewhat of a port of QMK for ESP32.

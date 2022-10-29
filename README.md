# Macro Keyboard

A Macro keyboard built on ESP32-S3.    


## Features

- Layouts: QMK style
- Macros: fakes multiple key presses on a single key.
- Deep sleep
- Battery level reports
- HID over USB or BLE.
- Support for multiple BLE hosts and specific key layout to switch between them.
- LED backlight with brightness control + effects for BLE actions


## Requirements

### Firmware

Using esp-idf (master branch)

### Hardware

Using Kicad v6.0


## Build

See the esp-idf [instructions](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html) on getting started.


To build:

```shell
idf.py build
```

To flash:

```shell
idf.py flash [-p port_name]
```

To get a console:

```shell
idf.py monitor [-p port_name]
```


### Keyboard Configuration

Each keyboard is defined in it's own subdirectory in the [keyboards](./firmware/keyboards) directory.
This keyboards directory is actually registered as a esp-idf component and will be built as a library and then linked with the [main](./firmware/main) component.

To choose which keyboard to compile for:

```shell
idf.py menuconfig
```
then `Keyboard Configuration` > `Keyboard to build`: Enter the name of the subdirectory for your keyboard.



## Todo

- Cleanup BLE files.
- Create an HID level to create packets to be routed to BLE or USB later on (waiting on esp to finish their esp_hid component).
- QMK features:
	- Make keymap fully compatible with QMK (maybe use the [QMK configurator](https://github.com/qmk/qmk_configurator) or the web api to compile keymaps).
	- Make keyboard compatible with VIA to change layouts and keymaps on the fly using NVS/EEPROM.
	- Support for more Layer modifiers (one-shot, tap, etc...).
- Better NVS stuff (layouts, configs, ...etc).
- Better Battery reports (when init ble connection use read level).
- Low Battery management: indicator + shutdown.
- Clean up HID descriptors to make them common between USB and BLE stacks.
- Better USB deinit (waiting on TinyUSB implementation).
- USB CDC and HID device in parallel to get log outputs in serial port.
- MIDI device?
- Mutiple BLE hosts in parallel? + BLE 5 features?
- If design is kept as BLE only, shift implementation to NimBLE stack (not Bluedroid).


## Issues

- Potential issue when switching BLE host: the hid report array is not propably reset.  
Should not be an issue as BLE host changes are supposed to happen without other keys being pressed.
- BLE local name needs to be less than 15 characters. Otherwise macOS will not show it in the discovery menu.


## Docs / Referencs

- ESP-IDF [Guide](https://docs.espressif.com/projects/esp-idf/en/v4.4.2/esp32s3/index.html).
- ESP32-S3 [Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-s3_datasheet_en.pdf).
- ESP32-S3 [Technical Reference Manual](https://www.espressif.com/sites/default/files/documentation/esp32-s3_technical_reference_manual_en.pdf).
- [TinyS3](https://esp32s3.com/index.html#home) board.
- [Blog](http://blog.komar.be/how-to-make-a-keyboard-the-matrix/) on matrix keyboard design. 
- QMK [keymaps](https://docs.qmk.fm/#/keymap?id=keymap-and-layers) and [layers](https://docs.qmk.fm/#/feature_layers) documentation.

### Inspirations

- [QMK](https://github.com/qmk/qmk_firmware/) for AVR devices.
- [MK32](https://github.com/Galzai/MK32) a QMK style firmware for ESP32 devices.

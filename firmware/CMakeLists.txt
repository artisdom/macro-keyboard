# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set( EXTRA_COMPONENT_DIRS   "./keyboards")
set( COMPONENTS   main keyboards esp_system esp_timer bt esp_adc driver nvs_flash tinyusb esp_psram )

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(Macro-Keyboard)

# Fix for TinyUSB HID buffer which needs to be the same size as VIA HID report size for the RX callback to work
idf_build_set_property(COMPILE_OPTIONS "-DCFG_TUD_HID_EP_BUFSIZE=32" APPEND)

#Set source files
# Add all your .c or .cpp files in folder here
set( srcs   "Macro-Keyboard.c"
            "matrix.c"
            "keyboard.c"
            "layers.c"
            "dynamic_keymap.c"
            "keymap_introspection.c"
            "memory.c"
            "leds.c"
            "toggle_switch.c"
            "battery.c"
            "ble.c"
            "esp_hidd_prf_api.c"
            "hid_dev.c"
            "hid_device_le_prf.c"
            "usb.c"
            "via.c")

#Set include directories
# Add all your include file path here
set(include_dirs ".")

idf_component_register( SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    REQUIRES esp_system esp_timer bt esp_adc driver nvs_flash keyboards)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-const-variable -fdiagnostics-color=always)

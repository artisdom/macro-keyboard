#Set source files
# Add all your .c or .cpp files in folder here
set( srcs   "Macro-Keyboard.c"
            "matrix.c"
            "keyboard.c"
            "keymap.c"
            "memory.c"
            "leds.c"
            "toggle_switch.c"
            "ble_hidd.c"
            "esp_hidd_prf_api.c"
            "hid_dev.c"
            "hid_device_le_prf.c")

#Set include directories
# Add all your include file path here
set (include_dirs ".")

idf_component_register( SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}")

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-const-variable)

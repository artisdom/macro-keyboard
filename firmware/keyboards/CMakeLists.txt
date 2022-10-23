# Set source files
set( srcs   "${CONFIG_KEYBOARD_DIR}/keymap.c")

# Set include directories
set(include_dirs "./${CONFIG_KEYBOARD_DIR}")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_REQUIRES "main")

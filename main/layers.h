#ifndef LAYERS_h
#define LAYERS_H


void 	 layers__init();
void 	 layers__set_default_layer(uint8_t layer);
void 	 layers__activate_layer(uint8_t layer);
void 	 layers__deactivate_layer(uint8_t layer);
void 	 layers__deactivate_all();
void 	 layers__toggle_layer(uint8_t layer);

uint16_t layers__get_keycode(uint8_t row, uint8_t col);


#endif
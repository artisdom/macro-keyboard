#ifndef BATTERY_H
#define BATTERY_H




void battery__init();
uint32_t battery__get_level();
bool battery__is_charging();


#endif
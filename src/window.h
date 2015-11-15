#pragma once

#include <pebble.h>

#include "config.h"

void main_window_init();

void main_window_minute_update(int hours, int minutes);

void main_window_date_update(int date);

void main_window_battery_update(int battery);

void main_window_bt_update(bool bt);

#pragma once

#include <pebble.h>

void main_window_set_config(GColor8 mc, GColor8 mnbtc, GColor8 hc, GColor8 hnbtc, GColor8 tc, GColor8 tlbc, GColor8 bc, int br, int bo, bool rm, int lbt);

void main_window_redraw();

void main_window_init();

void main_window_deinit();

void main_window_minute_update(int hours, int minutes);

void main_window_date_update(int date);

void main_window_battery_update(int battery);

void main_window_bt_update(bool bt);

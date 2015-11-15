#include <pebble.h>

#include "window.h"

static void tick_handler(struct tm *time_now, TimeUnits changed) {
  static bool first_run = true;
  
  // Convert to 24h
  char the_24h_buffer[] = "00";
  strftime(the_24h_buffer, sizeof(the_24h_buffer), "%H", time_now);
  uint8_t hour_buffer = (the_24h_buffer[0] - '0') * 10 + (the_24h_buffer[1] - '0');
  
  main_window_minute_update(hour_buffer, time_now->tm_min);
  if(first_run || (changed & DAY_UNIT)){
    main_window_date_update(time_now->tm_mday);
  }
  first_run = false;
}

static void battery_callback(BatteryChargeState state) {
  main_window_battery_update(state.charge_percent);
}

static void bluetooth_callback(bool connected) {
  main_window_bt_update(connected);
}

static void init() {
  main_window_init();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_callback);
  battery_callback(battery_state_service_peek());
  bluetooth_connection_service_subscribe(bluetooth_callback);
  bluetooth_callback(bluetooth_connection_service_peek());
}

static void deinit() { 
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
}

int main() {
  init();
  app_event_loop();
  deinit();
}

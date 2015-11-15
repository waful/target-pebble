#include <pebble.h>

#include "window.h"

static void tick_handler(struct tm *time_now, TimeUnits changed) {
  static bool first_run = true;
  main_window_minute_update(time_now->tm_hour, time_now->tm_min);
  if(first_run || (changed & DAY_UNIT)){
    main_window_date_update(time_now->tm_mday);
  }
  first_run = false;
}

static void battery_callback(BatteryChargeState state) {
  int battery = state.charge_percent == 100 ? 99 : state.charge_percent;
  main_window_battery_update(battery);
}

static void init() {
  main_window_init();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_callback);
  battery_callback(battery_state_service_peek());
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

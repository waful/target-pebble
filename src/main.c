#include <pebble.h>

#include "autoconfig.h"
#include "config.h"
#include "window.h"

static bool BT_VIBE;

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

void focus_handler(bool in_focus) {
  if (in_focus) {
    main_window_redraw();
  }
}

static void battery_callback(BatteryChargeState state) {
  main_window_battery_update(state.charge_percent);
}

static void bluetooth_callback(bool connected) {
  main_window_bt_update(connected);
  static bool first_call = true;
  if(BT_VIBE && !first_call){
      if(connected){
        vibes_long_pulse();
      }
      else{
        vibes_double_pulse();
      }
  }
  first_call = false;
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  autoconfig_in_received_handler(iter, context);

  main_window_set_config(getMinutes_color(), getMinutes_no_bt_color(), getHours_color(), getHours_no_bt_color(), getText_color(), getText_low_battery_color(), getBg_color(), getBar_radius(), getBar_offset(), getRing_markings());
  BT_VIBE = getBt_vibe();
  main_window_redraw();
}

static void init() {
  autoconfig_init();
  app_message_register_inbox_received(in_received_handler);
  main_window_set_config(getMinutes_color(), getMinutes_no_bt_color(), getHours_color(), getHours_no_bt_color(), getText_color(), getText_low_battery_color(), getBg_color(), getBar_radius(), getBar_offset(), getRing_markings());
  BT_VIBE = getBt_vibe();
  main_window_init();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_callback);
  battery_callback(battery_state_service_peek());
  bluetooth_connection_service_subscribe(bluetooth_callback);
  bluetooth_callback(bluetooth_connection_service_peek());
  app_focus_service_subscribe(focus_handler);
}

static void deinit() { 
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  autoconfig_deinit();
}

int main() {
  init();
  app_event_loop();
  deinit();
}

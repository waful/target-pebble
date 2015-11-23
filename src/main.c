#include <pebble.h>

#include "autoconfig.h"
#include "config.h"
#include "window.h"

static bool BT_VIBE;

static void tick_handler(struct tm *time_now, TimeUnits changed) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "start of tick handler");
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
  APP_LOG(APP_LOG_LEVEL_DEBUG, "end of tick handler");
}

static void battery_callback(BatteryChargeState state) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "start of battery handler");
  main_window_battery_update(state.charge_percent);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "end of battery handler");
}

static void bluetooth_callback(bool connected) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "start of bt handler");
  main_window_bt_update(connected);
  static bool first_call = true;
  if(BT_VIBE && !first_call){
      if(connected){
        vibes_short_pulse();
      }
      else{
        vibes_double_pulse();
      }
  }
  first_call = false;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "end of bt handler");
}

static void in_received_handler(DictionaryIterator *iter, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "start of msg handler");
  autoconfig_in_received_handler(iter, context);

  main_window_set_config(getMinutes_color(), getMinutes_no_bt_color(), getHours_color(), getHours_no_bt_color(), getText_color(), getText_low_battery_color(), getBg_color(), getBar_radius(), getBar_offset(), getRing_markings(), getLow_bat_threshold());
  BT_VIBE = getBt_vibe();
  main_window_redraw();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "end of msg handler");
}

static void init() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "start of init");
  autoconfig_init(512, 512);
  app_message_register_inbox_received(in_received_handler);
  main_window_set_config(getMinutes_color(), getMinutes_no_bt_color(), getHours_color(), getHours_no_bt_color(), getText_color(), getText_low_battery_color(), getBg_color(), getBar_radius(), getBar_offset(), getRing_markings(), getLow_bat_threshold());
  BT_VIBE = getBt_vibe();
  main_window_init();
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  battery_state_service_subscribe(battery_callback);
  battery_callback(battery_state_service_peek());
  bluetooth_connection_service_subscribe(bluetooth_callback);
  bluetooth_callback(bluetooth_connection_service_peek());
  APP_LOG(APP_LOG_LEVEL_DEBUG, "end of init");
}

static void deinit() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "start of deinit");
  main_window_deinit();
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  autoconfig_deinit();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "end of deinit");
}

int main() {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "start of main");
  init();
  app_event_loop();
  deinit();
  APP_LOG(APP_LOG_LEVEL_DEBUG, "end of main");
}

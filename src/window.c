#include "window.h"

static Window *s_window;
static Layer *s_rings_canvas;
static TextLayer *s_main_text_layer;

static int s_hours = 0, s_minutes = 0, s_date = 0;

static bool s_bt = true, low_bat = false;

static int32_t get_angle_for_hour(int hour) {
  return (hour * 360) / 12;
}

static int32_t get_angle_for_minute(int minute) {
  return (minute * 360) / 60;
}

static void draw_hour_and_minute(Layer *layer, GContext *ctx){
  GRect bounds = layer_get_bounds(layer);
  GColor8 minutes_color, hours_color;
  
  if(s_bt){
    minutes_color = MINUTES_COLOR;
    hours_color = HOURS_COLOR;
  }
  else{
    minutes_color = MINUTES_NO_BT_COLOR;
    hours_color = HOURS_NO_BT_COLOR;
  }
  
  bool minutes_reversed = false;
  bool hours_reversed = false;
  
  if(s_hours >= 12){
    hours_reversed = true;
  }
  if(s_hours % 2){
    minutes_reversed = true;
  }
  
  // Convert hour from 24 to 12h
  s_hours -= (s_hours > 12) ? 12 : 0;

  // Minutes are expanding circle arc
  int minute_angle = get_angle_for_minute(s_minutes);
  GRect frame = grect_inset(bounds, GEdgeInsets(0));
  graphics_context_set_fill_color(ctx, minutes_color);
  if(!minutes_reversed){
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, BAR_RADIUS * 1.5, 0, DEG_TO_TRIGANGLE(minute_angle));
  }
  else{
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, BAR_RADIUS * 1.5, DEG_TO_TRIGANGLE(minute_angle), DEG_TO_TRIGANGLE(360));
  }

  // Adjust geometry variables for inner ring
  frame = grect_inset(frame, GEdgeInsets(BAR_RADIUS * 1.3));

  // Hours are expanding circle arc
  int hour_angle = get_angle_for_hour(s_hours);
  graphics_context_set_fill_color(ctx, hours_color);
  if(!hours_reversed){
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, BAR_RADIUS, 0, DEG_TO_TRIGANGLE(hour_angle));
  }
  else{
    graphics_fill_radial(ctx, frame, GOvalScaleModeFitCircle, BAR_RADIUS, DEG_TO_TRIGANGLE(hour_angle), DEG_TO_TRIGANGLE(360));
  }
}

static void draw_text(){
  static char buf[] = "00.00";
  snprintf(buf, sizeof(buf), "%02d", s_date);
  text_layer_set_text(s_main_text_layer, buf);
}

static void rings_layer_update_proc(Layer *layer, GContext *ctx) {
  draw_hour_and_minute(layer, ctx);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_rings_canvas = layer_create(bounds);
  layer_set_update_proc(s_rings_canvas, rings_layer_update_proc);
  layer_add_child(window_layer, s_rings_canvas);
  
  s_main_text_layer = text_layer_create(GRect(BAR_RADIUS * 2.3, (bounds.size.h / 2.0) - 27, bounds.size.w - BAR_RADIUS * 4.6, 42));
  text_layer_set_text_alignment(s_main_text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(s_main_text_layer, GColorClear);
  text_layer_set_text_color(s_main_text_layer, TEXT_COLOR);
  text_layer_set_font(s_main_text_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  layer_add_child(window_layer, text_layer_get_layer(s_main_text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(s_main_text_layer);
  layer_destroy(s_rings_canvas);
  window_destroy(s_window);
}

void main_window_init() {
  s_window = window_create();
  window_set_background_color(s_window, BG_COLOR);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
}

void main_window_minute_update(int hours, int minutes) {
  s_hours = hours;
  s_minutes = minutes;
  layer_mark_dirty(s_rings_canvas);
}

void main_window_date_update(int date) {
  s_date = date;
  draw_text();
}

void main_window_battery_update(int battery) {
  bool tmp_low_bat = false;
  if(battery <= 20){
    tmp_low_bat = true;
  }
  if(tmp_low_bat != low_bat){
    low_bat = tmp_low_bat;
    if(low_bat){
      text_layer_set_text_color(s_main_text_layer, TEXT_LOW_BATTERY_COLOR);
    }
    else{
      text_layer_set_text_color(s_main_text_layer, TEXT_COLOR);
    }
  }
}

void main_window_bt_update(bool bt) {
  if(bt != s_bt){
    s_bt = bt;
    layer_mark_dirty(s_rings_canvas);
  }
}
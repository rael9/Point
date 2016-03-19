#include "pebble.h"

#define NUM_CLOCK_TICKS 11

static Window *s_window;
static Layer *s_simple_bg_layer, *s_date_layer, *s_hours_layer, *s_minutes_layer, *s_seconds_layer;
static TextLayer *s_day_label, *s_num_label;
static BitmapLayer *s_background_image_layer, *s_hours_image_layer, *s_minutes_image_layer, *s_seconds_image_layer;
static GBitmap *s_background_image, *s_hours_image, *s_minutes_image, *s_seconds_image;
static GPath *s_tick_paths[NUM_CLOCK_TICKS];
static char s_num_buffer[4], s_day_buffer[6];

static void bg_update_proc(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, layer_get_bounds(layer), 0, GCornerNone);
  graphics_context_set_fill_color(ctx, GColorBlack);
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_move_to(s_tick_paths[i], GPoint(0, 0));
    gpath_draw_filled(ctx, s_tick_paths[i]);
  }
}

static void hours_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
    
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  graphics_draw_rotated_bitmap(ctx, s_hours_image, center, (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6), center);
}

static void minutes_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
    
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  graphics_draw_rotated_bitmap(ctx, s_minutes_image, center, TRIG_MAX_ANGLE * t->tm_min / 60, center);
}

static void seconds_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
    
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  graphics_draw_rotated_bitmap(ctx, s_seconds_image, center, TRIG_MAX_ANGLE * t->tm_sec / 60, center);
}

static void date_update_proc(Layer *layer, GContext *ctx) {
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  strftime(s_day_buffer, sizeof(s_day_buffer), "%a", t);
  text_layer_set_text(s_day_label, s_day_buffer);

  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
  text_layer_set_text(s_num_label, s_num_buffer);
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(s_window));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_simple_bg_layer = layer_create(bounds);
  layer_set_update_proc(s_simple_bg_layer, bg_update_proc);
  layer_add_child(window_layer, s_simple_bg_layer);

  // Background image
  s_background_image_layer = bitmap_layer_create(bounds);
  s_background_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_BACKGROUND);
  bitmap_layer_set_compositing_mode(s_background_image_layer, GCompOpAssign);
  bitmap_layer_set_bitmap(s_background_image_layer, s_background_image);
  bitmap_layer_set_alignment(s_background_image_layer, GAlignCenter);
  layer_add_child(s_simple_bg_layer, bitmap_layer_get_layer(s_background_image_layer));
    
  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);

  s_day_label = text_layer_create(GRect(5, 77, 27, 20));
  text_layer_set_text(s_day_label, s_day_buffer);
  text_layer_set_background_color(s_day_label, GColorWhite);
  text_layer_set_text_color(s_day_label, GColorBlack);
  text_layer_set_font(s_day_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  layer_add_child(s_date_layer, text_layer_get_layer(s_day_label));

  s_num_label = text_layer_create(GRect(160, 77, 18, 20));
  text_layer_set_text(s_num_label, s_num_buffer);
  text_layer_set_background_color(s_num_label, GColorWhite);
  text_layer_set_text_color(s_num_label, GColorBlack);
  text_layer_set_font(s_num_label, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));

  layer_add_child(s_date_layer, text_layer_get_layer(s_num_label));

  s_hours_layer = layer_create(bounds);
  layer_set_update_proc(s_hours_layer, hours_update_proc);
  layer_add_child(window_layer, s_hours_layer);
  s_hours_image_layer = bitmap_layer_create(bounds);
  s_hours_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_HOURS);
  bitmap_layer_set_compositing_mode(s_hours_image_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_hours_image_layer, s_hours_image);
  bitmap_layer_set_alignment(s_hours_image_layer, GAlignCenter);
  layer_add_child(s_hours_layer, bitmap_layer_get_layer(s_hours_image_layer));
  
  s_minutes_layer = layer_create(bounds);
  layer_set_update_proc(s_minutes_layer, minutes_update_proc);
  layer_add_child(window_layer, s_minutes_layer);
  s_minutes_image_layer = bitmap_layer_create(bounds);
  s_minutes_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MINUTES);
  bitmap_layer_set_compositing_mode(s_minutes_image_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_minutes_image_layer, s_minutes_image);
  bitmap_layer_set_alignment(s_minutes_image_layer, GAlignCenter);
  layer_add_child(s_minutes_layer, bitmap_layer_get_layer(s_minutes_image_layer));
  
  s_seconds_layer = layer_create(bounds);
  layer_set_update_proc(s_seconds_layer, seconds_update_proc);
  layer_add_child(window_layer, s_seconds_layer);
  s_seconds_image_layer = bitmap_layer_create(bounds);
  s_seconds_image = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_SECONDS);
  bitmap_layer_set_compositing_mode(s_seconds_image_layer, GCompOpSet);
  bitmap_layer_set_bitmap(s_seconds_image_layer, s_seconds_image);
  bitmap_layer_set_alignment(s_seconds_image_layer, GAlignCenter);
  layer_add_child(s_seconds_layer, bitmap_layer_get_layer(s_seconds_image_layer));
}

static void window_unload(Window *window) {
  gbitmap_destroy(s_background_image);
  bitmap_layer_destroy(s_background_image_layer);
    
  layer_destroy(s_simple_bg_layer);
  layer_destroy(s_date_layer);

  text_layer_destroy(s_day_label);
  text_layer_destroy(s_num_label);

  layer_destroy(s_hours_layer);
  layer_destroy(s_minutes_layer);
  layer_destroy(s_seconds_layer);
}

static void init() {
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);

  s_day_buffer[0] = '\0';
  s_num_buffer[0] = '\0';

  tick_timer_service_subscribe(SECOND_UNIT, handle_second_tick);
}

static void deinit() {
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}

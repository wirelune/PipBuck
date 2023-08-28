#include <pebble.h>

//!!!
//CONTENT WARNING - TONS OF SHITCODE!!! YOU'VE BEEN WARNED!!!
//!!!


// UI
static int s_battery_level;
static char battery_text[] = "100";
static GBitmap *s_connect_icon, *s_dsconnect_icon;
static BitmapLayer *s_connect_layer, *s_dsconnect_layer;
static Window *s_main_window;
static TextLayer *s_time_layer, *s_date_layer, *s_percentage_layer;
// Pippie my beloved
static Layer *s_battery_layer1, *s_battery_layer2, *s_battery_layer3,
*s_battery_layer4, *s_battery_layer5, *s_battery_layer6;
static BitmapLayer *s_background_layer, *s_littlepip_layer,
*s_crippled1_layer, *s_crippled2_layer;
static GBitmap *s_background_bitmap,
*s_lpip_bitmap, *s_lpip50_bitmap, *s_lpiplow_bitmap,
*s_crippled_bitmap, *s_crippled2_bitmap;


static void battery_callback(BatteryChargeState state) {
  // Record the new battery level
  s_battery_level = state.charge_percent;

}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);

  // Write the current hours and minutes into a buffer
  static char s_buffer[8];
  static char s_buffer2[] = "00.00.00";
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ?
                                          "%H : %M" : "%I : %M", tick_time);
  strftime(s_buffer2, sizeof(s_buffer2), "%d.%m.%y", tick_time);
  text_layer_set_text(s_date_layer, s_buffer2);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);

  // Display battery percentage
  snprintf(battery_text, sizeof(battery_text), "%d", s_battery_level);
  text_layer_set_text(s_percentage_layer, battery_text);

  // Mark battery layer dirty just for lulz
  layer_mark_dirty(s_battery_layer1);
  layer_mark_dirty(s_battery_layer2);
  layer_mark_dirty(s_battery_layer3);
  layer_mark_dirty(s_battery_layer4);
  layer_mark_dirty(s_battery_layer5);
  layer_mark_dirty(s_battery_layer6);

  if(s_battery_level > 50)
  {
    layer_set_hidden(bitmap_layer_get_layer(s_crippled1_layer), true);
    layer_set_hidden(bitmap_layer_get_layer(s_crippled2_layer), true);
    bitmap_layer_set_bitmap(s_littlepip_layer, s_lpip_bitmap);
  }
  else if(s_battery_level > 20)
  {
    layer_set_hidden(bitmap_layer_get_layer(s_crippled1_layer), true);
    layer_set_hidden(bitmap_layer_get_layer(s_crippled2_layer), true);
    bitmap_layer_set_bitmap(s_littlepip_layer, s_lpip50_bitmap);
  }
  else if(s_battery_level <= 20)
  {
    layer_set_hidden(bitmap_layer_get_layer(s_crippled1_layer), false);
    layer_set_hidden(bitmap_layer_get_layer(s_crippled2_layer), false);
    bitmap_layer_set_bitmap(s_littlepip_layer, s_lpiplow_bitmap);
  }

}

static void bluetooth_callback(bool connected)
{
  layer_set_hidden(bitmap_layer_get_layer(s_dsconnect_layer), connected);

  if(!connected)
  {
    vibes_double_pulse();
  }
}

static void battery_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);

  // Find the width of the bar (total width = 20px)
  int width = (s_battery_level * 11) / 100;

  // Draw the background
  //graphics_context_set_fill_color(ctx, GColorRed);
  //graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Draw the bar
  graphics_context_set_fill_color(ctx, GColorGreen);
  graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);
}

static void main_window_load(Window *window) {
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create the TextLayer with specific bounds
  s_time_layer = text_layer_create(
      GRect(0, 2, 95, 30));

  s_date_layer = text_layer_create(
      GRect(13, 132, 80, 26));

  s_percentage_layer = text_layer_create(
      GRect(86, 2, 60, 30));

  // Create GBitmap
s_background_bitmap = gbitmap_create_with_resource(RESOURCE_ID_BACKGROUND);
s_lpip_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LPIP);
s_lpip50_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LPIP50);
s_lpiplow_bitmap = gbitmap_create_with_resource(RESOURCE_ID_LPIPLOW);

s_connect_icon = gbitmap_create_with_resource(RESOURCE_ID_CONNECT);
s_dsconnect_icon = gbitmap_create_with_resource(RESOURCE_ID_DISCONNECT);

s_crippled_bitmap = gbitmap_create_with_resource(RESOURCE_ID_CRIPPLED);

// Create BitmapLayer to display the GBitmap
s_background_layer = bitmap_layer_create(bounds);
s_littlepip_layer = bitmap_layer_create(GRect(36,34,78,96));
// Making pip transparent
bitmap_layer_set_compositing_mode(s_littlepip_layer, GCompOpSet);
// Crippled layers
s_crippled1_layer = bitmap_layer_create(GRect(5,119,31,5));
s_crippled2_layer = bitmap_layer_create(GRect(66,132,31,5));

// Bitmap layers for connection icons
s_connect_layer = bitmap_layer_create(GRect(107, 141, 16, 16));
s_dsconnect_layer = bitmap_layer_create(GRect(107, 141, 16, 16));

// Show the correct state of bt connection
bluetooth_callback(connection_service_peek_pebble_app_connection());


// Set the bitmap onto the layer and add to the window
bitmap_layer_set_bitmap(s_background_layer, s_background_bitmap);
layer_add_child(window_layer, bitmap_layer_get_layer(s_background_layer));
bitmap_layer_set_bitmap(s_littlepip_layer, s_lpip_bitmap);
layer_add_child(window_layer, bitmap_layer_get_layer(s_littlepip_layer));

// Create and add layer for connection icon
bitmap_layer_set_bitmap(s_connect_layer, s_connect_icon);
layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_connect_layer));
// And for disconnect
bitmap_layer_set_bitmap(s_dsconnect_layer, s_dsconnect_icon);
layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(s_dsconnect_layer));

// Create and add the battery meter Layer
s_battery_layer1 = layer_create(GRect(19, 121, 115, 2));
s_battery_layer2 = layer_create(GRect(50, 85, 115, 2));
s_battery_layer3 = layer_create(GRect(19, 57, 115, 2));
s_battery_layer4 = layer_create(GRect(49, 134, 115, 2));
s_battery_layer5 = layer_create(GRect(68, 134, 115, 2));
s_battery_layer6 = layer_create(GRect(101, 121, 115, 2));
layer_set_update_proc(s_battery_layer1, battery_update_proc);
layer_set_update_proc(s_battery_layer2, battery_update_proc);
layer_set_update_proc(s_battery_layer3, battery_update_proc);
layer_set_update_proc(s_battery_layer4, battery_update_proc);
layer_set_update_proc(s_battery_layer5, battery_update_proc);
layer_set_update_proc(s_battery_layer6, battery_update_proc);
layer_add_child(window_get_root_layer(window), s_battery_layer1);
layer_add_child(window_get_root_layer(window), s_battery_layer2);
layer_add_child(window_get_root_layer(window), s_battery_layer3);
layer_add_child(window_get_root_layer(window), s_battery_layer4);
layer_add_child(window_get_root_layer(window), s_battery_layer5);
layer_add_child(window_get_root_layer(window), s_battery_layer6);

// Crippled stuff
bitmap_layer_set_bitmap(s_crippled1_layer, s_crippled_bitmap);
layer_add_child(window_layer, bitmap_layer_get_layer(s_crippled1_layer));
bitmap_layer_set_bitmap(s_crippled2_layer, s_crippled_bitmap);
layer_add_child(window_layer, bitmap_layer_get_layer(s_crippled2_layer));

  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_background_color(s_percentage_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorGreen);
  text_layer_set_text_color(s_date_layer, GColorGreen);
  text_layer_set_text_color(s_percentage_layer, GColorGreen);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_font(s_percentage_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  text_layer_set_text_alignment(s_percentage_layer, GTextAlignmentCenter);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_date_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_percentage_layer));

}

static void main_window_unload(Window *window) {
  // Destroy TextLayer
  text_layer_destroy(s_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_percentage_layer);
  // Destroy GBitmap
  gbitmap_destroy(s_lpip_bitmap);
  gbitmap_destroy(s_background_bitmap);
  gbitmap_destroy(s_connect_icon);
  gbitmap_destroy(s_dsconnect_icon);
  // Destroy BitmapLayer
  bitmap_layer_destroy(s_littlepip_layer);
  bitmap_layer_destroy(s_background_layer);
  bitmap_layer_destroy(s_connect_layer);
  bitmap_layer_destroy(s_dsconnect_layer);
  // Destroy battery line layers
  layer_destroy(s_battery_layer1);
  layer_destroy(s_battery_layer2);
  layer_destroy(s_battery_layer3);
  layer_destroy(s_battery_layer4);
  layer_destroy(s_battery_layer5);
  layer_destroy(s_battery_layer6);
  // Destroy crippled
  bitmap_layer_destroy(s_crippled1_layer);
  bitmap_layer_destroy(s_crippled2_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}


static void init() {
  // Create main Window element and assign to pointer
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorBlack);

  // Register for battery level updates
  battery_state_service_subscribe(battery_callback);
  battery_callback(battery_state_service_peek());

  // Register for bluetooth updates
  connection_service_subscribe((ConnectionHandlers) {
    .pebble_app_connection_handler = bluetooth_callback
  });

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  // Register with TickTimerService
tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

// Make sure the time is displayed from the start
update_time();
}

static void deinit() {
  // Destroy Window
  window_destroy(s_main_window);
}


int main(void) {
  init();
  app_event_loop();
  deinit();
}

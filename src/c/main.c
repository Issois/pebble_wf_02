#include <pebble.h>

static Window *window_main;
static TextLayer *layer_battery;
static TextLayer *layer_day;
static TextLayer *layer_time;
static TextLayer *layer_date;
static TextLayer *layer_bluetooth;
static GFont font_time;
static GFont font_date;
static int battery_level;

static void battery_callback(BatteryChargeState state) {
	battery_level = state.charge_percent;
	static char battery_buffer[4];
	snprintf(battery_buffer, sizeof(battery_buffer), "%d", battery_level);
	text_layer_set_text(layer_battery, battery_buffer);
}

static void bluetooth_callback(bool connected) {
	if(connected){
		text_layer_set_background_color(layer_bluetooth, GColorBlack);
	}else{
		text_layer_set_background_color(layer_bluetooth, GColorClear);
	}

}

static void update_time() {
	time_t temp = time(NULL);
	struct tm *tick_time = localtime(&temp);

	static char time_buffer[8];
	strftime(time_buffer,sizeof(time_buffer),"%H:%M",tick_time);
	text_layer_set_text(layer_time, time_buffer);


	static char date_buffer[40];
	strftime(date_buffer, sizeof(date_buffer), "%d.%m.%y - %V.%u", tick_time);
	text_layer_set_text(layer_date, date_buffer);

	static char day_buffer[10];
	strftime(day_buffer, sizeof(day_buffer), "%A", tick_time);
	text_layer_set_text(layer_day, day_buffer);

}

static void tick_handler(struct tm *tick_time, TimeUnits unitchanged) {
	update_time();
}
// static void bluetooth_layer_draw(Layer *layer, GContext *ctx) {
// 	GRect bounds = layer_get_bounds(layer);
// 	graphics_context_set_fill_color(ctx, GColorBlack);
// 	graphics_fill_rect(ctx, bounds, 1, GCornerNone);
// }

static void main_window_load(Window *window) {
	font_time = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BRIT_70));
	font_date = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_BRIT_22));
	// Get information about the Window
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	// int x=30;
	layer_bluetooth = text_layer_create(GRect(5,5,20,20));
	text_layer_set_background_color(layer_bluetooth, GColorBlack);

	layer_day = text_layer_create(GRect(0,50,bounds.size.w,bounds.size.h));
	text_layer_set_text_color(layer_day, GColorBlack);
	text_layer_set_background_color(layer_day, GColorClear);
	text_layer_set_font(layer_day, font_date);
	text_layer_set_text_alignment(layer_day, GTextAlignmentCenter);

	layer_time = text_layer_create(GRect(0,68,bounds.size.w,bounds.size.h));
	text_layer_set_text_color(layer_time, GColorBlack);
	text_layer_set_background_color(layer_time, GColorClear);
	text_layer_set_font(layer_time, font_time);
	text_layer_set_text_alignment(layer_time, GTextAlignmentCenter);

	layer_date = text_layer_create(GRect(0,145,bounds.size.w,bounds.size.h));
	text_layer_set_text_color(layer_date, GColorBlack);
	text_layer_set_background_color(layer_date, GColorClear);
	text_layer_set_font(layer_date, font_date);
	text_layer_set_text_alignment(layer_date, GTextAlignmentCenter);

	layer_battery = text_layer_create(GRect(70,0,bounds.size.w,bounds.size.h));
	text_layer_set_text_color(layer_battery, GColorBlack);
	text_layer_set_background_color(layer_battery, GColorClear);
	text_layer_set_font(layer_battery, font_date);
	text_layer_set_text_alignment(layer_battery, GTextAlignmentCenter);


	// Add layers to the Window
	layer_add_child(window_layer, text_layer_get_layer(layer_time));
	layer_add_child(window_layer, text_layer_get_layer(layer_date));
	layer_add_child(window_layer, text_layer_get_layer(layer_day));
	layer_add_child(window_layer, text_layer_get_layer(layer_battery));
	layer_add_child(window_layer, text_layer_get_layer(layer_bluetooth));
}

static void main_window_unload(Window *window) {
	// Destroy TextLayers
	text_layer_destroy(layer_time);
	text_layer_destroy(layer_date);
	text_layer_destroy(layer_day);
	text_layer_destroy(layer_battery);
	text_layer_destroy(layer_bluetooth);
}

static void init() {
	// Create main Window element and assign to pointer
	window_main = window_create();

	// Set the background color
	window_set_background_color(window_main, GColorWhite);

	// Set handlers to manage the elements inside the Window
	window_set_window_handlers(window_main, (WindowHandlers) {
		.load = main_window_load,
		.unload = main_window_unload
	});

	connection_service_subscribe((ConnectionHandlers) {
		.pebble_app_connection_handler = bluetooth_callback
	});
	// Show the Window on the watch, with animated=true
	window_stack_push(window_main, true);

	battery_state_service_subscribe(battery_callback);
	// Make sure the time is displayed from the start
	battery_callback(battery_state_service_peek());
	update_time();

	// Register with TickTimerService
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}


static void deinit() {
	// Destroy Window
	window_destroy(window_main);
}




int main(void) {
	init();
	app_event_loop();
	deinit();
}

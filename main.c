#include <pebble.h>
#include "cards.h"
	
#include "content.h"

#define NUMBER_OF_CARDS  (NUMBER_OF_FLASH_CARDS)

//#define NEXT_INTERVAL  (30)    // seconds
//#define THRESHOLD      (800)   // mg

static Window *window;
//static GBitmap *image_front;
//static BitmapLayer *image_layer_front;
static TextLayer *text_layer;
//static uint8_t seconds_tick = 0;
static CardBack_t card_back;
static CardBack_t card_front;




// Globals
static enum side_t current_side;
static enum card_t current_card;
static Window *window;
//static GBitmap *image_front;
//static BitmapLayer *image_layer_front;
static CardBack_t card_back;
static CardBack_t card_front;
static bool card_used[NUMBER_OF_CARDS];
static uint8_t card_counter = NUMBER_OF_CARDS;

// Private functions
static void load_card(void);
static void init_card_text(void);
static void show_card_text(void);
static void show_card_text_front(void);
static void hide_card_text(void);
static void hide_card_text_front(void);
static void load_card_text(enum card_t aa);
static void reset_card_used(void);

void init_cards(Window *main_window,
                CardBack_t main_card_front,
                CardBack_t main_card_back)
{
  reset_card_used();
  current_card = FIRST_CARD;
  current_side = FRONT;
  window = main_window;
  window_set_background_color(main_window, GColorClear);
  card_front = main_card_front;
  card_back = main_card_back;
  init_card_text();
  GRect bounds = layer_get_bounds(window_get_root_layer(window));

  load_card();
}

void deinit_cards(void) {
  layer_remove_child_layers(window_get_root_layer(window));
  //bitmap_layer_destroy(image_layer_front);
  text_layer_destroy(card_front.front_line_one);
  text_layer_destroy(card_front.front_line_two);
  text_layer_destroy(card_back.back_line_one);
  //free_card_image();
}

enum side_t flip_card(void) {
  if(current_side == FRONT) {
    //layer_set_hidden((Layer*)image_layer_front, true);
	hide_card_text_front();
    show_card_text();
    current_side = BACK;
  } else {
    //layer_set_hidden((Layer *)image_layer_front, false);
    hide_card_text();
	show_card_text_front();
    current_side = FRONT;
  }

  return (current_side);
}

enum side_t get_side(void) {
  return (current_side);
}

void rand_card(void) {
  while (card_used[current_card]) {
    current_card = rand() % NUMBER_OF_CARDS;
  }

  load_card();
}

void load_card(void) {
  //load_card_image();
  load_card_text(current_card);
  card_used[current_card] = true;

  if (!(--card_counter)) {
    reset_card_used();
  }
}

void init_card_text(void) {
  // Create the various text layers
  card_front.front_line_one  = text_layer_create(GRect(0, 0, 144, 120));
  card_front.front_line_two  = text_layer_create(GRect(0, 120, 144, 32));

  // Set text colors
  text_layer_set_text_color(card_front.front_line_one, GColorBlack);
  text_layer_set_text_color(card_front.front_line_two, GColorBlack);

  // Set background colors
  text_layer_set_background_color(card_front.front_line_one, GColorWhite);
  text_layer_set_background_color(card_front.front_line_two, GColorWhite);

  // Set fonts
  text_layer_set_font(card_front.front_line_one, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_font(card_front.front_line_two, fonts_get_system_font(FONT_KEY_GOTHIC_28));

  // Text alignment
  text_layer_set_text_alignment(card_front.front_line_one, GTextAlignmentCenter);
  text_layer_set_text_alignment(card_front.front_line_two, GTextAlignmentRight);

  // Add as child layers to window root
  layer_add_child(window_get_root_layer(window), (Layer*)card_front.front_line_one);
  layer_add_child(window_get_root_layer(window), (Layer*)card_front.front_line_two);
  
  // Create the various text layers
  card_back.back_line_one   = text_layer_create(GRect(0, 0, 144, 168));

  // Set text colors
  text_layer_set_text_color(card_back.back_line_one, GColorBlack);

  // Set background colors
  text_layer_set_background_color(card_back.back_line_one, GColorWhite);

  // Set fonts
  text_layer_set_font(card_back.back_line_one, fonts_get_system_font(FONT_KEY_GOTHIC_28));

  // Text alignment
  text_layer_set_text_alignment(card_back.back_line_one, GTextAlignmentCenter);

  // Add as child layers to window root
  layer_add_child(window_get_root_layer(window), (Layer*)card_back.back_line_one);

  hide_card_text();
}

void show_card_text(void) {
  layer_set_hidden((Layer*)card_back.back_line_one, false);
}

void show_card_text_front(void) {
  layer_set_hidden((Layer*)card_front.front_line_one, false);
  layer_set_hidden((Layer*)card_front.front_line_two, false);
}

void hide_card_text(void) {
  layer_set_hidden((Layer*)card_back.back_line_one, true);
}

void hide_card_text_front(void) {
  layer_set_hidden((Layer*)card_front.front_line_one, true);
  layer_set_hidden((Layer*)card_front.front_line_two, true);
}

void load_card_text(enum card_t aa) {
  text_layer_set_text(card_front.front_line_one, aa_front_line_one[aa]);
  text_layer_set_text(card_front.front_line_two, aa_front_line_two[aa]);
  text_layer_set_text(card_back.back_line_one, aa_back_line_one[aa]);
}

void reset_card_used(void) {
  for (uint8_t i = NUMBER_OF_CARDS; i; --i) {
    card_used[NUMBER_OF_CARDS - i] = false;
  }

  card_counter = NUMBER_OF_CARDS;
  srand(time(NULL));
}

/* Knock handler */
void accel_tap_handler(AccelAxisType axis, int32_t direction) {
	text_layer_set_text(text_layer, "");
	rand_card();
	if (get_side() == BACK) {
      	flip_card();
	}
}

//static void accel_data_handler(AccelData *data, uint32_t num_samples) {
void up_click_handler(ClickRecognizerRef recognizer, void *context) {
	
	//nothing for now
}

void down_click_handler(ClickRecognizerRef recognizer, void *context) {
	text_layer_set_text(text_layer, "");
	if (flip_card() == FRONT) {
    window_set_background_color(window, GColorWhite);
    text_layer_set_text_color(text_layer, GColorBlack);
  }
}

void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	
	//nothing for now
}

/* Register the individual button press callbacks */
void click_config_provider(void *context)
{
	window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
	window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

/* Load all Window sub-elements */
void window_load(Window *window){
	init_cards(window, card_front, card_back);
	//Add the creation of the Window's elements here
	text_layer = text_layer_create(GRect(0, 0, 144, 168)); //defines frame size
	text_layer_set_background_color(text_layer, GColorClear); //sets background color
	text_layer_set_text_color(text_layer, GColorBlack); //sets text color
	
	layer_add_child(window_get_root_layer(window), (Layer*) text_layer); //shows text in front
	text_layer_set_text(text_layer, "KNOCK KNOCK");
	text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
  	text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
	
	accel_tap_service_subscribe(accel_tap_handler);
}

/* Un-load all Window sub-elements */
void window_unload(Window *window)
{
	//Safely destroy the Window's elements here to free up memory
	text_layer_destroy(text_layer);
}

/* Initialize the main app elements */
void init()
{
	//Initialize the app elements here!
	accel_tap_service_subscribe(&accel_tap_handler);
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	window_set_click_config_provider(window, click_config_provider); //sets up the button click behaviors
	window_stack_push(window, true);
	
}

/* De-initialize the main app elements */
void deinit()
{
	//De-initialize elements here to save memory!
	accel_tap_service_unsubscribe();
	window_destroy(window);
}

/* Main app lifecycle */
int main(void) {
	init();
	app_event_loop();
	deinit();
}
	

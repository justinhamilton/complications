//Complications by Justin Hamilton
//Huge thanks to joravasal on Github: https://github.com/joravasal

#include <pebble.h>
#include "layer_procs.h"
#include "main.h"
#include <ctype.h>

Window *window;

TextLayer *message_layer;

GFont logo_font;

#define EDITABLE_COLORS_LENGTH 55

GColor background_color;
GColor face_color;
GColor accent_color;
GColor hand_color;
GColor bold_tick_color;

bool charging;

char colors[55];

TextLayer *disconnected, *lowbattery;

enum SlotKey {
  SLOT1KEY = 0x0,
  SLOT2KEY = 0x1,
  SLOT3KEY = 0x2,
  SLOT4KEY = 0x3,
  SECONDHANDKEY = 0x4,
  SHOWLOGOKEY = 0x5,
  SHOWMINUTESKEY = 0x6,
  SHOWHOURSKEY = 0x7,
  KEY_BASALTCOLORS = 0x8,
  KEY_BACKGROUNDCOLOR = 0x9,
  KEY_HANDCOLOR = 0x10,
  KEY_ACCENTCOLOR = 0x11,
  KEY_BOLDTICKCOLOR = 0x12,
  KEY_FACECOLOR = 0x13,
};

int show_second_hand = 1;

int slot1id;
int slot2id;
int slot3id;
int slotid4;

GBitmap *logo;

bool connected_bool;

int show_minute_ticks = 1;
int show_hour_ticks = 1;

BitmapLayer *logo_layer;

Layer *background, *hands, *complications_background, *complications_hands;

int battery = 0;

void handle_bluetooth(bool connected) {
  if(connected) {
    bluetooth = true;
  } else {
    bluetooth = false;
    if(slotid[0] == 8 || slotid[1] == 8 || slotid[2] == 8 || slotid[3] == 8) {
      vibes_short_pulse();
    }
  }
}

void handle_battery(BatteryChargeState charge_state) {
  battery = charge_state.charge_percent;
  if(charge_state.is_plugged == true) {
    charging = true;
  } else {
    charging = false;
  }
}

unsigned int HexStringToUInt(char const* hexstring) {
	unsigned int result = 0;
	char const *c = hexstring;
	unsigned char thisC;
	while( (thisC = *c) != 0 ) {
		thisC = toupper(thisC);
		result <<= 4;
		if( isdigit(thisC)) {
			result += thisC - '0';
     } else if(isxdigit(thisC)) {
			result += thisC - 'A' + 10;
     } else {
			APP_LOG(APP_LOG_LEVEL_WARNING, "ERROR: Unrecognised hex character \"%c\"", thisC);
			return 0;
		}
		++c;
	}
	return result;  
}

void apply_colors() {
  #ifdef PBL_COLOR
    
  int string_num = (EDITABLE_COLORS_LENGTH - 1) / 6;
    char color_strings[string_num][7];
    int colorInts[string_num];
  
      for (int i = 0; i < string_num; i++) {
        memcpy(color_strings[i], &colors[6*i], 6);
        color_strings[i][6] = '\0';
        colorInts[i] = HexStringToUInt(color_strings[i]);
      }
  
    background_color = GColorFromHEX(colorInts[0]);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "background set to: %i", colorInts[0]);
    persist_write_int(KEY_BACKGROUNDCOLOR, colorInts[0]);
  
    hand_color = GColorFromHEX(colorInts[1]);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "hand set to: %i", colorInts[1]);
    persist_write_int(KEY_HANDCOLOR, colorInts[1]);
  
    accent_color = GColorFromHEX(colorInts[2]);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "accent set to: %i", colorInts[2]);
    persist_write_int(KEY_ACCENTCOLOR, colorInts[2]);
  
    bold_tick_color = GColorFromHEX(colorInts[3]);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "bold tick set to: %i", colorInts[3]);
    persist_write_int(KEY_BOLDTICKCOLOR, colorInts[3]);
  
    face_color = GColorFromHEX(colorInts[4]);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "face set to: %i", colorInts[4]);
    persist_write_int(KEY_FACECOLOR, colorInts[4]);
  
    window_set_background_color(window, background_color);
  #endif
}

//JS Stuff
static void in_recv_handler(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "JS Message Recieved");
  Tuple *t = dict_read_first(iterator);
  
  char new_bc[6];
  char new_hc[6];
  char new_ac[6];
  
  while(t) {
    switch(t->key) {
      case SLOT1KEY:
        slotid[0] = t->value->int16;
        persist_write_int(SLOT1KEY, t->value->int16);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Slot 1 set to: %i", slotid[0]);
        break;
      case SLOT2KEY:
        slotid[1] = t->value->int16;
        persist_write_int(SLOT2KEY, t->value->int16);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Slot 2 set to: %i", slotid[1]);
        break;
      case SLOT3KEY:
        slotid[2] = t->value->int16;
        persist_write_int(SLOT3KEY, t->value->int16);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Slot 3 set to: %i", slotid[2]);
        break;
      case SLOT4KEY:
        slotid[3] = t->value->int16;
        persist_write_int(SLOT4KEY, t->value->int16);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Slot 4 set to: %i", slotid[3]);
        break;
      case SECONDHANDKEY:
        show_second_hand = t->value->int16;
        persist_write_int(SECONDHANDKEY, show_second_hand);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Show seconds set to: %i", show_second_hand);
        break;
      case SHOWHOURSKEY:
        show_hour_ticks = t->value->int16;
        persist_write_int(SHOWHOURSKEY, show_hour_ticks);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Show hour ticks set to: %i", show_hour_ticks);
        break;
      case SHOWMINUTESKEY:
        show_minute_ticks = t->value->int16;
        persist_write_int(SHOWMINUTESKEY, show_minute_ticks);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Show minute ticks set to: %i", show_minute_ticks);
        break;
      case SHOWLOGOKEY:
        show_logo = t->value->int16;
        persist_write_int(SHOWLOGOKEY, show_logo);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Show logo set to: %i", show_logo);
        break;
      case KEY_BASALTCOLORS:
        memcpy(colors, t->value->cstring, t->length);
        persist_write_string(KEY_BASALTCOLORS, colors);
        APP_LOG(APP_LOG_LEVEL_DEBUG, "colors: %s", t->value->cstring);
        break;
    }
    t = dict_read_next(iterator);
  }
  for(int i = 0; i<4; i++) {
    mem_slotid[i] = slotid[i];
  }
  
  apply_colors();
  
  vibes_short_pulse();
}

static void tick_handler(struct tm *t, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

void handle_init(void) {
  logo_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_LOGO_FONT_15));
  window = window_create();
  #ifdef PBL_COLOR
    background_color = GColorFromHEX(0x000000);
    face_color = GColorFromHEX(0x000000);
    accent_color = GColorFromHEX(0xFF0000);
    hand_color = GColorFromHEX(0xFFFFFF);
    bold_tick_color = GColorFromHEX(0xFFFFFF);
  #else
    background_color = GColorBlack;
    face_color = GColorWhite;
    accent_color = GColorBlack;
    hand_color = GColorBlack;
    bold_tick_color = GColorBlack;
  #endif
  
  #ifdef PBL_COLOR
    background_color = GColorFromHEX(persist_exists(KEY_BACKGROUNDCOLOR) ? persist_read_int(KEY_BACKGROUNDCOLOR): 5592405);
    hand_color = GColorFromHEX(persist_exists(KEY_HANDCOLOR) ? persist_read_int(KEY_HANDCOLOR) : 16711680);
    accent_color = GColorFromHEX(persist_exists(KEY_ACCENTCOLOR) ? persist_read_int(KEY_ACCENTCOLOR) : 5592405);
    bold_tick_color = GColorFromHEX(persist_exists(KEY_BOLDTICKCOLOR) ? persist_read_int(KEY_BOLDTICKCOLOR) : 5592405);
    face_color = GColorFromHEX(persist_exists(KEY_FACECOLOR) ? persist_read_int(KEY_FACECOLOR) : 16777215);
  #endif
    
  window_set_background_color(window, background_color);
    
  logo = gbitmap_create_with_resource(RESOURCE_ID_LOGO_IMAGE);
  
  slotid[0] = persist_exists(SLOT1KEY) ? persist_read_int(SLOT1KEY) : 0;
  slotid[1] = persist_exists(SLOT2KEY) ? persist_read_int(SLOT2KEY) : 0;
  slotid[2] = persist_exists(SLOT3KEY) ? persist_read_int(SLOT3KEY) : 0;
  slotid[3] = persist_exists(SLOT4KEY) ? persist_read_int(SLOT4KEY) : 0; 
  show_second_hand = persist_exists(SECONDHANDKEY) ? persist_read_int(SECONDHANDKEY) : 1;
  show_logo = persist_exists(SHOWLOGOKEY) ? persist_read_int(SHOWLOGOKEY) : 1;
  show_minute_ticks = persist_exists(SHOWMINUTESKEY) ? persist_read_int(SHOWMINUTESKEY) : 1;
  show_hour_ticks = persist_exists(SHOWHOURSKEY) ? persist_read_int(SHOWHOURSKEY) : 1;
  
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Slot 1 initialized to: %i", slotid[0]);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Slot 2 initialized to: %i", slotid[1]);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Slot 3 initialized to: %i", slotid[2]);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Slot 4 initialized to: %i", slotid[3]);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Show Seconds initialized to: %i", show_second_hand);
  
  for(int i = 0; i < 4; i++) {
    mem_slotid[i] = slotid[i];
  }
  
  //Background Layer
  background = layer_create(GRect(0,0,144,168));
  layer_set_update_proc(background, background_update);
  layer_add_child(window_get_root_layer(window), background);
  
  //Complications Background Layer
  complications_background = layer_create(GRect(0,0,144,168));
  layer_set_update_proc(complications_background, complications_background_update);
  layer_add_child(window_get_root_layer(window), complications_background);
  
  //Logo Layer
  /*logo_layer = bitmap_layer_create(GRect(0,0,144,110));
  bitmap_layer_set_bitmap(logo_layer, logo);
  layer_add_child(window_get_root_layer(window), (Layer*)logo_layer);
  layer_set_hidden((Layer*)logo_layer, true);*/
  
  //Complications Hands Layer
  complications_hands = layer_create(GRect(0,0,144,168));
  layer_set_update_proc(complications_hands, complications_hands_update);
  layer_add_child(window_get_root_layer(window), complications_hands);
  
  //Hands Layer
  hands = layer_create(GRect(0,0,144,168));
  layer_set_update_proc(hands, hands_update);
  layer_add_child(window_get_root_layer(window), hands);
  
  //Subscribe to Time Service
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
    
  /*message_layer = text_layer_create(GRect(0,50,144,24));
  text_layer_set_font(message_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24));
  text_layer_set_text(message_layer, "Hello World");
  text_layer_set_text_color(message_layer, GColorBlack);
  text_layer_set_background_color(message_layer, GColorClear);
  text_layer_set_text_alignment(message_layer, GTextAlignmentCenter);
  layer_add_child((Layer*)window, (Layer*)message_layer);*/
  
  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  app_message_open(1000, app_message_outbox_size_maximum());
  
  battery_state_service_subscribe(&handle_battery);
  handle_battery(battery_state_service_peek());
  
  bluetooth_connection_service_subscribe(&handle_bluetooth);
  handle_bluetooth(bluetooth_connection_service_peek());
  
  window_stack_push(window, true);
}

void handle_deinit(void) {
  window_destroy(window);
  layer_destroy(background);
  layer_destroy(hands);
  layer_destroy(complications_background);
  layer_destroy(complications_hands);
  
  tick_timer_service_unsubscribe();
}

int main(void) {
  handle_init();
  app_event_loop();
  handle_deinit();
}


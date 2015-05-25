//Complications by Justin Hamilton
//Huge thanks to joravasal on Github: https://github.com/joravasal

#include <pebble.h>
#include "layer_procs.h"
#include "main.h"
#include <ctype.h>

Window *window;

enum SlotKey {
  SLOT1KEY = 0x1,
  SLOT2KEY = 0x2,
  SLOT3KEY = 0x3,
  SLOT4KEY = 0x4,
  SECONDHANDKEY = 0x5,
};

int show_second_hand = 1;

int slot1id;
int slot2id;
int slot3id;
int slotid4;

GBitmap *logo;

BitmapLayer *logo_layer;

Layer *background, *hands, *complications_background, *complications_hands;

int battery = 0;

void handle_battery(BatteryChargeState charge_state) {
  battery = charge_state.charge_percent;
  if(battery <= 20 || charge_state.is_plugged) {
    layer_set_hidden(complications_hands, true);
    slotid[0] = 0;
    slotid[1] = 0;
    slotid[2] = 7;
    slotid[3] = 0;
    show_second_hand = 0;
  } else {
    for(int i = 0; i < 4; i++) {
      show_second_hand = persist_read_int(SECONDHANDKEY);
      slotid[i] = mem_slotid[i];
      layer_set_hidden(complications_hands, false);
    }
  }
}

//JS Stuff
static void in_recv_handler(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "JS Message Recieved");
  Tuple *t = dict_read_first(iterator);
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
        APP_LOG(APP_LOG_LEVEL_DEBUG, "Show seconds: %i", show_second_hand);
        break;
    }
    t = dict_read_next(iterator);
  }
  for(int i = 0; i<4; i++) {
    mem_slotid[i] = slotid[i];
  }
  vibes_short_pulse();
}

static void tick_handler(struct tm *t, TimeUnits units_changed) {
  layer_mark_dirty(window_get_root_layer(window));
}

void handle_init(void) {
  window = window_create();
  #ifdef PBL_COLOR
    window_set_background_color(window, GColorDarkGray);
  #else
    window_set_background_color(window, GColorBlack);
  #endif
    
  logo = gbitmap_create_with_resource(RESOURCE_ID_LOGO_IMAGE);
  
  slotid[0] = persist_exists(SLOT1KEY) ? persist_read_int(SLOT1KEY) : 0;
  slotid[1] = persist_exists(SLOT2KEY) ? persist_read_int(SLOT2KEY) : 0;
  slotid[2] = persist_exists(SLOT3KEY) ? persist_read_int(SLOT3KEY) : 0;
  slotid[3] = persist_exists(SLOT4KEY) ? persist_read_int(SLOT4KEY) : 0; 
  show_second_hand = persist_exists(SECONDHANDKEY) ? persist_read_int(SECONDHANDKEY) : 1;
  
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
  logo_layer = bitmap_layer_create(GRect(0,0,144,110));
  bitmap_layer_set_bitmap(logo_layer, logo);
  layer_add_child(window_get_root_layer(window), (Layer*)logo_layer);
  layer_set_hidden((Layer*)logo_layer, true);
  
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
  
  app_message_register_inbox_received((AppMessageInboxReceived) in_recv_handler);
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
  
  battery_state_service_subscribe(&handle_battery);
  handle_battery(battery_state_service_peek());
  
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


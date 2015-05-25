#include <pebble.h>
#include "layer_procs.h"
#include "main.h"
  
int slotid[4] = {0,0,0,0};
int mem_slotid[4] = {0,0,0,0};
//0 = none, 1 = seconds, 2 = month, 3 = day dial, 4 = weekday, 5 = day box, 6 = logo (top only), 7 = battery gauge

//Background Update Proc  
void background_update(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
  //Outer White Circle
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 67);
  
  //Middle Circle
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorDarkGray);
  #else
    graphics_context_set_fill_color(ctx, GColorBlack);
  #endif
  graphics_fill_circle(ctx, center, 65);
  
  //Inner White Circle
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 63);
  
  //Minute Ticks (1-60)
  #ifdef PBL_COLOR
    graphics_context_set_stroke_color(ctx, GColorDarkGray);
  #else
    graphics_context_set_stroke_color(ctx, GColorBlack);
  #endif
    
  for(int i = 0; i < 60; i++) {
    int32_t tick_angle = TRIG_MAX_ANGLE * i / 60;
    GPoint tick = {
      .x = (int16_t)(sin_lookup(tick_angle) * (int32_t)65 / TRIG_MAX_RATIO) + center.x,
      .y = (int16_t)(-cos_lookup(tick_angle) * (int32_t)65 / TRIG_MAX_RATIO) + center.y,
    };
    graphics_draw_line(ctx, tick, center);
  }
  
  graphics_fill_circle(ctx, center, 58);
  
  //Hour Ticks (1-12)
  #ifdef PBL_COLOR
    graphics_context_set_stroke_width(ctx, 2);
    
    for(int i = 0; i < 60; i += 5) {
      int32_t tick_angle = TRIG_MAX_ANGLE * i / 60;
      GPoint tick = {
        .x = (int16_t)(sin_lookup(tick_angle) * (int32_t)65 / TRIG_MAX_RATIO) + center.x,
        .y = (int16_t)(-cos_lookup(tick_angle) * (int32_t)65 / TRIG_MAX_RATIO) + center.y,
      };
      graphics_draw_line(ctx, tick, center);
    }
    
    graphics_fill_circle(ctx, center, 56);
  #else
    for(int i = 0; i < 60; i += 5) {
      int32_t tick_angle = TRIG_MAX_ANGLE * i / 60;
      GPoint tick = {
        .x = (int16_t)(sin_lookup(tick_angle) * (int32_t)65 / TRIG_MAX_RATIO) + center.x,
        .y = (int16_t)(-cos_lookup(tick_angle) * (int32_t)65 / TRIG_MAX_RATIO) + center.y,
      };
      graphics_draw_line(ctx, tick, center);
    }
    
    graphics_fill_circle(ctx, center, 54);
  #endif
}

//Hands Update Proc
void hands_update(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  #ifdef PBL_COLOR
    graphics_context_set_stroke_color(ctx, GColorDarkGray);
    graphics_context_set_stroke_width(ctx, 2);
  #else
    graphics_context_set_stroke_color(ctx, GColorBlack);
  #endif
  
  //Length Declarations
  int16_t minute_hand_length = 54;
  int16_t second_hand_length = minute_hand_length;
  int16_t hour_hand_length = 40;
  
  //Hour Hand
  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  GPoint hour_hand = {
    .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)hour_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)hour_hand_length / TRIG_MAX_RATIO) + center.y,
  };
  graphics_draw_line(ctx, hour_hand, center);
  
  //Minute Hand
  int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
  GPoint minute_hand = {
    .x = (int16_t)(sin_lookup(minute_angle) * (int32_t)minute_hand_length / TRIG_MAX_RATIO) + center.x,
    .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)minute_hand_length / TRIG_MAX_RATIO) + center.y,
  };
  graphics_draw_line(ctx, minute_hand, center);
  
  //Second Hand
  if(slotid[0] != 1 && slotid[1] != 1 && slotid[2] != 1 && slotid[3] != 1 && show_second_hand != 0) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Seconds Update: Show Second Hand Current Value: %i", show_second_hand);
    #ifdef PBL_COLOR
      graphics_context_set_stroke_color(ctx, GColorRed);
    #endif
      
    int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
    GPoint second_hand = {
      .x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
      .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
    };
    graphics_draw_line(ctx, second_hand, center);
    #ifdef PBL_COLOR
      graphics_context_set_fill_color(ctx, GColorRed);
      graphics_fill_circle(ctx, center, 5);
    #endif
  }
  
  //Circle in the Center
    
  #ifdef PBL_COLOR
    graphics_context_set_fill_color(ctx, GColorDarkGray);
  #else
    graphics_context_set_fill_color(ctx, GColorBlack);
  #endif
    
  graphics_fill_circle(ctx, center, 3);
  
  graphics_context_set_fill_color(ctx, GColorWhite);
  
  graphics_fill_circle(ctx, center, 1);
}

//Complications Background Update Proc
void complications_background_update(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
  int segments = 0;
  int thin = 0;
  int bold = 0;
  bool circle = true;
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  static char date_text[] = "00";
  static char battery_text[] = "000";
  
  //Slot Points
  GPoint slot[4] = {GPoint(center.x, center.y-30), GPoint(center.x+30, center.y), GPoint(center.x,center.y+30), GPoint(center.x-30,center.y)};
  #ifdef PBL_COLOR
    graphics_context_set_stroke_color(ctx, GColorDarkGray);
    graphics_context_set_text_color(ctx, GColorDarkGray);
  #else
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_context_set_text_color(ctx, GColorBlack);
  #endif
  for(int i = 0; i < 4; i++) {
    for(int h = 0; h < 4; h++) {
      switch(slotid[i]) {
        case 0:
          segments = 0;
          thin = 0;
          bold = 0;
          circle = false;
          if(i == 0) {
            layer_set_hidden((Layer*)logo_layer, true);
          }
          break;
        case 1:
          segments = 60;
          thin = 12;
          bold = 4;
          circle = true;
          show_second_hand = 0;
          if(i == 0) {
            layer_set_hidden((Layer*)logo_layer, true);
          }
          break;
        case 2:
          segments = 60;
          thin = 0;
          bold = 12;
          circle = true;
          if(i == 0) {
            layer_set_hidden((Layer*)logo_layer, true);
          }
          break;
        case 3:
          segments = 31;
          thin = 31;
          bold = 6;
          circle = true;
          if(i == 0) {
            layer_set_hidden((Layer*)logo_layer, true);
          }
          break;
        case 4:
          segments = 7;
          thin = 0;
          bold = 7;
          circle = true;
          if(i == 0) {
            layer_set_hidden((Layer*)logo_layer, true);
          }
          break;
        case 5:
          segments = 0;
          thin = 0;
          bold = 0;
          circle = false;
          if(i == 0) {
            layer_set_hidden((Layer*)logo_layer, true);
          }
          graphics_draw_round_rect(ctx, GRect(slot[i].x-8,slot[i].y-7, 16, 15), 3);
          strftime(date_text, sizeof(date_text), "%d", t);
          graphics_draw_text(ctx, date_text, fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(slot[i].x-7,slot[i].y-9,14,8), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
          break;
        case 7:
          if(i == 0) {
            segments = 0;
            thin = 0;
            bold = 0;
            layer_set_hidden((Layer*)logo_layer, false);
            circle = false;
          } else {
            segments = 0;
            thin = 0;
            bold = 0;
            layer_set_hidden((Layer*)logo_layer, true);
            circle = false;
          }
          break;
        case 6:
          segments = 0;
          thin = 0;
          bold = 0;
          circle = false;
          #ifdef PBL_COLOR
            if(battery == 100) {
              graphics_context_set_fill_color(ctx, GColorGreen);
            } else if(battery <= 90 && battery > 20) {
              graphics_context_set_fill_color(ctx, GColorChromeYellow);
            } else if (battery <= 20) {
              graphics_context_set_fill_color(ctx, GColorRed);
              graphics_context_set_text_color(ctx, GColorWhite);
            }
          #else
            graphics_context_set_text_color(ctx, GColorBlack);
            graphics_context_set_fill_color(ctx, GColorWhite);
            graphics_context_set_stroke_color(ctx, GColorBlack);
          #endif
          snprintf(battery_text, sizeof(battery_text), "%i", battery);
          if(battery == 100) {
            graphics_fill_rect(ctx, GRect(slot[i].x-11,slot[i].y-7, 23, 15), 3, GCornersAll);
            graphics_draw_round_rect(ctx, GRect(slot[i].x-11,slot[i].y-7, 23, 15), 3);
            graphics_draw_text(ctx, battery_text, fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(slot[i].x-12,slot[i].y-9,24,8), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL); 
          } else {
            graphics_fill_rect(ctx, GRect(slot[i].x-8,slot[i].y-7, 16, 15), 3, GCornersAll);
            graphics_draw_round_rect(ctx, GRect(slot[i].x-8,slot[i].y-7, 16, 15), 3);
            graphics_draw_text(ctx, battery_text, fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(slot[i].x-7,slot[i].y-9,14,8), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
          }
          if(i == 0) {
            layer_set_hidden((Layer*)logo_layer, true);
          }
          break;
      }
    }
    #ifdef PBL_COLOR
      graphics_context_set_stroke_color(ctx, GColorDarkGray);
    #else
      graphics_context_set_stroke_color(ctx, GColorBlack);
    #endif
    if(thin != 0) {
      for(int j = 0; j < segments; j+=(segments/thin)) {
        int32_t tick_angle = TRIG_MAX_ANGLE * j / segments;
        GPoint tick = {
          .x = (int16_t)(sin_lookup(tick_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].x,
          .y = (int16_t)(-cos_lookup(tick_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].y,
        };
        graphics_draw_line(ctx, tick, slot[i]);
      }
    }
    #ifdef PBL_COLOR
    #else
      if(circle) {
        graphics_fill_circle(ctx, slot[i], 12);
      }
    #endif
    #ifdef PBL_COLOR
      graphics_context_set_stroke_width(ctx, 2);
    #endif
    if(bold != 0) {
      if(slotid[i] == 3) {
        for(int j = 0; j < 30; j+=5) {
          int32_t tick_angle = TRIG_MAX_ANGLE * j / 31;
          GPoint tick = {
            .x = (int16_t)(sin_lookup(tick_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].x,
            .y = (int16_t)(-cos_lookup(tick_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].y,
          };
          graphics_draw_line(ctx, tick, slot[i]);
        }
      } else {
        for(int j = 0; j < segments; j+=(segments/bold)) {
          int32_t tick_angle = TRIG_MAX_ANGLE * j / segments;
          GPoint tick = {
            .x = (int16_t)(sin_lookup(tick_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].x,
            .y = (int16_t)(-cos_lookup(tick_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].y,
          };
          
          graphics_draw_line(ctx, tick, slot[i]);
        }
      }
    }
    #ifdef PBL_COLOR
      graphics_context_set_stroke_width(ctx, 1);
    #endif
    graphics_context_set_fill_color(ctx, GColorWhite);
    #ifdef PBL_COLOR
      if(circle) {
        graphics_fill_circle(ctx, slot[i], 12);
      }
    #else
      if(circle) {
        graphics_fill_circle(ctx, slot[i], 12);
      }
    #endif
  }
}

//Complications Hands Update Proc
void complications_hands_update(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  int32_t month_angle = TRIG_MAX_ANGLE * t->tm_mon / 12;
  int32_t weekday_angle = TRIG_MAX_ANGLE * t->tm_wday / 7;
  int32_t day_angle = TRIG_MAX_ANGLE * t->tm_mday / 30;
  
  #ifdef PBL_COLOR
    graphics_context_set_stroke_color(ctx, GColorRed);
    graphics_context_set_stroke_width(ctx, 2);
  #else
    graphics_context_set_stroke_color(ctx, GColorBlack);
  #endif
  GPoint slot[4] = {GPoint(center.x, center.y-30), GPoint(center.x+30, center.y), GPoint(center.x,center.y+30), GPoint(center.x-30,center.y)};
  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  for(int i = 0; i < 4; i++) {
    for(int h = 0; h < 4; h++) {
      if(slotid[i] == 1) {
        GPoint second_hand = {
          .x = (int16_t)(sin_lookup(second_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].x,
          .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].y,
        };
        graphics_draw_line(ctx, second_hand, slot[i]);
      } else if(slotid[i] == 2) {
        GPoint month_hand = {
          .x = (int16_t)(sin_lookup(month_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].x,
          .y = (int16_t)(-cos_lookup(month_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].y,
        };
        graphics_draw_line(ctx, month_hand, slot[i]);
      } else if(slotid[i] == 3) {
        GPoint day_hand = {
          .x = (int16_t)(sin_lookup(day_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].x,
          .y = (int16_t)(-cos_lookup(day_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].y,
        };
        graphics_draw_line(ctx, day_hand, slot[i]);
      } else if(slotid[i] == 4) {
        GPoint weekday_hand = {
          .x = (int16_t)(sin_lookup(weekday_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].x,
          .y = (int16_t)(-cos_lookup(weekday_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].y,
        };
        graphics_draw_line(ctx, weekday_hand, slot[i]);
      }
    }
    if(slotid[i] != 0 && slotid[i] != 5 && slotid[i] != 6 && slotid[i] != 7) {
      #ifdef PBL_COLOR
        graphics_context_set_fill_color(ctx, GColorRed);
      #else
        graphics_context_set_fill_color(ctx, GColorBlack);
      #endif
      graphics_fill_circle(ctx, slot[i], 3);
      graphics_context_set_fill_color(ctx, GColorWhite);
      graphics_fill_circle(ctx, slot[i], 1);
    }
  }
}
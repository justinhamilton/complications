#include <pebble.h>
#include "layer_procs.h"
#include "main.h"
  
int slotid[4] = {0,0,0,0};
int mem_slotid[4] = {0,0,0,0};
//0 = none, 1 = seconds, 2 = month, 3 = day dial, 4 = weekday, 5 = day box, 6 = battery gauge, 7 = bluetooth monitor, 8 = battery icon, 9 = day name, 10 = digital time, 11 = digital date

GPath *bluetooth_icon_path, *charge_icon_path;

const GPathInfo BLUETOOTH_ICON_POINTS = {
  8,
  (GPoint []) {
    {-8, -4},
    {0, 4},
    {-4, 8},
    {-4, -8},
    {0, -4},
    {-8, 4},
    {-4, 0},
    {-8,-4},
  }
};

const GPathInfo CHARGE_ICON_POINTS = {
  7,
  (GPoint []) {
    {0,-1},
    {-2,0},
    {2,0},
    {0,3},
    {2,0},
    {-2,0},
    {0,-3},
  }
};

/*#ifdef PBL_COLOR
  
#else
  GPath *minute_hand_path, *hour_hand_path, *second_hand_path, *complication_bold_path, *clock_bold_path;
  const GPathInfo MINUTE_HAND_POINTS = {
    5,
    (GPoint []) {
      {3,0},
      {3,-52},
      {0,-54},
      {-3,-52},
      {-3,0},
    }
  };
  const GPathInfo SECOND_HAND_POINTS = {
    5,
    (GPoint []) {
      {3,0},
      {3,-52},
      {0,-54},
      {-3,-52},
      {-3,0},
    }
  };
  const GPathInfo HOUR_HAND_POINTS = {
    5,
    (GPoint []) {
      {3,0},
      {3,-38},
      {0,-40},
      {-3,-38},
      {-3,0},
    }
  };
  const GPathInfo COMPLICATION_BOLD_POINTS = {
    4,
    (GPoint []) {
      {1,0},
      {1,-15},
      {-1,-15},
      {-1,0},
    }
  };
#endif*/

int show_logo = 0;

bool bluetooth = true;

//Background Update Proc  
void background_update(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
  //Outer White Circle
  graphics_context_set_fill_color(ctx, face_color);
  graphics_fill_circle(ctx, center, 67);
  
  //Middle Circle
  graphics_context_set_fill_color(ctx, accent_color);
  graphics_fill_circle(ctx, center, 65);
  
  //Inner White Circle
  graphics_context_set_fill_color(ctx, face_color);
  graphics_fill_circle(ctx, center, 63);
  
  //Minute Ticks (1-60)
  if(show_minute_ticks) {
    graphics_context_set_stroke_color(ctx, accent_color);
      for(int i = 0; i < 60; i++) {
        int32_t tick_angle = TRIG_MAX_ANGLE * i / 60;
        GPoint tick = {
          .x = (int16_t)(sin_lookup(tick_angle) * (int32_t)65 / TRIG_MAX_RATIO) + center.x,
          .y = (int16_t)(-cos_lookup(tick_angle) * (int32_t)65 / TRIG_MAX_RATIO) + center.y,
        };
        graphics_draw_line(ctx, tick, center);
      }
      
      graphics_fill_circle(ctx, center, 58);
  }
  
  //Hour Ticks (1-12)
  if(show_hour_ticks) {
    #ifdef PBL_COLOR
        graphics_context_set_stroke_width(ctx, 2);
        graphics_context_set_stroke_color(ctx, bold_tick_color);
        
        for(int i = 0; i < 60; i += 5) {
          int32_t tick_angle = TRIG_MAX_ANGLE * i / 60;
          GPoint tick = {
            .x = (int16_t)(sin_lookup(tick_angle) * (int32_t)63 / TRIG_MAX_RATIO) + center.x,
            .y = (int16_t)(-cos_lookup(tick_angle) * (int32_t)63 / TRIG_MAX_RATIO) + center.y,
          };
          graphics_draw_line(ctx, tick, center);
        }
        
        graphics_fill_circle(ctx, center, 56);
      #else
      #endif
  }
}

//Hands Update Proc
void hands_update(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);
  
  time_t now = time(NULL);
  struct tm *t = localtime(&now);
  
  graphics_context_set_stroke_color(ctx, accent_color);
  
  #ifdef PBL_COLOR
    graphics_context_set_stroke_width(ctx, 2);
  #endif
  
  //Length Declarations
  int16_t minute_hand_length = 54;
  int16_t second_hand_length = minute_hand_length;
  int16_t hour_hand_length = 40;
  
  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
  
  #ifdef PBL_COLOR
    //Hour Hand
    GPoint hour_hand = {
      .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)hour_hand_length / TRIG_MAX_RATIO) + center.x,
      .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)hour_hand_length / TRIG_MAX_RATIO) + center.y,
    };
    graphics_draw_line(ctx, hour_hand, center);
    
    //Minute Hand
    GPoint minute_hand = {
      .x = (int16_t)(sin_lookup(minute_angle) * (int32_t)minute_hand_length / TRIG_MAX_RATIO) + center.x,
      .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)minute_hand_length / TRIG_MAX_RATIO) + center.y,
    };
    graphics_draw_line(ctx, minute_hand, center);
    
    //Second Hand
    if(show_second_hand != 0) {
      
      graphics_context_set_stroke_color(ctx, hand_color);
      GPoint second_hand = {
        .x = (int16_t)(sin_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.x,
        .y = (int16_t)(-cos_lookup(second_angle) * (int32_t)second_hand_length / TRIG_MAX_RATIO) + center.y,
      };
      graphics_draw_line(ctx, second_hand, center);
    }
    #else
      /*graphics_context_set_fill_color(ctx, GColorBlack);
      graphics_context_set_stroke_color(ctx, GColorBlack);
      gpath_destroy(minute_hand_path);
      gpath_destroy(second_hand_path);
      gpath_destroy(hour_hand_path);
      minute_hand_path = gpath_create(&MINUTE_HAND_POINTS);
      hour_hand_path = gpath_create(&HOUR_HAND_POINTS);
      second_hand_path = gpath_create(&SECOND_HAND_POINTS);
      gpath_move_to(second_hand_path, center);
      gpath_move_to(hour_hand_path, center);
      gpath_move_to(minute_hand_path, center);
      gpath_rotate_to(minute_hand_path, minute_angle);
      gpath_draw_filled(ctx, minute_hand_path);
      gpath_rotate_to(hour_hand_path, hour_angle);
      gpath_draw_filled(ctx, hour_hand_path);
      graphics_context_set_fill_color(ctx, GColorWhite);
      gpath_rotate_to(second_hand_path, second_angle);
      gpath_draw_filled(ctx, second_hand_path);
      gpath_draw_outline(ctx, second_hand_path);*/
    #endif
    #ifdef PBL_COLOR
      graphics_context_set_fill_color(ctx, hand_color);
      graphics_fill_circle(ctx, center, 5);
    #endif
  
  //Circle in the Center
  graphics_context_set_fill_color(ctx, accent_color);
    
  graphics_fill_circle(ctx, center, 3);
  
  graphics_context_set_fill_color(ctx, face_color);
  
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
  
    graphics_context_set_stroke_color(ctx, accent_color);
    graphics_context_set_text_color(ctx, accent_color);
  for(int i = 0; i < 4; i++) {
    if(show_logo == 1) {
          if(i == 0) {
            segments = 0;
            thin = 0;
            bold = 0;
            static char logo_text[] = "pebble";
            graphics_draw_text(ctx, logo_text, logo_font, GRect(0,slot[i].y-15,144,20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
            //layer_set_hidden((Layer*)logo_layer, false);
            circle = false;
          } else {
            segments = 0;
            thin = 0;
            bold = 0;
            //layer_set_hidden((Layer*)logo_layer, true);
            circle = false;
          }
    }
    for(int h = 0; h < 4; h++) {
      switch(slotid[i]) {
        case 0:
          segments = 0;
          thin = 0;
          bold = 0;
          circle = false;
          if(i == 0) {
            //layer_set_hidden((Layer*)logo_layer, true);
          }
          break;
        case 1:
          segments = 60;
          thin = 12;
          bold = 4;
          circle = true;
          if(i == 0) {
            //layer_set_hidden((Layer*)logo_layer, true);
          }
          break;
        case 2:
          segments = 60;
          thin = 0;
          bold = 12;
          circle = true;
          if(i == 0) {
            //layer_set_hidden((Layer*)logo_layer, true);
          }
          break;
        case 3:
          segments = 31;
          thin = 31;
          bold = 6;
          circle = true;
          if(i == 0) {
            //layer_set_hidden((Layer*)logo_layer, true);
          }
          break;
        case 4:
          segments = 7;
          thin = 0;
          bold = 7;
          circle = true;
          if(i == 0) {
            //layer_set_hidden((Layer*)logo_layer, true);
          }
          break;
        case 5:
          segments = 0;
          thin = 0;
          bold = 0;
          circle = false;
          if(i == 0) {
            //layer_set_hidden((Layer*)logo_layer, true);
          }
          graphics_context_set_stroke_color(ctx, accent_color);
          graphics_context_set_text_color(ctx, hand_color);
          graphics_draw_round_rect(ctx, GRect(slot[i].x-8,slot[i].y-7, 16, 15), 3);
          strftime(date_text, sizeof(date_text), "%d", t);
          graphics_draw_text(ctx, date_text, fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(slot[i].x-7,slot[i].y-9,14,8), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
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
          break;
        case 7:
          gpath_destroy(bluetooth_icon_path);
          graphics_context_set_stroke_color(ctx, accent_color);
          bluetooth_icon_path = gpath_create(&BLUETOOTH_ICON_POINTS);
          gpath_move_to(bluetooth_icon_path, slot[i]);
          gpath_draw_outline(ctx, bluetooth_icon_path);
          #ifdef PBL_COLOR
            if(bluetooth) {
              graphics_context_set_fill_color(ctx, GColorGreen);
              graphics_fill_circle(ctx, GPoint(slot[i].x+5,slot[i].y+4), 2);
              graphics_context_set_fill_color(ctx, GColorBulgarianRose);
              graphics_fill_circle(ctx, GPoint(slot[i].x+5,slot[i].y-4), 2);
            }
            
            if(bluetooth == false) {
              graphics_context_set_fill_color(ctx, GColorDarkGreen);
              graphics_fill_circle(ctx, GPoint(slot[i].x+5,slot[i].y+4), 2);
              graphics_context_set_fill_color(ctx, GColorRed);
              graphics_fill_circle(ctx, GPoint(slot[i].x+5,slot[i].y-4), 2);
            }
          #endif
          segments = 0;
          bold = 0;
          thin = 0;
          circle = false;
          break;
        case 11:
          if(i == 0 || i == 2) {
            circle = false;
            segments = 0;
            bold = 0;
            thin = 0;
            if(i == 0 || i == 2) {
              char date_text[] = "00/00/00";
              graphics_context_set_stroke_color(ctx, accent_color);
              graphics_context_set_text_color(ctx, hand_color);
              strftime(date_text, sizeof(date_text), "%m/%d/%y", t);
              graphics_draw_round_rect(ctx, GRect(slot[i].x-26,slot[i].y-7, 52, 15), 3);
              graphics_draw_text(ctx, date_text, fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(slot[i].x-25,slot[i].y-9,50,20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
            }
          }
          break;
        case 9:
          bold = 0;
          thin = 0;
          segments = 0;
          circle = false;
          if(i == 0 || i == 2) {
            char day_text[10];
            int length = 0;
            switch(t->tm_wday) {
              case 0:
                snprintf(day_text, sizeof(day_text), "SUNDAY");
                length=7;
                break;
              case 1:
                snprintf(day_text, sizeof(day_text), "MONDAY");
                length=7;
                break;
              case 2:
                snprintf(day_text, sizeof(day_text), "TUESDAY");
                length=8;
                break;
              case 3:
                snprintf(day_text, sizeof(day_text), "WEDNESDAY");
                length=10;
                break;
              case 4:
                snprintf(day_text, sizeof(day_text), "THURSDAY");
                length=9;
                break;
              case 5:
                snprintf(day_text, sizeof(day_text), "FRIDAY");
                length=7;
                break;
              case 6:
                snprintf(day_text, sizeof(day_text), "SATURDAY");
                length=9;
                break;
            }
            graphics_context_set_stroke_color(ctx, accent_color);
            graphics_draw_round_rect(ctx, GRect(slot[i].x-(length*7)/2,slot[i].y-7, length*7, 15), 3);
            graphics_context_set_text_color(ctx, hand_color);
            graphics_draw_text(ctx, day_text, fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(0,slot[i].y-9,144,20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
          }
          break;
        case 10:
          if(i == 0 || i == 2) {
            circle = false;
            segments = 0;
            bold = 0;
            thin = 0;
            if(i == 0 || i == 2) {
              char time_text[] = "00:00:00";
              graphics_context_set_text_color(ctx, hand_color);
              graphics_context_set_stroke_color(ctx, accent_color);
              if(clock_is_24h_style() == true) {
                strftime(time_text, sizeof(time_text), "%H:%M:%S", t);
              } else {
                strftime(time_text, sizeof(time_text), "%I:%M:%S", t);
              }
              graphics_draw_round_rect(ctx, GRect(slot[i].x-26,slot[i].y-7, 52, 15), 3);
              graphics_draw_text(ctx, time_text, fonts_get_system_font(FONT_KEY_GOTHIC_14), GRect(slot[i].x-25,slot[i].y-9,50,20), GTextOverflowModeWordWrap, GTextAlignmentCenter, NULL);
            }
          }
          break;
        case 8:
            gpath_destroy(charge_icon_path);
            graphics_context_set_fill_color(ctx, accent_color);
            charge_icon_path = gpath_create(&CHARGE_ICON_POINTS);
            gpath_move_to(charge_icon_path, GPoint(slot[i].x-1,slot[i].y));
            graphics_context_set_stroke_color(ctx, accent_color);
            graphics_draw_round_rect(ctx, GRect(slot[i].x-11,slot[i].y-5,20,10), 2);
            if(charging) {
              graphics_fill_rect(ctx, GRect(slot[i].x-11,slot[i].y-5,20,10), 1, GCornersAll);
              graphics_context_set_stroke_color(ctx, face_color);
              gpath_draw_outline(ctx, charge_icon_path);
              graphics_context_set_stroke_color(ctx, accent_color);
            } else {
              switch(battery) {
                case 100:
                  graphics_fill_rect(ctx, GRect(slot[i].x-11,slot[i].y-5,20,10), 1, GCornersAll);
                  break;
                case 90:
                  graphics_fill_rect(ctx, GRect(slot[i].x-11,slot[i].y-5,18,10), 1, GCornersAll);
                  break;
                case 80:
                  graphics_fill_rect(ctx, GRect(slot[i].x-11,slot[i].y-5,16,10), 1, GCornersAll);
                  break;
                case 70:
                  graphics_fill_rect(ctx, GRect(slot[i].x-11,slot[i].y-5,14,10), 1, GCornersAll);
                  break;
                case 60:
                  graphics_fill_rect(ctx, GRect(slot[i].x-11,slot[i].y-5,12,10), 1, GCornersAll);
                  break;
                case 50:
                  graphics_fill_rect(ctx, GRect(slot[i].x-11,slot[i].y-5,10,10), 1, GCornersAll);
                  break;
                case 40:
                  graphics_fill_rect(ctx, GRect(slot[i].x-11,slot[i].y-5,8,10), 1, GCornersAll);
                  break;
                case 30:
                  graphics_fill_rect(ctx, GRect(slot[i].x-11,slot[i].y-5,6,10), 1, GCornersAll);
                  break;
                case 20:
                  graphics_fill_rect(ctx, GRect(slot[i].x-11,slot[i].y-5,4,10), 1, GCornersAll);
                  break;
                case 10:
                  graphics_fill_rect(ctx, GRect(slot[i].x-11,slot[i].y-5,2,10), 1, GCornersAll);
                  break;
                case 0:
                  graphics_fill_rect(ctx, GRect(slot[i].x-11,slot[i].y-5,0,10), 1, GCornersAll);
                  break;
              }
            }
          
          graphics_fill_rect(ctx, GRect(slot[i].x+9,slot[i].y-3,2,6), 2, GCornersRight);
          circle = false;
          segments = 0;
          bold = 0;
          thin = 0;
          break;
    }
      graphics_context_set_stroke_color(ctx, accent_color);
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
      graphics_context_set_stroke_color(ctx, bold_tick_color);
    #else
      graphics_context_set_fill_color(ctx, GColorBlack);
    #endif
    if(bold != 0) {
      if(slotid[i] == 3) {
        /*#ifdef PBL_COLOR
            gpath_destroy(complication_bold_path);
            complication_bold_path = gpath_create(&COMPLICATION_BOLD_POINTS);
            gpath_move_to(complication_bold_path, slot[i]);
        #endif*/
        for(int j = 0; j < 30; j+=5) {
          int32_t tick_angle = TRIG_MAX_ANGLE * j / 31;
          #ifdef PBL_COLOR
            GPoint tick = {
              .x = (int16_t)(sin_lookup(tick_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].x,
              .y = (int16_t)(-cos_lookup(tick_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].y,
            };
            graphics_draw_line(ctx, tick, slot[i]);
          #else
            //gpath_rotate_to(complication_bold_path, tick_angle);
            //gpath_draw_filled(ctx, complication_bold_path);
          #endif
        }
      } else {
        #ifdef PBL_COLOR
            //gpath_destroy(complication_bold_path);
            //complication_bold_path = gpath_create(&COMPLICATION_BOLD_POINTS);
            //gpath_move_to(complication_bold_path, slot[i]);
        #endif
        for(int j = 0; j < segments; j+=(segments/bold)) {
          int32_t tick_angle = TRIG_MAX_ANGLE * j / segments;
          #ifdef PBL_COLOR
            GPoint tick = {
              .x = (int16_t)(sin_lookup(tick_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].x,
              .y = (int16_t)(-cos_lookup(tick_angle) * (int32_t)15 / TRIG_MAX_RATIO) + slot[i].y,
            };
            graphics_draw_line(ctx, tick, slot[i]);
          #else
            //gpath_rotate_to(complication_bold_path, tick_angle);
            //gpath_draw_filled(ctx, complication_bold_path);
          #endif
        }
      }
    }
    #ifdef PBL_COLOR
      graphics_context_set_stroke_width(ctx, 1);
    #endif
    graphics_context_set_fill_color(ctx, face_color);
      if(circle) {
        graphics_fill_circle(ctx, slot[i], 12);
      }
    }
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
  
  graphics_context_set_stroke_color(ctx, hand_color);
  
  #ifdef PBL_COLOR
    graphics_context_set_stroke_width(ctx, 2);
  #endif
  GPoint slot[4] = {GPoint(center.x, center.y-30), GPoint(center.x+30, center.y), GPoint(center.x,center.y+30), GPoint(center.x-30,center.y)};
  int32_t second_angle = TRIG_MAX_ANGLE * t->tm_sec / 60;
  for(int i = 0; i < 4; i++) {
    if(i == 0) {
      if(show_logo) {
        
      }
    }
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
    if(slotid[i] != 0 && slotid[i] != 5 && slotid[i] != 6 && slotid[i] != 7 && slotid[i] != 8 && slotid[i] != 9 && slotid[i] != 10 && slotid[i] != 11) {
      graphics_context_set_fill_color(ctx, hand_color);
      graphics_fill_circle(ctx, slot[i], 3);
      graphics_context_set_fill_color(ctx, face_color);
      graphics_fill_circle(ctx, slot[i], 1);
    }
  }
}
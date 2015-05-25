#pragma once

void background_update(Layer *layer, GContext *ctx);
void hands_update(Layer *layer, GContext *ctx);
void complications_background_update(Layer *layer, GContext *ctx);
void complications_hands_update(Layer *layer, GContext *ctx);

int slotid[4];
int mem_slotid[4];
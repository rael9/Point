#pragma once

#include "pebble.h"

#define NUM_CLOCK_TICKS 11

static const GPathInfo MINUTE_HAND_POINTS = {
  4, (GPoint []) {
        { -4, 20 },
        { 4, 20 },
        { 2, -70 },
        { -2, -70 },
    }
};

static const GPathInfo HOUR_HAND_POINTS = {
  4, (GPoint []){
        { -5, 20 },
        { 5, 20 },
        { 3, -50 },
        { -3, -50 }
    }
};

static const GPathInfo SECOND_HAND_POINTS = {
  7, (GPoint []){
        { -2, 20 },
        { 2, 20 },
        { 2, -70 },
        { 8, -70 },
        { 0, -80 },
        { -8, -70 },
        { -2, -70 }
    }
};

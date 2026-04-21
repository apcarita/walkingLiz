#pragma once

// Servo IDs
#define FL_HIP   0
#define FL_KNEE  1
#define FR_HIP   2
#define FR_KNEE  3
#define BL_HIP   4
#define BL_KNEE  5
#define BR_HIP   6
#define BR_KNEE  7

// Pins
const int SERVO_PINS[8] = {2, 3, 4, 5, 6, 7, 8, 9};

// Safe limits — calibrated, do not exceed
//                       min   max
const int S_MIN[8] = {    16,   15,   80,    0,    0,   80,   81,   75 };
const int S_MAX[8] = {    96,  145,  160,  180,   42,  180,  161,  120 };

// Neutral (standing) positions
// Hip neutrals are midpoint of safe range (estimate — tune for standing balance)
// Knee neutrals are from calibration
//                         FL_HIP  FL_KNEE  FR_HIP  FR_KNEE  BL_HIP  BL_KNEE  BR_HIP  BR_KNEE
const int S_NEUTRAL[8] = {    56,     90,    120,    151,     21,    115,     121,     90 };

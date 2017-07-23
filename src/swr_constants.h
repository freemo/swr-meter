#ifndef _SWR_CONSTANTS_H_
#define _SWR_CONSTANTS_H_

#define POWER_FWD_PIN A15
#define POWER_REFL_PIN A14
#define DIFFERENTIAL_VREF_PIN A13
#define DIFFERENTIAL_PHASE_PIN A12
#define DIFFERENTIAL_MAGNITUDE_PIN A11
#define SHIFTED_DIFFERENTIAL_VREF_PIN A10
#define SHIFTED_DIFFERENTIAL_PHASE_PIN A9
#define SHIFTED_DIFFERENTIAL_MAGNITUDE_PIN A8

#define DOWN_BUTTON_PIN 22
#define UP_BUTTON_PIN 24
#define HEARTBEAT_LED_PIN 35
#define STATUS_LED_PIN 51

#define SCREEN_HEIGHT 64
#define SCREEN_WIDTH 128
#define PERCENT_BAR_TITLE_WIDTH 20
#define PERCENT_BAR_WIDTH (SCREEN_WIDTH-PERCENT_BAR_TITLE_WIDTH)
#define CHARACTER_WIDTH 6
#define SCREEN_ROW_1_Y 0
#define SCREEN_ROW_2_Y 16
#define SCREEN_ROW_3_Y 33
#define SCREEN_ROW_4_Y 49
#define SCREEN_ROW_5_Y 57

#define SERIAL_BAUDE_RATE 9600

#define DISPLAY_REFRESH_RATE_MS 25

#define CHARACTERISTIC_IMPEDANCE 50.0

#define CALIBRATION_SAMPLES 10000
#define CALIBRATION_THRESHOLD 10
#define MAX_CALIBRATION_POWER_POINTS_DUMMY 8
#define MAX_CALIBRATION_POWER_POINTS_OPEN 4

#define HEARTBEAT_RATE_LONG_MS 1000
#define HEARTBEAT_RATE_SHORT_MS 100
#define LED_STATUS_RATE_SLOW_MS 250
#define LED_STATUS_RATE_FAST_MS 100
#define LED_STATUS_RATE_VERY_FAST_MS 25

#endif /* _SWR_CONSTANTS_H_ */

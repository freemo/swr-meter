#ifndef _SWR_CONSTANTS_H_
#define _SWR_CONSTANTS_H_

#define POWER_FWD_PIN A15
#define POWER_RVR_PIN A14
#define COMPLEX_VREF_PIN A13
#define COMPLEX_PHASE_PIN A12
#define COMPLEX_MAGNITUDE_PIN A13

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

#define LOW_POWER 5.0
#define HIGH_POWER 200.0

#define MAX_BANDS_COUNT 64
#define MAX_BAND_NAME_LENGTH 8
#define MAX_CALIBRATION_POWER_POINTS 32

#endif /* _SWR_CONSTANTS_H_ */

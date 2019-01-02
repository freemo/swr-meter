#include "main.h"

#include "swr_display.h"
#include "swr_power.h"
#include "swr_heartbeat.h"
#include "swr_calibrate.h"
#include "swr_constants.h"
#include "swr_strings.h"
#include "swr_status_led.h"
#include "swr_commandline.h"
#include "swr_eeprom.h"
#include "swr_sensors.h"
#include <Adafruit_FT6206.h>

enum TopScreen {
  TOP_POWER,
  TOP_REFLECTION
};

enum BottomScreen {
  BOTTOM_POWER,
  BOTTOM_REFLECTION,
  BOTTOM_LOAD
};

boolean error = false;

boolean calibrating = false;
boolean calibratingPause = false;
boolean calibratingDummy = true;

TopScreen currentTopScreen = TOP_POWER;
BottomScreen currentBottomScreen = BOTTOM_POWER;

SensorData sensorData;

float calibratingPowerPoint = -1.0;

Adafruit_FT6206 touch = Adafruit_FT6206();

void setup()   {
  Serial.begin(SERIAL_BAUDE_RATE);//SERIAL_BAUDE_RATE);
  while (!Serial);     // used for leonardo debugging

  Serial.println("Begining system initialization...");
  heartbeatSetup();
  Serial.println("  Heartbeat initialized");
  // statusLedSetup();
  // Serial.println("  Status LED initialized");
  displaySetup();
  Serial.println("  Display initialized");
  //eepromSetup();
  // Serial.println("  EEPROM initialized");
  commandlineSetup();
  Serial.println("  Serial Commandline initialized");
  Serial.println("System initialized!");

  if (!touch.begin())
    Serial.println("  Unable to start touchscreen.");
  else
    Serial.println("Touchscreen started.");

  // pinMode(DOWN_BUTTON_PIN, INPUT);
  // pinMode(UP_BUTTON_PIN, INPUT);

  // Serial.println("Checking EEPROM integrity...");
  // //make sure eeprom isn't corrupt
  // if( checkEepromCrc() == false ) {
  //   Serial.println("EEPROM failed CRC check!");
  //   error = true;
  //   uint32_t actualCrc = eepromCrc32Actual();
  //   uint32_t storedCrc = eepromCrc32Stored();
  //   char errorMsgLine3[11] = "0x";
  //   uint32toa(actualCrc, errorMsgLine3 + 2, 16);
  //   char errorMsgLine4[11] = "0x";
  //   uint32toa(storedCrc, errorMsgLine4 + 2, 16);
  //
  //   prepareRender();
  //   renderError(CORRUPT_EEPROM, CRC_CHECK_FAILED, errorMsgLine3, errorMsgLine4);
  //   finishRender();
  // }
  // else {
  //   Serial.println("EEPROM passed all checks!");
  //
  //   if( calibrateOnBoot() == true )
  //   {
  //     Serial.println("Configured to calibrate on boot...");
  //     calibrating = true;
  //     calibratingPause = false;
  //     calibratingDummy = true;
  //     bumpCalibratingPowerPoint();
  //
  //     prepareRender();
  //     renderCalibration(calibratingPowerPoint, calibratingDummy);
  //     finishRender();
  //   }
  // }
  // sensorData.differentialMagnitudeDb = 0.8;
  // sensorData.differentialPhaseDeg = 0.7;
}

void loop() {
  unsigned long time = millis();
  static unsigned long refreshDisplayTime = 0;

  if(touch.touched())
  {
    // Retrieve a point
    //TS_Point p = touch.getPoint();
    touch.getPoint();
    // // Scale using the calibration #'s
    // // and rotate coordinate system
    // p.x = map(p.x, TOUCH_MIN_Y, TOUCH_MAX_Y, 0, SCREEN_HEIGHT);
    // p.y = map(p.y, TOUCH_MIN_X, TOUCH_MAX_X, 0, SCREEN_WIDTH);
    // int y = SCREEN_HEIGHT - p.x;
    // int x = p.y;
    //
    // if( y > SCREEN_ROW_GRFX_Y && y < SCREEN_ROW_GRFX_Y_END ) {
    switchSystem();
    //}
  }

  heartbeatUpdate();
  // statusLedUpdate();
  commandlineUpdate();

  if( error )
     return;

  //updateDownButton();
  //updateUpButton();

  if( (long)(time - refreshDisplayTime) >= 0 && !calibrating) {
    refreshDisplayTime = time + DISPLAY_REFRESH_RATE_MS;

    if(demoMode()) {
      updateComplexDemo(&(sensorData.differentialMagnitudeDb), &(sensorData.calculatedPhaseDeg));
      updatePowerDemo(&(sensorData.fwdPower), &(sensorData.reflPower));
      sensorData.swr = powerToSwr(sensorData.fwdPower, sensorData.reflPower);
    }
    else {
      sensorData = readSensors(sensorData);
    }

    commandlineUpdate();

    // if( sensorData.fwdPower >= TRANSMIT_THREASHOLD_POWER ) {
    //   if( sensorData.swr < 1.5 )
    //     setLedStatus(SLOW);
    //   else if( sensorData.swr >= 1.5 && sensorData.swr < 2.0 )
    //     setLedStatus(FAST);
    //   else if( sensorData.swr >= 2.0 && sensorData.swr < 3.0 )
    //     setLedStatus(VERY_FAST);
    //   else
    //     setLedStatus(ON);
    // }
    // else
    //   setLedStatus(OFF);

    prepareRender();
    renderSwr(sensorData.swr);
    commandlineUpdate();

    renderPowerBars(sensorData.fwdPower, sensorData.reflPower);
    commandlineUpdate();

    //renderReflectionBars(sensorData.differentialMagnitudeDb, sensorData.calculatedPhaseDeg);

    renderPowerText(sensorData.fwdPower, sensorData.reflPower);
    commandlineUpdate();

    renderLoadText(sensorData.differentialMagnitudeDb, sensorData.calculatedPhaseDeg);
    commandlineUpdate();

    //arbitrary measure of when not considered transmitting
    static boolean transmitting = false;
    if(sensorData.fwdPower < TRANSMIT_THREASHOLD_POWER && transmitting) {
      transmitting = false;
    } else if(sensorData.fwdPower > (TRANSMIT_THREASHOLD_POWER*2) && !transmitting) {
      transmitting = true;
      clearSmithChart();
    }

    if(sensorData.fwdPower > TRANSMIT_THREASHOLD_POWER)
      renderSmithChart(sensorData.differentialMagnitudeDb, sensorData.calculatedPhaseDeg);
    finishRender();
    commandlineUpdate();
  }

  // if(calibrating) {
  //   if( calibratingPause ) {
  //       if(waitForStop()) {
  //         if( bumpCalibratingPowerPoint() ) {
  //           if( calibratingDummy ) {
  //             calibratingDummy = false;
  //           }
  //           else {
  //             calibrating = false;
  //             calibratingDummy = true;
  //             calibratingPause = false;
  //             deactivateCalibrateOnBoot();
  //             return;
  //           }
  //         }
  //         calibratingPause = false;
  //         prepareRender();
  //         renderCalibration(calibratingPowerPoint, calibratingDummy);
  //         finishRender();
  //       }
  //   }
  //   else {
  //     if( runCalibration() ) {
  //       calibratingPause = true;
  //       CalibrationAverages result = getCalibration();
  //       CalibrationData currentCalibration = calibrationData(calibratingPowerPoint, calibratingDummy);
  //       currentCalibration.fwd = result.adcFwd;
  //       currentCalibration.refl = result.adcRvr;
  //       currentCalibration.vref = result.adcVref;
  //       currentCalibration.phase = result.adcPhase;
  //       currentCalibration.magnitude = result.adcMagnitude;
  //       currentCalibration.phaseShifted = result.adcPhaseShifted;
  //       currentCalibration.vrefShifted = result.adcVrefShifted;
  //       setCalibrationData(calibratingPowerPoint, calibratingDummy, currentCalibration);
  //       prepareRender();
  //       renderStopTransmitting();
  //       finishRender();
  //     }
  //   }
  // }
}

void updateDownButton() {
  static boolean downButtonLowLast = false;
  int buttonState = digitalRead(DOWN_BUTTON_PIN);

  if( downButtonLowLast == false && buttonState == LOW ) {
    if( currentBottomScreen == BOTTOM_POWER )
      currentBottomScreen = BOTTOM_REFLECTION;
    else if( currentBottomScreen == BOTTOM_REFLECTION )
      currentBottomScreen = BOTTOM_LOAD;
    else if( currentBottomScreen == BOTTOM_LOAD )
      currentBottomScreen = BOTTOM_POWER;

    downButtonLowLast = true;
  }

  if(downButtonLowLast && buttonState == HIGH)
    downButtonLowLast = false;
}

void updateUpButton() {
  static boolean upButtonLowLast = false;
  int buttonState = digitalRead(UP_BUTTON_PIN);

  if( upButtonLowLast == false && buttonState == LOW ) {
    if( currentTopScreen == TOP_POWER )
      currentTopScreen = TOP_REFLECTION;
    else if( currentTopScreen == TOP_REFLECTION )
      currentTopScreen = TOP_POWER;

    upButtonLowLast = true;
  }

  if(upButtonLowLast && buttonState == HIGH)
    upButtonLowLast = false;
}


boolean bumpCalibratingPowerPoint() {
  etl::iset<float, std::less<float>>::const_iterator itr;
  etl::iset<float, std::less<float>>::const_iterator itrEnd;

  etl::set<float, MAX_CALIBRATION_POWER_POINTS_DUMMY> powerPointDataDummy = calibrationPowerPointsDummy();
  etl::set<float, MAX_CALIBRATION_POWER_POINTS_OPEN> powerPointDataOpen = calibrationPowerPointsOpen();
  if(calibratingDummy) {
    itr = powerPointDataDummy.begin();
    itrEnd = powerPointDataDummy.end();
  }
  else {
    itr = powerPointDataOpen.begin();
    itrEnd = powerPointDataOpen.end();
  }

  // Iterate through the list.
  boolean isNext = false;
  while (itr != itrEnd)
  {
    float currentPowerPoint = *itr++;
    if( calibratingPowerPoint < 0.0 || isNext ) {
      calibratingPowerPoint = currentPowerPoint;
      return false;
    }
    else if(calibratingPowerPoint == currentPowerPoint)
      isNext = true;
  }

  if(isNext) {
    if( calibratingDummy )
      calibratingPowerPoint = *(calibrationPowerPointsOpen().begin());
    else
      calibratingPowerPoint = *(calibrationPowerPointsDummy().begin());
    return true;
  }

  return false;
}

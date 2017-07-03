#include <math.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

#define SCREEN_HEIGHT SSD1306_LCDHEIGHT
#define SCREEN_WIDTH SSD1306_LCDWIDTH
#define PERCENT_BAR_TITLE_WIDTH 20
#define PERCENT_BAR_WIDTH (SCREEN_WIDTH-PERCENT_BAR_TITLE_WIDTH)
#define CHARACTER_WIDTH 6
#define SCREEN_ROW_1_Y 0
#define SCREEN_ROW_2_Y 16
#define SCREEN_ROW_3_Y 33
#define SCREEN_ROW_4_Y 49
#define SCREEN_ROW_5_Y 57

float power_fwd_test = 100.0;
boolean power_fwd_increasing = true;
float power_rvr_test = 100.0;
boolean power_rvr_increasing = true;

void setup()   {                
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  // init done
}

void loop() {
  render(power_fwd_test, power_rvr_test);
  delay(25);
  adjustTestValues();
}

void adjustTestValues() {
  if( power_fwd_test < 10.0 )
    if( power_fwd_increasing )
      power_fwd_test += 0.1;
    else
      power_fwd_test -= 0.1;
  else if(power_fwd_test >= 100)
    if( power_fwd_increasing )
      power_fwd_test += 10.0;
    else
      power_fwd_test -= 10.0;
  else
    if( power_fwd_increasing )
      power_fwd_test += 1.0;
    else
      power_fwd_test -= 1.0;
      
  if(power_fwd_test > 1000.0) {
    power_fwd_test = 1000.0;
    power_fwd_increasing = false;
  }
  else if(power_fwd_test < 0.0 ) {
    power_fwd_test = 0.0;
    power_fwd_increasing = true;
  }

  if( power_rvr_test < 10.0 )
    if( power_rvr_increasing )
      power_rvr_test += 0.2;
    else
      power_rvr_test -= 0.2;
  else if(power_rvr_test >= 100)
    if( power_rvr_increasing )
      power_rvr_test += 20.0;
    else
      power_rvr_test -= 20.0;
  else
    if( power_rvr_increasing )
      power_rvr_test += 2.0;
    else
      power_rvr_test -= 2.0;
  if(power_rvr_test > power_fwd_test) {
    power_rvr_test = power_fwd_test;
    power_rvr_increasing = false;
  }
  else if( power_rvr_test < 0.0 ) {
    power_rvr_test = 0.0;
    power_rvr_increasing = true;
  }
}

uint8_t percentBar(uint8_t y_offset, float percent) {
  display.fillRect(PERCENT_BAR_TITLE_WIDTH, y_offset, PERCENT_BAR_WIDTH * percent, 15, 1);
  return PERCENT_BAR_TITLE_WIDTH + (PERCENT_BAR_WIDTH * percent);
}

float logBased(float value, float base) {
  return log10(value)/log10(base);
}

float scaleToPercent(float value, float middle, float scale) {
  if( value == 0 )
    return 0.0;
  else if(isinf(value))
    return 1.0;
  float p = logBased(value / middle, scale);
  float pabs = fabs(p);
  float z = (pow(2.0, pabs) - 1.0) / pow(2.0, pabs);
  if( p < 0 )
    z = -1.0 * z;
  return (z + 1.0) / 2.0;
}

float scaleToPercent(float value, float value_min, float value_mid, float scale) {
  if(isinf(value))
    return 1.0;
  return scaleToPercent(value - value_min, value_mid - value_min, scale);
}

String makeValueLabel(float value, String units) {
  String label = String(value);
  if( isinf(value) )
    return "***";
  int8_t decimalIndex = label.indexOf(".");
  if( decimalIndex > 1 )
    label = label.substring(0,decimalIndex);
  else
    label = label.substring(0, 3);
  label.concat(units);
  return label;
}

float swrFromPower(float power_fwd, float power_rvr) {
  if( power_rvr <= 0 )
    return 1.0;
  float pwrs = sqrt(power_rvr / power_fwd);
  return (1.0 + pwrs)/(1.0 - pwrs);
}

void renderCompleteBar(int8_t y_offset, String label, float value, String units, float value_min, float value_mid, float scale) {
  display.setCursor(0, y_offset + 4);
  display.println(label);
  float barPercent = scaleToPercent(value, value_min, value_mid, scale);
  uint8_t barEnd = percentBar(y_offset, barPercent);
  String valueLabel = makeValueLabel(value, units);
  uint8_t valueLabelWidth = valueLabel.length() * CHARACTER_WIDTH + 2;
  if( barEnd < PERCENT_BAR_TITLE_WIDTH + valueLabelWidth) { 
    display.setCursor(barEnd + 2, y_offset + 4);
    display.println(valueLabel);
  }
  else {
    display.setTextColor(BLACK);
    display.setCursor(barEnd - valueLabelWidth, y_offset + 4);
    display.println(valueLabel);
    display.setTextColor(WHITE);
  }
}

void render(float power_fwd, float power_rvr) {
  display.clearDisplay();

  renderCompleteBar(SCREEN_ROW_1_Y, "SWR", swrFromPower(power_fwd, power_rvr), "", 1.0, 2.0, 2.0);
  renderCompleteBar(SCREEN_ROW_2_Y, "Fwd", power_fwd, "w", 0.0, 100.0, 2.0);
  renderCompleteBar(SCREEN_ROW_3_Y, "Rvr", power_rvr, "w", 0.0, 100.0, 2.0);

  display.setTextColor(WHITE);
  display.setCursor(0, SCREEN_ROW_4_Y);
  display.println("Refl 3.6 Phase 132");
  display.drawCircle(110, SCREEN_ROW_4_Y, 1, WHITE);

  display.setCursor(32, SCREEN_ROW_5_Y);
  display.println("9i + 34");
  
  display.display();
}


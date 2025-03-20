#include "Adafruit_NeoPixel.h"
#include "TM1637Display.h"
#include "WiFiManager.h"
#include "NTPClient.h"

#define NEOPIXEL_PIN D6
#define SEVEN_SEG_CLK D5
#define SEVEN_SEG1_DIO D4
#define SEVEN_SEG2_DIO D3
#define SEVEN_SEG3_DIO D2

#define AM_LED_PIN D0
#define PM_LED_PIN D1

#define SWITCH_INPUT_PIN A0

#define NUMPIXELS_PER_STRIP 12
#define NUM_STRIPS 3
#define NUMPIXELS (NUMPIXELS_PER_STRIP*NUM_STRIPS)

int ledColorMode = -1;

/* UTC offset to US/Eastern time zone */
const long utcOffsetInSeconds = -10800;

const int Display_backlight = 2;

const char *ssid = "caribbean-IOT";
const char *password = "^rY$OdOU9tA0!uv&mb*SiKd5b";

//======================================================================


Adafruit_NeoPixel pixels(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);
TM1637Display red1(SEVEN_SEG_CLK, SEVEN_SEG1_DIO);
TM1637Display red2(SEVEN_SEG_CLK, SEVEN_SEG2_DIO);
TM1637Display red3(SEVEN_SEG_CLK, SEVEN_SEG3_DIO);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void setup() {
  Serial.begin(9600);
  
  pinMode(NEOPIXEL_PIN, OUTPUT);
  pinMode(SEVEN_SEG_CLK, OUTPUT);
  pinMode(SEVEN_SEG1_DIO, OUTPUT);
  pinMode(SEVEN_SEG2_DIO, OUTPUT);
  pinMode(SEVEN_SEG3_DIO, OUTPUT);
  pinMode(AM_LED_PIN, OUTPUT);
  pinMode(PM_LED_PIN, OUTPUT);

  pinMode(SWITCH_INPUT_PIN, INPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  timeClient.begin();

  red1.setBrightness(Display_backlight);
  red2.setBrightness(Display_backlight);
  red3.setBrightness(Display_backlight);
  pixels.setBrightness(50);
}

void loop() {
  bool colorModeChanged = false;
  
  timeClient.update();
#if 0
  Serial.print("Time: ");
  Serial.println(timeClient.getFormattedTime());
#endif

  time_t epochTime = timeClient.getEpochTime();

  struct tm *ptm = localtime(&epochTime);
  int year = ptm->tm_year + 1900;
  int month = ptm->tm_mon + 1;
  int day = ptm->tm_mday;
  int hours = ptm->tm_hour;
  int minutes = ptm->tm_min;
  int seconds = ptm->tm_sec;
#if 0
  Serial.print(year); Serial.print("-"); Serial.print(month); Serial.print("-"); Serial.print(day); Serial.print(", ");
  Serial.print(hours); Serial.print(":"); Serial.print(minutes); Serial.print(":"); Serial.println(seconds);
#endif

  red1.showNumberDecEx(month, 0b01000000, true, 2, 0);
  red1.showNumberDecEx(day, 0b01000000, true, 2, 2);
  red2.showNumberDecEx(year, 0b00000000, true);
  red3.showNumberDecEx(hours > 12 ? hours - 12 : hours, 0b01000000, true, 2, 0);
  red3.showNumberDecEx(minutes, 0b01000000, true, 2, 2);

  if ((month * 30 + day) >= 121 && (month * 30 + day) < 331) {
    // DST adjustment - Summer
    timeClient.setTimeOffset(utcOffsetInSeconds);
  } else {                                           
    // DST adjustment - Winter
    timeClient.setTimeOffset(utcOffsetInSeconds - 3600);
  }

  if (timeClient.getHours() >= 12) {
    analogWrite(AM_LED_PIN, 0);
    analogWrite(PM_LED_PIN, 10);
  } else {
    analogWrite(AM_LED_PIN, 10);
    analogWrite(PM_LED_PIN, 0);
  }

  pixels.clear();  // Set all pixel colors to 'off'

  if ((analogRead(SWITCH_INPUT_PIN) > 100) || (ledColorMode < 0)) {
    ledColorMode = ledColorMode + 1;
    if (ledColorMode > 3) {
      ledColorMode = 0;
    }
    colorModeChanged = true;
  }

#if 0
  Serial.print("Var=");
  Serial.print(var);
  Serial.print(", Digital=");
  Serial.println(analogRead(SWITCH_INPUT_PIN));
#endif

  if (colorModeChanged) {
    switch (ledColorMode) {
      case 0:
        for (int i = 0; i < NUMPIXELS_PER_STRIP; i++) {
          pixels.setPixelColor(i, pixels.Color(255, 0, 0));
        }
        for (int i = NUMPIXELS_PER_STRIP; i < 2*NUMPIXELS_PER_STRIP; i++) {
          pixels.setPixelColor(i, pixels.Color(160, 160, 0));
        }
        for (int i = 2*NUMPIXELS_PER_STRIP; i < NUMPIXELS; i++) {
          pixels.setPixelColor(i, pixels.Color(255, 0, 0));
        }
        pixels.show();
        break;
  
      case 1:
        for (int i = 0; i < NUMPIXELS_PER_STRIP; i++) {
          pixels.setPixelColor(i, pixels.Color(0, 0, 255));
        }
        for (int i = NUMPIXELS_PER_STRIP; i < 2*NUMPIXELS_PER_STRIP; i++) {
          pixels.setPixelColor(i, pixels.Color(200, 250, 255));
        }
        for (int i = 2*NUMPIXELS_PER_STRIP; i < NUMPIXELS; i++) {
          pixels.setPixelColor(i, pixels.Color(0, 0, 255));
        }
        pixels.show();
        break;
  
      case 2:
        pixels.clear();
        for (int i = 0; i < NUMPIXELS_PER_STRIP; i++) {
          pixels.setPixelColor(i, pixels.Color(255, 0, 10));
        }
        for (int i = NUMPIXELS_PER_STRIP; i < 2*NUMPIXELS_PER_STRIP; i++) {
          pixels.setPixelColor(i, pixels.Color(0, 10, 255));
        }
        for (int i = 2*NUMPIXELS_PER_STRIP; i < NUMPIXELS; i++) {
          pixels.setPixelColor(i, pixels.Color(255, 0, 10));
        }
        pixels.show();
        break;
  
      case 3:
        pixels.clear();
        for (int i = 0; i < NUMPIXELS; i++) {
          pixels.setPixelColor(i, pixels.Color(0, 0, 0));
        }
        pixels.show();
        break;
    }
  }
}

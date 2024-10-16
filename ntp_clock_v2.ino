
#include "secrets.h"

#include "bootloader_random.h"
#include <WiFi.h>
#include "MD_Parola.h"
#include "MD_MAX72xx.h"
#include "SPI.h"

#include "Clock_Font2.h"

// Hardware SPI:
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 5

// NTP Setup:
#define NTP_SERVER     "pool.ntp.org"
#define UTC_OFFSET     8
#define UTC_OFFSET_DST 0

// WiFi:
// #define WIFI_SSID
// #define WIFI_PASS
// #define WIFI_HOSTNAME

// Push Button:
#define BTN_PIN 15
bool btnState = false;

// sprites
const uint8_t F_INVADER = 2;
const uint8_t W_INVADER = 10;
const uint8_t PROGMEM invader[F_INVADER * W_INVADER] =  // space invader
{
  0x0e, 0x98, 0x7d, 0x36, 0x3c, 0x3c, 0x36, 0x7d, 0x98, 0x0e,
  0x70, 0x18, 0x7d, 0xb6, 0x3c, 0x3c, 0xb6, 0x7d, 0x18, 0x70,
};

const uint8_t F_SES = 2;
const uint8_t W_SES = 8;
const uint8_t PROGMEM ses[F_SES * W_SES] =  // ses
{
  15, 15, 15, 15, 240, 240, 240, 240,
  240, 240, 240, 240, 15, 15, 15, 15,
};

// Create a new instance of the MD_MAX72XX class:
MD_Parola myDisplay = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// vars for time tracking
int8_t last_second;

void setup() {
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP);

  Serial.println();
  Serial.println("******************************************************");

  // init display
  myDisplay.begin(2);
  myDisplay.displayClear();

  myDisplay.setZone(1, 0, MAX_DEVICES-1);

  myDisplay.setCharSpacing(0);
  myDisplay.setIntensity(0);  

  Serial.println("Display intiliased");

  // pick random sprite to use
  bootloader_random_enable();
  uint32_t randomNumber;
  delay(50);
  randomNumber = esp_random();
  Serial.printf("random=%d\n",randomNumber);
  if((randomNumber & 0x01) == 0){
    myDisplay.setSpriteData(1, invader, W_INVADER, F_INVADER, invader, W_INVADER, F_INVADER);
  } else {
    myDisplay.setSpriteData(1, ses, W_SES, F_SES, ses, W_SES, F_SES);
  }
  myDisplay.displayZoneText(1, "RKSES", PA_CENTER, 50, 250, PA_SPRITE, PA_SPRITE);

  // init wifi
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  char hostname[100];
  strcpy(hostname, WIFI_HOSTNAME);
  strcat(hostname, WiFi.macAddress().c_str());
  WiFi.setHostname(hostname);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    if (myDisplay.displayAnimate()) {
      myDisplay.displayReset();
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Mac Address: ");
  Serial.println(hostname);

  myDisplay.setZone(0, 1, 3);
  myDisplay.setZone(1, 0, 0);
  myDisplay.setIntensity(0);

  myDisplay.setFont(0, SmallDigits);
  myDisplay.setFont(1, SmallerDigits);

  // init ntp
  configTime((UTC_OFFSET*60*60), UTC_OFFSET_DST, NTP_SERVER);

}

void loop() {
  struct tm timeinfo;
  
  if (!getLocalTime(&timeinfo)) {
    return;
  }

  if(last_second != timeinfo.tm_sec){
    char DaysWeek[4];
    char Day[3];
    char Month[4];
    char Time[6];
    char Sec[3];
    char DayMonth[7];

    strftime(DaysWeek , 4, "%a", &timeinfo);
    strftime(Day, 3, "%d", &timeinfo);
    strftime(Month, 4, "%b", &timeinfo);
    strftime(Time, 6, "%R", &timeinfo);
    strftime(Sec, 3, "%S", &timeinfo);
    strftime(DayMonth, 7, "%d %m", &timeinfo);

    if(digitalRead(BTN_PIN) == LOW){
      if(!btnState){
        myDisplay.setZone(0, 2, 3);
        myDisplay.setZone(1, 0, 1);
        btnState = true;
      }
      myDisplay.displayZoneText(0, DaysWeek, PA_LEFT, myDisplay.getSpeed(), myDisplay.getPause(), PA_NO_EFFECT);
      myDisplay.displayZoneText(1, DayMonth, PA_LEFT, myDisplay.getSpeed(), myDisplay.getPause(), PA_NO_EFFECT);
    } else {
      if(btnState){
        myDisplay.setZone(0, 1, 3);
        myDisplay.setZone(1, 0, 0);
        btnState = false;
      }
      myDisplay.displayZoneText(0, Time,  PA_LEFT, myDisplay.getSpeed(), myDisplay.getPause(), PA_NO_EFFECT);
      myDisplay.displayZoneText(1, Sec, PA_LEFT, myDisplay.getSpeed(), myDisplay.getPause(), PA_NO_EFFECT);
    }

    myDisplay.displayAnimate();

    last_second = timeinfo.tm_sec;
  }

  delay(50);
}
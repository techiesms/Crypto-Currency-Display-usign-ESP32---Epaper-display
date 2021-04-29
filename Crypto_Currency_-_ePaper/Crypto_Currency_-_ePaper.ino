  /*\
 * 
 * This is the code for the project
 * 
 * "Crypto Currency Monitor using ESP32 & E-Paper Display"
 * 
 * 
 * To watch the full tutorial video, just head on to our YouTube channel
 * 
 * https://www.youtube.com/techiesms
 * 
 * 
 *         techiesms
 * explore | learn | share 
 * 
 * 
 */
#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM !!!"
#endif


#include <WiFi.h>
#include <HTTPClient.h>
#include <WifiClientSecure.h>
#include <ArduinoJson.h>
#include "cryptos.h"
#include "coingecko-api.h"
#include <Arduino.h>
#include <esp_task_wdt.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "epd_driver.h"
#include "firasans.h"
#include "esp_adc_cal.h"
#include <Wire.h>
#include <SPI.h>
#include <SD.h>


#define BATT_PIN            36
#define SD_MISO             12
#define SD_MOSI             13
#define SD_SCLK             14
#define SD_CS               15

int cursor_x ;
int cursor_y ;

uint8_t *framebuffer;
int vref = 1100;

// ----------------------------
// Configurations - Update these
// ----------------------------

const char *ssid = "SSID";
const char *password = "PASS";


// ----------------------------
// End of area you need to change
// ----------------------------


void setup()
{
  char buf[128];

  Serial.begin(115200);


  connectToWifi();

  // Correct the ADC reference voltage
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
    Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
    vref = adc_chars.vref;
  }

  epd_init();

  framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
  if (!framebuffer) {
    Serial.println("alloc memory failed !!!");
    while (1);
  }
  memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);

  epd_poweron();
  epd_clear();

  epd_poweroff();

  epd_poweron();



}

void loop()
{
  downloadBaseData("inr");
  delay(1000);
  downloadBtcAndEthPrice();
  title();
  for (int i = 0; i < cryptosCount; i++)
  {
    cursor_y = (50 * (i + 3));
    renderCryptoCard(cryptos[i]);
  }
  delay(5000);
}

void title()
{

  cursor_x = 20;
  cursor_y = 50;
  char *sym = "Symbol";
  writeln((GFXfont *)&FiraSans, sym, &cursor_x, &cursor_y, NULL);

  cursor_x = 290;
  cursor_y = 50;
  char *prc = "Price";
  writeln((GFXfont *)&FiraSans, prc, &cursor_x, &cursor_y, NULL);

  cursor_x = 520;
  cursor_y = 50;
  char *da = "Day(%)";
  writeln((GFXfont *)&FiraSans, da, &cursor_x, &cursor_y, NULL);

  cursor_x = 790;
  cursor_y = 50;
  char *we = "Week(%)";
  writeln((GFXfont *)&FiraSans, we, &cursor_x, &cursor_y, NULL);


}

void renderCryptoCard(Crypto crypto)
{



  Serial.print("Crypto Name  - "); Serial.println(crypto.symbol);

  cursor_x = 50;

  char *string1 = &crypto.symbol[0];

  writeln((GFXfont *)&FiraSans, string1, &cursor_x, &cursor_y, NULL);

  cursor_x = 220;

  String Str = (String)(crypto.price.inr);
  char* string2 = &Str[0];

  Serial.print("price usd - "); Serial.println(Str);

  Rect_t area = {
    .x = cursor_x,
    .y = cursor_y-40,
    .width = 320,
    .height = 50,
  };

  epd_clear_area(area);

  writeln((GFXfont *)&FiraSans, string2, &cursor_x, &cursor_y, NULL);

  Serial.print("Day change - "); Serial.println(formatPercentageChange(crypto.dayChange));

  cursor_x = 530;

  Rect_t area1 = {
    .x = cursor_x,
    .y = cursor_y-40,
    .width = 150,
    .height = 50,
  };

  epd_clear_area(area1);
  Str = (String)(crypto.dayChange);
  char* string3 = &Str[0];

  writeln((GFXfont *)&FiraSans, string3, &cursor_x, &cursor_y, NULL);


  Serial.print("Week change - "); Serial.println(formatPercentageChange(crypto.weekChange));

  cursor_x = 800;
  
  Rect_t area2 = {
    .x = cursor_x,
    .y = cursor_y-40,
    .width = 150,
    .height = 50,
  };

  epd_clear_area(area2);
  
  Str = (String)(crypto.weekChange);
  char* string4 = &Str[0];

  writeln((GFXfont *)&FiraSans, string4, &cursor_x, &cursor_y, NULL);

}


void connectToWifi()
{
  WiFi.begin(ssid, password);
  String dots[3] = {".", "..", "..."};
  int numberOfDots = 1;

  //tft.setTextColor(//tft_WHITE, //tft_BLACK);
  while (WiFi.status() != WL_CONNECTED)
  {
    //tft.drawCentreString("Connecting to WiFi " + dots[numberOfDots - 1], 120, 120, 2);
    Serial.println("Connecting to WiFi");
    if (numberOfDots == 3)
    {
      numberOfDots = 0;
    }
    else
    {
      numberOfDots++;
    }

    delay(300);
    //tft.fillScreen(//tft_BLACK);
  }

  Serial.println("Connected!!!_______________");

}



String formatPercentageChange(double change)
{


  double absChange = change;

  if (change < 0)
  {
    absChange = -change;
  }

  if (absChange > 100) {
    return String(absChange, 0) + "%";
  } else if (absChange >= 10) {
    return String(absChange, 1) + "%";
  } else {
    return String(absChange) + "%";
  }
}

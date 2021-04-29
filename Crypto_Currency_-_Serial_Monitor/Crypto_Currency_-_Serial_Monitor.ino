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

#include <WiFi.h>
#include <HTTPClient.h>
#include <WifiClientSecure.h>
#include <ArduinoJson.h>
#include "cryptos.h"
#include "coingecko-api.h"

// ----------------------------
// Configurations - Update these
// ----------------------------

const char *ssid = "SSID";
const char *password = "PASS";
unsigned long secondsForEachCrypto = 5;

void setup()
{
  Serial.begin(115200);

  connectToWifi();

  delay(4000);
}

void loop()
{
  downloadBaseData("inr");
  delay(1000);
  downloadBtcAndEthPrice();
  for (int i = 0; i < cryptosCount; i++)
  {
    renderCryptoCard(cryptos[i]);
    delay(secondsForEachCrypto * 1000);
  }
}

void renderCryptoCard(Crypto crypto)
{

  Serial.print("Crypto Name  - "); Serial.println(crypto.symbol);

  Serial.print("price usd - "); Serial.println(formatCurrency(crypto.price.inr));

  Serial.print("Day change - "); Serial.println(formatPercentageChange(crypto.dayChange));

  Serial.print("Week change - "); Serial.println(formatPercentageChange(crypto.weekChange));
 
  Serial.print("Price in Bitcoin - "); Serial.println(crypto.price.btc);
  
  Serial.print("Price in ETH - "); Serial.println(crypto.price.eth);
 
}



void connectToWifi()
{
  WiFi.begin(ssid, password);
  String dots[3] = {".", "..", "..."};
  int numberOfDots = 1;

  //tft.setTextColor(//tft_WHITE, //tft_BLACK);
  while (WiFi.status() != WL_CONNECTED)
  {
   
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

String formatCurrency(double price)
{
  int digitsAfterDecimal = 3;

  if (price >= 1000)
  {
    digitsAfterDecimal = 0;
  }
  else if (price >= 100)
  {
    digitsAfterDecimal = 1;
  }
  else if (price >= 1)
  {
    digitsAfterDecimal = 2;
  }
  else if (price < 0.001)
  {
    digitsAfterDecimal = 4;
  }

  return String(price, digitsAfterDecimal);
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

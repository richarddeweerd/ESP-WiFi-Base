#include <ESP8266WiFi.h>

#include <ESP8266WebServer.h>
#include <WiFiUdp.h>
#include <time.h>                       // time() ctime()
#include <sys/time.h>                   // struct timeval
#include <coredecls.h>                  // settimeofday_cb()
#include <ArduinoJson.h>
#include "FS.h"
#include "EspConfig.h"

#include "config.h"
#include "globals.h"
#include "webtools.h"
#include "webpages.h"


#define TZ              1       // (utc+) TZ in hours
#define DST_MN          60      // use 60mn for summer time in some countries
#define TZ_MN           ((TZ)*60)
#define TZ_SEC          ((TZ)*3600)
#define DST_SEC         ((DST_MN)*60)


timeval tv;

time_t now;


void setup()
{
  pinMode ( led, OUTPUT );
  pinMode ( resetPin, INPUT );
  
  digitalWrite ( led, 1 );
  Serial.begin(115200);
  Serial.println();

  Serial.println("");

 
  Serial.println(F("Booting...."));
  
  Serial.println(F("Mounting FS..."));
  if (!SPIFFS.begin()) {
    Serial.println(F("Failed to mount file system"));
    return;
  }
  config.init();
  Serial.println("******************************");
  //config.dbg();
  Serial.println("******************************");
  

  
  if (WiFi.SSID().length() > 0)
  {
    //WiFi config available
    WiFi.mode(WIFI_STA);
    WiFi.hostname(config.devicename);
    Serial.println(WiFi.hostname());
    Serial.print("Connecting to ");
    Serial.print(WiFi.SSID());
    WiFi.begin();
    if (!config.dhcp)
    {
      WiFi.config(config.ip, config.gw, config.sn, config.ns0, config.ns1);
    }    
    byte timeout = 0;
    while ((WiFi.status() == WL_DISCONNECTED) && (timeout < 50)) 
    {
      delay(250);
      Serial.print(".");
      timeout++;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
      Serial.println(F(" WiFi connected"));
      Serial.println(F("IP address: "));
      Serial.println(WiFi.localIP());
    } 
    else 
    {
      Serial.print(F(" Error: "));
      Serial.println(WiFi.status() );
    }
  } 
  else 
  {
    installMode = true;
  }

  if (WiFi.status() != WL_CONNECTED)
  {
    //connection failed or no saved config. Start AP
    Serial.println(F("No WiFi connection starting Access Point."));
    
    WiFi.mode(WIFI_AP);
    
    WiFi.softAPConfig(ap_IP, ap_IP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(config.devicename, ap_Pass);
    Serial.println(F("Access point started"));
    WiFi.scanNetworks(true);
  } 
  
  configTime(TZ_SEC, DST_SEC, config.ntpserver);
  
  initWebServer();
}

void loop()
{
 
  now = time(nullptr);

  webServer.handleClient();

  if (digitalRead(resetPin) == LOW){
    if (resetActive)
    {
      if (millis() - resetStart > 2000)
      {
        Serial.println(F("Reset started"));
      }
    } 
    else 
    {
      resetActive = true;
      resetStart = millis();
    }
  } 
  else 
  {
    resetActive = false;
  }
}

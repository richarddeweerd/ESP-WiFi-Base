bool resetActive = false;
unsigned long resetStart = 0;


void checkReset()
{
  if (digitalRead(resetPin) == LOW)
  {
    if (resetActive)
    {
      if (millis() - resetStart > resetTime)
      {
        Serial.println(F("Reset started"));
        resetActive = false;
        SPIFFS.format();
        WiFi.setAutoConnect(false);
        WiFi.disconnect(true);
        Serial.println(F("Rebooting"));
        ESP.restart();
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

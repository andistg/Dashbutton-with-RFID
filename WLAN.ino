//****************************************************************
//Einbindung WLAN
#include <ESP8266WiFi.h>
//****************************************************************

void setup()

//*****************************************************************
//Setup WLAN
{
  Serial.begin(115200);
  Serial.println();
  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.disconnect(true);
  delay(20);
  WiFi.mode(WIFI_STA);
  delay(20);

  WiFi.begin("SSID-Name", "WLAN-KEY");    // <<-- ändern

  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected, IP address: ");
  Serial.println( WiFi.macAddress() );
  Serial.println( WiFi.localIP() );
  Serial.println( ESP.getChipId() );
}
//**********************************************************************

void loop()
{
  heartbeat (500);
}


void heartbeat (unsigned int interval)          // Kontrolle ob loop läuft/blockiert
{
  static unsigned long last_ms = 0;
  static bool state = LOW;

  unsigned long ms = millis();

  if (ms - last_ms >= interval) {
    last_ms = ms;
    state = !state;
    digitalWrite(LED_BUILTIN, state);
  }
}

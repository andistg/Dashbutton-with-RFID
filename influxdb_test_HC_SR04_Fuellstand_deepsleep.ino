#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif


#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

int triggerPin = 12;                               // Der Trigger Pin
int echoPin = 14;                                  // Der Echo Pin
long dauer=0;                                // Hier wird die Zeitdauer abgespeichert
                                             // die die Ultraschallwelle braucht
                                             // um zum Sensor zurückzukommen
long entfernung=0;                           // Hier wird die Entfernung vom 
                                             // Hindernis abgespeichert

#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 1800 /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
      default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}


// WiFi AP SSID
#define WIFI_SSID "ASD100"
// WiFi password
#define WIFI_PASSWORD "schnabel1234"
// InfluxDB v2 server url, e.g. https://eu-central-1-1.aws.cloud2.influxdata.com (Use: InfluxDB UI -> Load Data -> Client Libraries)
#define INFLUXDB_URL "https://us-central1-1.gcp.cloud2.influxdata.com"
// InfluxDB v2 server or cloud API authentication token (Use: InfluxDB UI -> Data -> Tokens -> <select token>)
#define INFLUXDB_TOKEN "lh5Xp-kbwtkOZ1uUn1qnByuvAeIgf2dhlOEH3GwCndtjONjku66TX3IX2YU_4vP73ZZaaOQlK4n4JMT-CHQgew=="
// InfluxDB v2 organization id (Use: InfluxDB UI -> User -> About -> Common Ids )
#define INFLUXDB_ORG "andreas.a.schnabel@gmail.com"
// InfluxDB v2 bucket name (Use: InfluxDB UI ->  Data -> Buckets)
#define INFLUXDB_BUCKET "Messwerte BME280"

#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

// Data point
Point sensor("Smart Waste"); //_measurement

void setup() {
  Serial.begin(115200);
  delay(200);

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

   ++bootCount;
  Serial.println("Boot number: " + String(bootCount));

  //Print the wakeup reason for ESP32
  print_wakeup_reason();

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
  " Minute");
  
  pinMode(triggerPin, OUTPUT);                // Trigger Pin als Ausgang definieren
    pinMode(echoPin, INPUT);                    // Echo Pin als Eingang defnieren
  

  // Setup wifi
  

  // Add tags
  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", WiFi.SSID());

  // Accurate time is necessary for certificate validation and writing in batches
  // For the fastest time sync find NTP servers in your area: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

 digitalWrite(triggerPin, LOW);              
    delay(5);                                // 5 Millisekunden warten
    digitalWrite(triggerPin, HIGH);             // Den Trigger auf HIGH setzen um eine 
                                             // Ultraschallwelle zu senden
    delay(10);                               // 10 Millisekunden warten
    digitalWrite(triggerPin, LOW);              // Trigger auf LOW setzen um das 
                                             // Senden abzuschließen
    dauer = pulseIn(echoPin, HIGH);             // Die Zeit messen bis die 
                                             // Ultraschallwelle zurückkommt
    entfernung = (dauer/2) / 29.1;           // Die Zeit in den Weg in Zentimeter umrechnen
 
        Serial.print(entfernung);            // Den Weg in Zentimeter ausgeben
        Serial.println(" cm");               //


  sensor.clearFields();

  // Store measured value into point
  // Report RSSI of currently connected network
  sensor.addField("Fuellstand", entfernung); //_field
  

  // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

  // If no Wifi signal, try to reconnect it
  if ((WiFi.RSSI() == 0) && (wifiMulti.run() != WL_CONNECTED)) {
    Serial.println("Wifi connection lost");
  }

  // Write point
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
Serial.println("Going to sleep now");
 
  WiFi.mode(WIFI_OFF);

  Serial.flush(); 
  
  esp_deep_sleep_start();

}

void loop(){
  //This is not going to be called
}

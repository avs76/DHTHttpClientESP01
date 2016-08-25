#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include "DHT.h"

#define DHTPIN 2     // what digital pin we're connected to
#define DHTTYPE DHT11   
const char* ssid = "SSID";
const char* password = "password";

char Req[150]="";
float temperature=0;
float humidity=0;
float hhum=0;
float htemp=0;
float thum=0;
float ttemp=0;
//int light=0;
int tvcc =0;
float hvcc,vcc =0;
int counter=1;
DHT dht(DHTPIN, DHTTYPE);
ADC_MODE(ADC_VCC);

void setup() {

  Serial.begin(115200);
  Serial.println("");
  Serial.println("Starting...");
  
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(500);
  while (WiFi.status() != WL_CONNECTED) {
    delay(5000);
    Serial.println(".................");
  }

  
  Serial.println("");
  Serial.println("WiFi connected!");
  //Serial.println(WiFi.());
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  dht.begin();
}

void loop() {
 tvcc=ESP.getVcc();
// light = analogRead(A0);

 thum = dht.readHumidity();
  // Read temperature as Celsius (the default)
 ttemp = dht.readTemperature();
 
if (isnan(thum) || isnan(ttemp) ) {
    Serial.println("Failed DHT sensor!");
  } 
  else
  { 
    Serial.print("T= ");Serial.print(ttemp);
    Serial.print(" H= ");Serial.print(thum);
    Serial.print(" Vcc= ");Serial.println(tvcc);
    temperature=ttemp;
    humidity=thum;
  }
  
  hhum+=humidity;
  htemp+=temperature;
  hvcc+=tvcc;
    
  if(counter++>=20)
  { 
    counter=1;
    humidity=hhum/20;
    temperature=htemp/20;
    vcc=hvcc/20000;
    
    hhum=0;
    htemp=0;
    hvcc=0;
    if((WiFi.status() == WL_CONNECTED)) {
        
        HTTPClient http;

        Serial.print("[HTTP] begin...\n");

if (isnan(humidity) || isnan(temperature) ) {
    Serial.println("Failed to read from DHT sensor!");
    temperature=-1;
    humidity=-1;
  }
      sprintf(Req,"http://184.106.153.149/update?key=__YOUR__WRITE__KEY__&field1=%d.%d&field2=%d.%d&field3=%d.%d",(int)temperature,(int)(temperature*100)%100,(int)humidity, (int)(humidity*100)%100 ,(int)vcc, (int)(vcc*1000)%1000);
  
        http.begin(Req); //HTTP Get

        Serial.print("[HTTP] GET...\n");
        Serial.println(Req);
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                Serial.println(payload);
            }
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();

    }
  }
  delay(2800);
}


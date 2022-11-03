#include <ESP8266WiFi.h>
#include "secrets.h"
#include "ThingSpeak.h"
#include "DHT.h"

WiFiClient  client;

const char* ssid = SECRET_SSID;
const char* password = SECRET_PASS;
unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

int number = 0;

//Sensor's memory register addresses:
#define DHTPIN 2     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);
long lastReadingTime = 0;
float t = 0;
float h = 0;

void setup() {
  Serial.begin(115200);  // Initialize serial
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  WiFi.mode(WIFI_STA);

  // Inicia sensor DHT
  Serial.println("");
  Serial.println("DHT iniciado!");
  dht.begin();

  // Inicia comunicação ThingSpeak
  Serial.println("");
  Serial.println("ThingSpeak iniciado!");
  WiFi.mode(WIFI_STA);
  ThingSpeak.begin(client);
}

void loop()
{
  // Connect or reconnect to WiFi
  if(WiFi.status() != WL_CONNECTED){
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, password);  // Connect to WPA/WPA2 network
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
    // Mostra o endereco IP
    Serial.println(WiFi.localIP());
    }    
    // Grava dados no canal do ThinkSpeak
    getData();
}

void getData() {
  // Wait a few seconds between measurements.
  delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();

  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.println("\n");

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // set the fields with the values
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  
  // write to the ThingSpeak channel 
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
}

// Client Program for Machine Room
// Read temperature and humidity
// Sending Sensor Data to Server
// Displaying data on LCD

// ESP8266 WiFi Library
#include <ESP8266WiFi.h>

//  LCD I2C Library
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2); // Set the LCD address to 0x27 for a 16 chars and 2 line display

// Sensor Library
#include "DHT.h"

// Variable declaration for DHT sensor
#define DHTPIN 2
#define DHTTYPE DHT21  // DHT 21
DHT dht(DHTPIN,DHTTYPE);

//  Network Parameters
const char* miconssid = "SMU_SENSOR";       //  SSID of softAP
const char* miconpassword = "P@ssw0rd";     //  Password of softAP

IPAddress host(192,168,4,1);       // sofAPIP (Server)

void setup() {
  
  //  Initialize serial monitor
  Serial.begin(115200);
  delay(10);

  //  Initialize LCD
  lcd.init();
  lcd.backlight();

  //  Initialize DHT
  dht.begin();

  //  Function : Connect to WiFi
  connectToAP();    
}

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
        Serial.println("SoftAP Disconnected");
        connectToAP();
  }
  
  delay(300000);

  // Sensor Reading
  float humData = dht.readHumidity(); // Read humidity
  float tempData = dht.readTemperature(); // Read temperature (Celcius)

  // Sending Data to Server
  Serial.print("Connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("Connection failed!\n");
    return;
  }
  
  // We now create a URI for the request
  
  Serial.print("Connected to AP, now sending data...\n");

  // This will send the request to the server
  if (isnan(humData) || isnan(tempData)) {
      client.println("Failed to read from DHT sensor!\r");
  }
  else {
      client.print( String("Data from client (OP): \n") + 
                    "Humidity (%): " + humData + "\n" +
                    "Temperature (C): " + tempData + "\r");
      client.print(humData, DEC);
      client.print("\r");
      client.print(tempData, DEC);
      client.print("\r");
  }
  
  // Display data on LCD
  lcd.setCursor(0,0);
  lcd.print("Hum :" + String(humData) + " %");
  lcd.setCursor(0,1);
  lcd.print("Temp :" + String(tempData) + " C");
  delay(1);
    
  if (isnan(humData) || isnan(tempData)) {
    
      Serial.println("Failed to read from DHT sensor!\r");
  }
  else {
      
      Serial.println( "Humidity (%): " + String(humData) + "\n" +
                      "Temperature (C): " + String(tempData));
  }
               
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !\n");
      client.stop();
      return;
    }
  }
}

/*
 * FUNCTION DECLARATION =======================================>>>
 */

int connectToAP(){
    // Connect to WiFi network
    Serial.print("Connecting to ");
    Serial.println(miconssid);
    WiFi.mode(WIFI_STA);                      // Station Mode
    WiFi.begin(miconssid, miconpassword);     // Starting Connection
  
    while (WiFi.status() != WL_CONNECTED) {
        delay(2000);
        Serial.println(".");
    }

    // Print status Connect
    Serial.println("Connected to SoftAP");  
    Serial.print("localIP address: ");
    Serial.println(WiFi.localIP());
}

// softAP and Server Program for Operator Room
// Receive Sensor Data from Client
// Sending data to Thingspeak
// Activating Buzzer under certain conditions

// Library
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>

// Network Parameters
const char* ssid     = "Z";                 //  SSID of the router
const char* password = "00008888";          //  Password of the router
const char* miconssid = "SMU_SENSOR";       //  SSID for softAP
const char* miconpassword = "P@ssw0rd";     //  Password for softAP

// ThingSpeak information
char thingSpeakAddress[] = "api.thingspeak.com";
unsigned long channelID = 532231;       //  Thingspeak Channel ID (Changeable)
char* writeAPIKey = "GS70UETLJ9PXRCOM"; //  Thingspeak Channel Write API Key (Changeable)
const unsigned long postingInterval = 120L * 1000L;
unsigned int dataFieldOne = 1;  // Field to write humidity data
unsigned int dataFieldTwo = 2;  // Field to write temperature data

// Global variables
unsigned long lastConnectionTime = 0;
long lastUpdateTime = 0; 

// Server declaration
WiFiServer server(80);  // Using port 80  

void setup() {
  
  pinMode(2, OUTPUT);
  Serial.begin(115200);
  delay(10);
  
  Serial.println("Program Start");

  establishAP();
  connectWiFi();
  
}
 
void loop() {
  
  if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi Disconnected");
        reconnectWiFi();
  }
  
  // Check if a client has connected
  WiFiClient clientAP = server.available();
  if (!clientAP) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("Client connecting...");
  while(!clientAP.available()){
    delay(1);
  }

  // Read the first line of the request
  String req = clientAP.readStringUntil('\r');
  Serial.println(req);
  String humString = clientAP.readStringUntil('\r');
  String tempString = clientAP.readStringUntil('\r');
  float humFloat = humString.toFloat();
  float tempFloat = tempString.toFloat();

  //  Write data to Thingspeak if != NULL
  if (req != "Failed to read from DHT sensor!") {
      write2TSData(channelID , dataFieldOne , humFloat , dataFieldTwo , tempFloat);
      delay(1);
  }
  
  // Activate Buzzer and LED if the Temperature (C) is below 16 or above 24, or
  // Activate Buzzer and LED if the Humidity (%) is below XX or above XX
  if (tempFloat < 16 || tempFloat > 24 || humFloat < 60 || humFloat > 70){
    digitalWrite(2, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(5000);              // wait for a second
    digitalWrite(2, LOW);    // turn the LED off by making the voltage LOW
  }
    
  clientAP.flush();
  Serial.println("Client disonnected.\n");
  
  // The client will actually be disconnected when the function returns and 'client' object is detroyed
}

/*
 *  FUNCTION DECLARATION =======================================>>>
 */
 
int connectWiFi(){
    WiFiClient clientTS;
    
    while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, password);
        delay(5000);
        Serial.println("Connecting to " + String(ssid));
    }
    
    Serial.println("Connected to " + String(ssid));
    ThingSpeak.begin(clientTS);
}

int reconnectWiFi() {

    WiFiClient clientTS;
    
    while (WiFi.status() != WL_CONNECTED) {
        delay(3000);
        Serial.println("Connecting to " + String(ssid));
        WiFi.begin(ssid, password);
        if (WiFi.status() == WL_CONNECTED) {
            Serial.println("Connected to " + String(ssid));
            ThingSpeak.begin(clientTS);
        }
    }
}

int establishAP(){

    //  Mengatur softAP ----------------------------------------------------------
    Serial.print("Configuring access point...");

    WiFi.mode(WIFI_AP);
    WiFi.softAP(miconssid, miconpassword);
  
    //  Start the server -------------------------------------------------------
    server.begin();
    Serial.println("Server started!");
    Serial.println(WiFi.softAPIP());
}

int write2TSData( long TSChannel, unsigned int TSField1, float field1Data, unsigned int TSField2, float field2Data ){

  ThingSpeak.setField( TSField1, field1Data );
  ThingSpeak.setField( TSField2, field2Data );
  Serial.println("Sending to Thingspeak...");
  int writeSuccess = ThingSpeak.writeFields(TSChannel, writeAPIKey);
  if (writeSuccess){
    
    Serial.println("Data written to Thingspeak!" );
    }
  return writeSuccess;
}
/*
int displayLCD(String top, String bottom) {
    lcd.setCursor(0,0);
    lcd.print(top);
    lcd.setCursor(0,1);
    lcd.print(bottom);
}
*/

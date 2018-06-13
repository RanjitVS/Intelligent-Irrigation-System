#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN            D2         
#define DHTTYPE           DHT22     // DHT 22 (AM2302)
//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>
#include <SoftwareSerial.h>
#define relay D4
SoftwareSerial BT(D10,9); //TX, RX  pins of arduino respetively
String command;

DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

// WiFi network info.
char ssid[] = "VSR wifi";
char wifiPassword[] = "vsr123321";

// Cayenne authentication info. This should be obtained from the Cayenne Dashboard.
char username[] = "936938d0-f2d2-11e7-91c0-e55799240ffd";
char password[] = "9c45525fa05fbaaa1e7394f35738e0fd0002ea0a";
char clientID[] = "b8ffdf40-f2d2-11e7-8e1f-631f7dc7614e";

unsigned long lastMillis = 0;

void setup() {
  BT.begin(9600);
 Serial.begin(9600);
  Serial.begin(9600);
  Cayenne.begin(username, password, clientID, ssid, wifiPassword); 
  Serial.begin(9600); 
  // Initialize device.
  dht.begin();
  Serial.println("DHTxx Unified Sensor Example");
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Temperature");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
  Serial.println("------------------------------------");
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.println("Humidity");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
  Serial.println("------------------------------------");
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1;
}

void loop() {
  //Bluetooth
while (BT.available()){  //Check if there is an available byte to read
  char c = BT.read(); //Conduct a serial read
  command += c; //build the string.
  } 

  if (command.length() > 0) {
    Serial.println(command);
   if((command.length()>=1)&&(digitalRead(relay)==LOW))   
   {
    digitalWrite(relay,HIGH);
   }  
   else if((command.length()>=1)&&(digitalRead(relay)==HIGH))
   {
    digitalWrite(relay,LOW);
   }
 command="";} 

  //IOT
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    Serial.print("Temperature: ");
    Serial.print(event.temperature);
    Serial.println(" *C");
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    Serial.print("Humidity: ");
    Serial.print(event.relative_humidity);
    Serial.println("%");
  }
  
  Cayenne.loop();

  //Publish data every 10 seconds (10000 milliseconds). Change this value to publish at a different interval.
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();
    //Write data to Cayenne here. This example just sends the current uptime in milliseconds.
    Cayenne.virtualWrite(0, lastMillis);
    //Some examples of other functions you can use to send data.
    Cayenne.celsiusWrite(1, event.temperature);
    Cayenne.humidityWrite(2,event.humidity);
    //Cayenne.luxWrite(2, 700);
    //Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
  }
}

//Default function for processing actuator commands from the Cayenne Dashboard.
//You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("CAYENNE_IN_DEFAULT(%u) - %s, %s", request.channel, getValue.getId(), getValue.asString());
  //Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}

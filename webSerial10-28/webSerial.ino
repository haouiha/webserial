#include <ArduinoJson.h>
#include <EEPROM.h>
#include <StreamUtils.h>

byte STX = 02;
byte ETX = 03;

uint8_t START_PATTERN[3] = {0, 111, 222};

//https://arduinojson.org/v6/assistant/
const size_t capacity = JSON_OBJECT_SIZE(7) + 300;

DynamicJsonDocument jsonDoc(capacity);

String broker ,
       deviceid ,
       devicesecret ,
       devicetoken ,
       password ,
       port,
       ssid ;

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  while (!Serial) continue;
  Serial.write(START_PATTERN, 3);
  Serial.flush();

  EepromStream eeprom(0, EEPROM.length());
  deserializeJson(jsonDoc, eeprom);
  Serial.write(STX);
  serializeJsonPretty(jsonDoc, Serial);
  Serial.write(ETX);
}

void loop() {
  // https://arduinojson.org/v6/api/jsonvariant/as/
  while (Serial.available() > 0) {

    DeserializationError err = deserializeJson(jsonDoc, Serial);
    //EepromStream eepromStream(address, size);
    EepromStream eeprom(0, EEPROM.length());

    if (err == DeserializationError::Ok)
    {
      String command  =  jsonDoc["command"].as<String>();

      if (command == "getInfo") {
        //-----------------READING--------------------
        deserializeJson(jsonDoc, eeprom);
        Serial.write(STX);
        serializeJsonPretty(jsonDoc, Serial);
        Serial.write(ETX);
      } else if (command == "updateInfo") {
        //------------------WRITING-----------------
        serializeJson(jsonDoc["payload"], eeprom);
        eeprom.flush();
        Serial.write(STX);
        Serial.print("saved");
        Serial.write(ETX);
      } else if (command) {
        Serial.write(STX);
        Serial.print(command);
        Serial.println(", invalid command.");
        Serial.write(ETX);
      }

    }
    else
    {
      // Flush all bytes in the "link" serial port buffer
      Serial.read();
    }
  }

  broker = jsonDoc["broker"].as<String>();
  deviceid = jsonDoc["deviceid"].as<String>();
  devicesecret = jsonDoc["devicesecret"].as<String>();
  devicetoken = jsonDoc["devicetoken"].as<String>();
  password = jsonDoc["password"].as<String>();
  port = jsonDoc["port"].as<String>();
  ssid = jsonDoc["ssid"].as<String>();

//  Serial.print("broker");
//  Serial.println(broker);


}

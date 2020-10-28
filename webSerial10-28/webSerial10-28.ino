

#include <ArduinoJson.h>
#include <EEPROM.h>
#include <StreamUtils.h>

String x;

byte STX = 02;
byte ETX = 03;

uint8_t START_PATTERN[3] = {0, 111, 222};

DynamicJsonDocument jsonDoc(200);

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
        String payload  =  jsonDoc["payload"].as<String>();
        //------------------WRITING-----------------
        serializeJson(jsonDoc["payload"], eeprom);
        Serial.write(STX);
        Serial.print("saved");
        eeprom.flush();
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

}

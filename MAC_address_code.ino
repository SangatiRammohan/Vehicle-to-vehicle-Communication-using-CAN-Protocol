#include <ESP8266WiFi.h>

void setup(){
   //tx = 48:3F:DA:AB:27:16
   //rx = A4:CF:12:F2:C7:F2
   // CAR MAC RX = 48:55:19:E0:7B:17
  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
}

void loop(){

}

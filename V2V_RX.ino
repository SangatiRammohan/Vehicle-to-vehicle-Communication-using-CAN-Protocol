#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
#define buzzer D5
// Structure to receive data
typedef struct struct_message {
    float temperature;
    float humidity;  
    int gasSensorValue;
    int irSensorValue;
} struct_message;

// Data container
struct_message myData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.println(len);
  
  Serial.print("Temperature: ");
  Serial.println(myData.temperature);
  Serial.print("Humidity: ");
  Serial.println(myData.humidity);
  
  Serial.print("Gas Sensor Value: ");
  Serial.println(myData.gasSensorValue);

  Serial.print("IR Sensor Value: ");
  Serial.println(myData.irSensorValue);

    lcd.setCursor(0, 0);       
    lcd.print("T:");
    lcd.print(myData.temperature);
    lcd.setCursor(0, 1);       
    lcd.print("H:");
    lcd.print(myData.humidity);
    lcd.setCursor(8, 0);       
    lcd.print("G:");
    lcd.print(myData.gasSensorValue);
    lcd.setCursor(8, 1);       
    lcd.print("F:");
    lcd.print("60%");
}
 
void setup() {
  pinMode(buzzer,OUTPUT);
  // Initialize Serial Monitor
  Serial.begin(115200);

    lcd.init();                    
    lcd.backlight();
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Set the ESP-NOW role to Slave
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  
  // Register callback for receiving data
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  if(myData.irSensorValue==0)
{
  digitalWrite(buzzer,HIGH);
}
else
{
    digitalWrite(buzzer,LOW);
  }
}

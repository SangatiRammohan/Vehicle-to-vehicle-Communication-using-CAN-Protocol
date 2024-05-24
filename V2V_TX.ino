#include <ESP8266WiFi.h>
#include <espnow.h>
#include <DHT.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define DHTPIN D5          // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11      // DHT 22  (AM2302)
#define GAS_SENSOR_PIN A0 // Analog pin connected to the gas sensor
#define IR_SENSOR_PIN D3   // Digital pin connected to the IR sensor

DHT dht(DHTPIN, DHTTYPE);

// Receiver MAC Address
uint8_t receiverAddress[] = {0xA4, 0xCF, 0x12, 0xF2, 0xC7, 0xF2};

// Structure to send data
typedef struct struct_message {
  float temperature;
  float humidity;
  int gasSensorValue;
  int irSensorValue;
} struct_message;

// Data container
struct_message myData;

unsigned long lastTime = 0;
unsigned long timerDelay = 2000;  // send readings timer

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

void setup() {

  
    lcd.init();                    
    lcd.backlight(); 
    
  pinMode(IR_SENSOR_PIN, INPUT);
  // Init Serial Monitor
  Serial.begin(115200);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set the ESP-NOW role to Controller
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  
  // Register callback for sending data
  esp_now_register_send_cb(OnDataSent);

  // Add the receiver peer
  esp_now_add_peer(receiverAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  if ((millis() - lastTime) > timerDelay) {
    // Read temperature and humidity from DHT sensor
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();
  int gasSensorValue = analogRead(GAS_SENSOR_PIN);
  int irSensorValue = digitalRead(IR_SENSOR_PIN);

    lcd.setCursor(0, 0);       
    lcd.print("T:");
    lcd.print(temperature);
    lcd.setCursor(0, 1);       
    lcd.print("H:");
    lcd.print(humidity);
    lcd.setCursor(8, 0);       
    lcd.print("G:");
    lcd.print(gasSensorValue);
    lcd.setCursor(8, 1);       
    lcd.print("F:");
    lcd.print("60%");
    
    // Check if any reads failed and exit early (to try again)
    if (isnan(temperature) || isnan(humidity)) {
      Serial.println("Failed to read from DHT sensor!");
      lastTime = millis();
      return;
    }

    // Set values to send
    myData.temperature = temperature;
    myData.humidity = humidity;
    myData.gasSensorValue = gasSensorValue;
    myData.irSensorValue = irSensorValue;
    // Send message via ESP-NOW
    esp_now_send(receiverAddress, (uint8_t *)&myData, sizeof(myData));

    Serial.println("Data sent via ESP-NOW");

    lastTime = millis();
  }
}

/*
  Bai tap lon - He thong Vien thong
  Node do nhiet do & do am voi xu ly tai bien (Edge Computing) - Milestone 1

  Phan cung:
    - ESP32 DevKit
    - Cam bien SHT30 (I2C, dia chi 0x44): SDA = GPIO21, SCL = GPIO22
    - LED canh bao GPIO2 (qua tro 220 ohm), coi buzzer GPIO4

  Chuc nang:
    - Doc nhiet do / do am tu SHT30 qua I2C
    - So sanh voi nguong ngay tren ESP32 (xu ly tai bien)
    - Vuot nguong -> bat LED + coi
    - In du lieu ra Serial Monitor (115200 baud)

  Thu vien: Adafruit SHT31 Library (dung duoc cho SHT30)
  Milestone tiep theo: WiFi + MQTT, painlessMesh, Edge AI (Edge Impulse)
*/

#include <Wire.h>
#include "Adafruit_SHT31.h"

// ---------- Cau hinh chan ----------
#define LED_PIN     2
#define BUZZER_PIN  4

// ---------- Nguong canh bao ----------
const float TEMP_THRESHOLD = 40.0;   // °C
const float HUMI_THRESHOLD = 80.0;   // % RH

Adafruit_SHT31 sht30 = Adafruit_SHT31();

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  if (!sht30.begin(0x44)) {
    Serial.println("Khong tim thay cam bien SHT30! Kiem tra lai day noi.");
    while (1) delay(10);
  }
  Serial.println("Cam bien SHT30 san sang.");
}

void loop() {
  float temp = sht30.readTemperature();
  float humi = sht30.readHumidity();

  if (isnan(temp) || isnan(humi)) {
    Serial.println("Loi doc cam bien!");
    delay(2000);
    return;
  }

  Serial.print("Nhiet do: ");
  Serial.print(temp, 1);
  Serial.print(" C | Do am: ");
  Serial.print(humi, 1);
  Serial.print(" %");

  // Xu ly tai bien: so nguong ngay tren ESP32
  bool alert = (temp > TEMP_THRESHOLD) || (humi > HUMI_THRESHOLD);

  if (alert) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("  --> CANH BAO!");
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("  (binh thuong)");
  }

  delay(2000);
}

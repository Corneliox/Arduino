#include "DHT.h"

// ================================
// KONFIGURASI PIN
// ================================
#define DHTPIN1 17  
#define DHTPIN2 16
#define DHTTYPE DHT22
#define BATT_PIN 35   

// ================================
// KONFIGURASI KALIBRASI (OFFSET) - TAHAP 2
// ================================
// Target: Xiaomi (27.1C | 62%)
#define T1_OFFSET -1.1
#define H1_OFFSET -11.6
#define T2_OFFSET -0.6
#define H2_OFFSET -17.2

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

void setup() {
  Serial.begin(115200);
  dht1.begin();
  dht2.begin();
  
  Serial.println("=== MODE VALIDASI TERKALIBRASI V2 ===");
  Serial.println("Data akan diupdate setiap 1 detik.");
  Serial.println("-------------------------------------------------------------");
  Serial.println("Suhu 1 (C)\tHum 1 (%)\tSuhu 2 (C)\tHum 2 (%)\tBaterai (V)");
  Serial.println("-------------------------------------------------------------");
}

void loop() {
  // 1. Baca Sensor RAW
  float h1_raw = dht1.readHumidity();
  float t1_raw = dht1.readTemperature();
  float h2_raw = dht2.readHumidity();
  float t2_raw = dht2.readTemperature();

  // 2. Terapkan Kalibrasi
  float t1 = t1_raw + T1_OFFSET;
  float h1 = h1_raw + H1_OFFSET;
  float t2 = t2_raw + T2_OFFSET;
  float h2 = h2_raw + H2_OFFSET;

  // 3. Baca Baterai
  int raw = analogRead(BATT_PIN);
  float voltage = (raw * (3.3 / 4095.0)) * 2.0;

  // 4. Tampilkan Data Terkalibrasi
  // Format: Nilai Terkalibrasi (Nilai Asli)
  
  if (isnan(t1)) Serial.print("Error\t\t"); 
  else { 
    Serial.print(t1, 1); 
    Serial.print("\t\t"); 
  }

  if (isnan(h1)) Serial.print("Error\t\t"); 
  else { 
    Serial.print(h1, 1); 
    Serial.print("\t\t"); 
  }

  if (isnan(t2)) Serial.print("Error\t\t"); 
  else { 
    Serial.print(t2, 1); 
    Serial.print("\t\t"); 
  }

  if (isnan(h2)) Serial.print("Error\t\t"); 
  else { 
    Serial.print(h2, 1); 
    Serial.print("\t\t"); 
  }

  Serial.println(voltage);

  delay(1000);
}
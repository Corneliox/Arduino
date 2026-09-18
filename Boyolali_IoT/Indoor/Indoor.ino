#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiManager.h>
#include <ThingSpeak.h>
#include "DHT.h"
#include "secrets.h"

// ================================
// KONFIGURASI SENSOR & PIN
// ================================
#define DHTPIN1 17  
#define DHTPIN2 16
#define DHTTYPE DHT22

DHT dht1(DHTPIN1, DHTTYPE);
DHT dht2(DHTPIN2, DHTTYPE);

// ================================
// KALIBRASI SENSOR (Offset) - TAHAP 2
// ================================
#define TEMP1_OFFSET -0.4
#define HUM1_OFFSET  -20.6
#define TEMP2_OFFSET -0.3
#define HUM2_OFFSET  -14.2

// Lolin D32 menggunakan GPIO 35 untuk baterai internal
#define BATT_PIN 35   

// ================================
// KONFIGURASI THINGSPEAK
// ================================
unsigned long myChannelNumber = SECRET_CH_ID;       
const char* myWriteAPIKey = SECRET_WRITE_API_KEY; 

WiFiClient client;
WiFiMulti wifiMulti; 

// ================================
// KONFIGURASI DEEP SLEEP
// ================================
#define SLEEP_MINUTES 5  
#define uS_TO_S_FACTOR 1000000
#define TIME_TO_SLEEP  (SLEEP_MINUTES * 60 * uS_TO_S_FACTOR)

// ================================
// FUNGSI TIDUR
// ================================
void goToSleep() {
  Serial.printf("\n💤 Selesai. Masuk Deep Sleep selama %d menit...\n", SLEEP_MINUTES);
  Serial.flush(); 
  
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP);
  esp_deep_sleep_start();
}

// ================================
// SETUP UTAMA
// ================================
void setup() {
  Serial.begin(115200);
  delay(1000); 

  // 1. Nyalakan Sensor
  dht1.begin();
  dht2.begin();
  delay(2000); 

  // 2. Baca Data Sensor RAW
  float h1_raw = dht1.readHumidity();
  float t1_raw = dht1.readTemperature();
  float h2_raw = dht2.readHumidity();
  float t2_raw = dht2.readTemperature();

  // Validasi Fisik Sensor
  if (isnan(h1_raw) || isnan(t1_raw)) { Serial.println("⚠️ Sensor 1 Error"); h1_raw = 0; t1_raw = 0; }
  if (isnan(h2_raw) || isnan(t2_raw)) { Serial.println("⚠️ Sensor 2 Error"); h2_raw = 0; t2_raw = 0; }

  // 3. Terapkan KALIBRASI
  float t1 = t1_raw + TEMP1_OFFSET;
  float h1 = h1_raw + HUM1_OFFSET;
  float t2 = t2_raw + TEMP2_OFFSET;
  float h2 = h2_raw + HUM2_OFFSET;

  // Pastikan tidak ada nilai aneh (misal kelembapan negatif)
  if (h1 < 0) h1 = 0; if (h1 > 100) h1 = 100;
  if (h2 < 0) h2 = 0; if (h2 > 100) h2 = 100;

  // 4. Baca Baterai
  analogReadResolution(12);
  int raw = 0;
  for(int i=0; i<10; i++){ raw += analogRead(BATT_PIN); delay(2); }
  raw /= 10;
  
  float voltage = (raw * (3.3 / 4095.0)) * 2.0;
  // Menggunakan rumus float agar presisi desimal terjaga
  float battPercent = (voltage - 3.2) * 100.0 / (4.2 - 3.2);
  
  if (battPercent > 100) battPercent = 100;
  if (battPercent < 0) battPercent = 0;

  // TAMPILAN SERIAL MONITOR YANG DIMINTA
  Serial.println("\n--- DATA SENSOR TERKALIBRASI ---");
  Serial.printf("Sensor 1: %.1f C (Raw %.1f), %.1f %% (Raw %.1f)\n", t1, t1_raw, h1, h1_raw);
  Serial.printf("Sensor 2: %.1f C (Raw %.1f), %.1f %% (Raw %.1f)\n", t2, t2_raw, h2, h2_raw);
  // Menggunakan %.2f %% agar persentase baterai tampil 2 angka desimal (lebih presisi)
  Serial.printf("Baterai : %.2f V (%.2f %%)\n", voltage, battPercent);

  // =========================================
  // LOGIKA KONEKSI WIFI
  // =========================================
  WiFi.mode(WIFI_STA); 
  WiFi.setTxPower(WIFI_POWER_19_5dBm);

  for (size_t i = 0; i < NUM_WIFI_NETWORKS; i++) {
    wifiMulti.addAP(WIFI_NETWORKS[i].ssid, WIFI_NETWORKS[i].password);
  }

  Serial.println("\nMencoba menghubungkan ke WiFi tersimpan...");
  
  boolean connected = false;

  for(int i=0; i<20; i++) { 
    if(wifiMulti.run() == WL_CONNECTED) {
      connected = true;
      break;
    }
    Serial.print(".");
    delay(500);
  }

  if (!connected) {
    Serial.println("\n⚠️ WiFi Prioritas tidak ditemukan. Membuka WiFiManager...");
    WiFiManager wm;
    wm.setConfigPortalTimeout(180); 
    
    if (!wm.autoConnect("Wemos Indoor", "12345678")) {
      Serial.println("❌ Gagal Konek via Portal. Tidur...");
      goToSleep();
    }
  }

  Serial.println("\n✅ WiFi connected");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // =========================================
  // KIRIM KE THINGSPEAK
  // =========================================
  ThingSpeak.begin(client);

  // Kirim Data TERKALIBRASI
  ThingSpeak.setField(1, t1); 
  ThingSpeak.setField(2, t2); 
  ThingSpeak.setField(3, h1); 
  ThingSpeak.setField(4, h2); 
  ThingSpeak.setField(5, (int)battPercent);

  int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

  if (httpCode == 200) {
    Serial.println("✅ Data terkirim ke ThingSpeak!");
  } 
  else if (httpCode == -401) {
    Serial.println("⚠️ Tabrakan (-401). Menunggu 16 detik...");
    delay(16000); 
    Serial.println("🔄 Mengirim ulang...");
    httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    
    if (httpCode == 200) Serial.println("✅ Sukses Retry!");
    else Serial.println("❌ Gagal Retry.");
  } 
  else {
    Serial.println("❌ Gagal Error: " + String(httpCode));
  }

  goToSleep();
}

void loop() {}
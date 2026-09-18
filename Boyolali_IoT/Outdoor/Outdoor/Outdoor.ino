#include <WiFi.h>
#include <WiFiMulti.h> // Library untuk prioritas WiFi
#include <WiFiManager.h>
#include <ThingSpeak.h>
#include <LiquidCrystal.h>
#include "DHT.h"
#include "secrets.h"

// =========================================
// KONFIGURASI PIN & HARDWARE
// =========================================
// LCD Parallel (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(19, 23, 14, 27, 25, 26); 
// CATATAN: Sesuaikan pin LCD dengan sambungan kabel Anda!
// Jika pakai kabel lama saya: 12, 14, 27, 26, 25, 33
// Jika pakai kabel baru Anda: 19, 23, 14, 27, 25, 26

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// =========================================
// KALIBRASI SENSOR (Outdoor)
// =========================================
#define TEMP_OFFSET -0.9
#define HUM_OFFSET  -7.0

// =========================================
// KONFIGURASI THINGSPEAK
// =========================================
unsigned long myChannelNumber = SECRET_CH_ID;       
const char* myWriteAPIKey = SECRET_WRITE_API_KEY; 

WiFiClient client;
WiFiMulti wifiMulti; 

// Timer
unsigned long lastSendTime = 0;
unsigned long sendInterval = 30000; // Kirim setiap 30 Detik

// =========================================
// FUNGSI KONEKSI WIFI (LOGIKA KOMPLEKS)
// =========================================
void connectToWiFi() {
  // 1. Coba WiFi Prioritas (Hardcoded)
  lcd.clear();
  lcd.print("Mencari WiFi...");
  Serial.println("\n[WiFi] Mencari WiFi Prioritas...");
  
  // MENINGKATKAN JANGKAUAN SINYAL:
  // Set daya pancar WiFi ke level maksimal (19.5 dBm)
  WiFi.setTxPower(WIFI_POWER_19_5dBm);

  // Tambahkan daftar WiFi
  for (size_t i = 0; i < NUM_WIFI_NETWORKS; i++) {
    wifiMulti.addAP(WIFI_NETWORKS[i].ssid, WIFI_NETWORKS[i].password);
  }

  // Coba connect selama 10 detik
  int attempts = 0;
  while (wifiMulti.run() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  // 2. Jika Gagal, Buka Portal AP (WiFiManager)
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n[WiFi] Prioritas gagal. Membuka Portal AP 5 Menit...");
    
    lcd.clear();
    lcd.setCursor(0,0); lcd.print("WiFi Hilang!");
    lcd.setCursor(0,1); lcd.print("Buka HP Anda");
    delay(2000);

    WiFiManager wm;
    
    // Callback untuk menampilkan nama AP di LCD
    wm.setAPCallback([](WiFiManager *myWiFiManager) {
      lcd.clear();
      lcd.print("Mode Setup WiFi");
      lcd.setCursor(0,1);
      lcd.print("SSID: Wemos Out"); // Nama AP dipendekkan agar muat LCD
    });

    // Set Timeout 5 Menit (300 detik)
    wm.setConfigPortalTimeout(300); 
    
    // Buat AP. Jika timeout, fungsi ini return false.
    if (!wm.autoConnect("Wemos Outdoor", "12345678")) {
      Serial.println("\n[WiFi] Timeout AP habis. Restarting connection logic...");
      lcd.clear();
      lcd.print("Gagal Konek");
      lcd.setCursor(0,1);
      lcd.print("Coba Ulang...");
      delay(3000);
      // Kita tidak restart ESP, tapi membiarkan loop berjalan agar mencoba lagi nanti
      return; 
    }
  }

  // 3. Sukses Terhubung
  Serial.println("\n[WiFi] Terhubung!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  
  lcd.clear();
  lcd.print("WiFi OK");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  delay(2000);
}

void setup() {
  Serial.begin(115200);
  
  // Init Hardware
  lcd.begin(16, 2);
  dht.begin();
  
  lcd.print("Unit Outdoor");
  lcd.setCursor(0,1);
  lcd.print("Menyalakan...");
  delay(2000);

  // Jalankan logika koneksi awal
  connectToWiFi();
  
  ThingSpeak.begin(client);
}

void loop() {
  // Cek koneksi WiFi setiap saat. Jika putus, jalankan prosedur koneksi lagi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[Loop] WiFi Putus! Menjalankan prosedur koneksi...");
    connectToWiFi();
  }

  // Timer Non-Blocking (Setiap 30 Detik)
  if (millis() - lastSendTime > sendInterval) {
    
    // 1. Baca Sensor RAW
    float h_raw = dht.readHumidity();
    float t_raw = dht.readTemperature();

    // Cek Error Sensor
    if (isnan(h_raw) || isnan(t_raw)) {
      Serial.println("Gagal baca sensor DHT!");
      lcd.clear();
      lcd.print("Sensor Error");
      return; 
    }

    // 2. Terapkan Kalibrasi
    float t = t_raw + TEMP_OFFSET;
    float h = h_raw + HUM_OFFSET;

    // Validasi batas (0-100%)
    if (h < 0) h = 0;
    if (h > 100) h = 100;

    // 3. Tampilkan di LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("T:" + String(t, 1) + "C");
    lcd.setCursor(9, 0); // Geser ke kanan
    lcd.print("H:" + String(h, 0) + "%");
    
    lcd.setCursor(0, 1);
    lcd.print("Kirim Data...");

    // 4. Kirim ke ThingSpeak (Field 6 & 7)
    ThingSpeak.setField(1, t);
    ThingSpeak.setField(2, h);

    int httpCode = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);

    if (httpCode == 200) {
      Serial.println("✅ Data Outdoor Terkirim");
      lcd.setCursor(0, 1);
      lcd.print("Terkirim OK   ");
    } else {
      Serial.println("❌ Gagal Kirim: " + String(httpCode));
      lcd.setCursor(0, 1);
      lcd.print("Err: " + String(httpCode));
    }

    lastSendTime = millis(); // Reset timer
  }
}
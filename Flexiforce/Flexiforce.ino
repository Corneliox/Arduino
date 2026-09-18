/*
 * Proyek: FlexiForce A201 + TUR-AMP Drive Circuit + Bar LED Indikator
 * Pinout:
 * - Pin A0: Input Analog dari terminal 'OUT' modul TUR-AMP
 * - Pin D2 s/d D6: Anoda LED 1 s/d 5 (via resistor 220 Ohm ke GND)
 */

// Konfigurasi Pin
const int SENSOR_PIN = A0;
const int LED_PINS[] = {2, 3, 4, 5, 6};
const int NUM_LEDS = 5;

// ================= KALIBRASI BEBAN =================
// Sesuaikan dua angka ini berdasarkan hasil bacaan Serial Monitor Anda
const int ADC_MIN = 5;   // Nilai ADC saat sensor TIDAK ditekan (Offset/Deadband)
const int ADC_MAX = 50;  // Nilai ADC saat ditekan MAKSIMAL (sebelum jenuh)

// Parameter Filter (Exponential Moving Average)
float filteredValue = 0.0;
const float ALPHA = 0.18; // Faktor respons filter (0.1 = sangat halus, 0.4 = sangat responsif)

// Variabel Waktu Non-Blocking (Pengganti delay)
unsigned long previousMillis = 0;
const unsigned long SAMPLE_INTERVAL_MS = 20; // Sampling rate 50 Hz (tiap 20 ms)

void setup() {
  // Gunakan baud rate cepat agar transfer data serial tidak menghambat CPU
  Serial.begin(115200);

  // Inisialisasi pin LED sebagai output
  for (int i = 0; i < NUM_LEDS; i++) {
    pinMode(LED_PINS[i], OUTPUT);
    digitalWrite(LED_PINS[i], LOW);
  }

  // Inisialisasi awal nilai filter
  filteredValue = analogRead(SENSOR_PIN);
}

void loop() {
  unsigned long currentMillis = millis();

  // Eksekusi setiap 20 milidetik (non-blocking)
  if (currentMillis - previousMillis >= SAMPLE_INTERVAL_MS) {
    previousMillis = currentMillis;

    // 1. Baca data analog mentah
    int rawADC = analogRead(SENSOR_PIN);

    // 2. Filter sinyal untuk meredam noise listrik dan fluktuasi mekanis
    filteredValue = (ALPHA * rawADC) + ((1.0 - ALPHA) * filteredValue);

    // 3. Batasi dan petakan data ke rentang kalibrasi yang valid
    int clampedValue = constrain((int)filteredValue, ADC_MIN, ADC_MAX);
    
    // 4. Konversi nilai ke jumlah level LED yang harus menyala (0 sampai 5)
    int activeLeds = 0;
    if (clampedValue > ADC_MIN) {
      activeLeds = map(clampedValue, ADC_MIN, ADC_MAX, 1, NUM_LEDS);
    }

    // 5. Perbarui status fisik pin LED
    for (int i = 0; i < NUM_LEDS; i++) {
      digitalWrite(LED_PINS[i], i < activeLeds ? HIGH : LOW);
    }

    // 6. Format telemetri untuk Arduino Serial Plotter (Tools > Serial Plotter)
    Serial.print("Mentah:");
    Serial.print(rawADC);
    Serial.print(",");
    Serial.print("Terfilter:");
    Serial.print((int)filteredValue);
    Serial.print(",");
    Serial.print("Level_LED:");
    Serial.println(activeLeds * (ADC_MAX / NUM_LEDS));
  }
}
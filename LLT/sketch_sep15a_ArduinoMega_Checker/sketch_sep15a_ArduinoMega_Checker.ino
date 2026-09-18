#define INPUT_PIN 2 // Pin 2 Mega (Kabel dari Pin 11 Uno)

// ====================================================================
// CONFIGURATION: SET DURASI PEREKAMAN DI SINI
// ====================================================================
const uint32_t recordDurationSec = 0; // 0 = Bebal tanpa waktu | >0 = Batas waktu rekam (detik)
// ====================================================================

// Variabel Interupsi Volatile (Direct Register Capture)
volatile uint32_t rawHigh = 0;
volatile uint32_t rawPeriod = 0;
volatile bool pulseReady = false;
volatile uint32_t lastRise = 0;
volatile uint32_t fallTime = 0;

void signalChangeISR() {
  uint32_t now = micros();
  if (PINE & 0x10) { // Cek Register PE4 (Pin 2 Mega) secara instan
    if (lastRise > 0) {
      rawPeriod = now - lastRise;
      if (fallTime >= lastRise) {
        rawHigh = fallTime - lastRise;
      }
      pulseReady = true;
    }
    lastRise = now;
  } else {
    fallTime = now;
  }
}

// Variabel Logika Sistem
enum SystemState { MODE_RAW, MODE_RECORD, MODE_DONE };
SystemState currentState = MODE_RAW;

uint32_t stateStartMillis = 0;
uint32_t lastOutputMillis = 0;

// Variabel Akumulator Statistik Global
uint32_t totalPulsesInSession = 0;
float hzSessionSum = 0.0f;
float dutySessionSum = 0.0f;

// Variabel Interval 2 Detik (Khusus Record Mode)
uint32_t pulseCount2s = 0;
float hzSum2s = 0.0f;
float dutySum2s = 0.0f;

void resetStats() {
  pulseCount2s = 0;
  hzSum2s = 0.0f;
  dutySum2s = 0.0f;
  
  totalPulsesInSession = 0;
  hzSessionSum = 0.0f;
  dutySessionSum = 0.0f;
  
  stateStartMillis = millis();
  lastOutputMillis = millis();
  pulseReady = false;
}

void setup() {
  pinMode(INPUT_PIN, INPUT);
  Serial.begin(115200);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), signalChangeISR, CHANGE);

  Serial.println("=====================================================");
  Serial.println("         Sistem Kontrol Waktu Mega Aktif             ");
  Serial.print("   Konfigurasi Batas Waktu: ");
  if (recordDurationSec == 0) Serial.println("BEBAL (Tanpa Batas)");
  else { Serial.print(recordDurationSec); Serial.println(" Detik"); }
  Serial.println("   Ketik 'raw' atau 'record' untuk memulai/pindah    ");
  Serial.println("=====================================================");
}

void loop() {
  // 1. BACA PERINTAH PERALIHAN MODE VIA SERIAL MONITOR
  if (Serial.available() > 0) {
    String cmd = Serial.readString();
    cmd.trim();
    
    if (cmd.equalsIgnoreCase("raw")) {
      currentState = MODE_RAW;
      resetStats();
      Serial.println("\n>>> SESSIN MULAI: RAW MODE (Semburan Instan) <<<");
    } 
    else if (cmd.equalsIgnoreCase("record")) {
      currentState = MODE_RECORD;
      resetStats();
      Serial.println("\n>>> SESI MULAI: RECORD MODE (Ringkasan 2 Detik) <<<");
    }
  }

  // Jika sesi rekam sudah selesai (Limit Tercapai), abaikan pemrosesan pulsa
  if (currentState == MODE_DONE) return;

  uint32_t currentMillis = millis();
  uint32_t elapsedMillis = currentMillis - stateStartMillis;

  // 2. CEK TIMEOUT / PEMBATASAN RANGE WAKTU
  if (recordDurationSec > 0 && elapsedMillis >= (recordDurationSec * 1000)) {
    // Tampilkan data akumulasi terakhir jika berada di mode record sebelum ditutup
    if (currentState == MODE_RECORD && pulseCount2s > 0) {
      Serial.print("[MEGA RECORD 2s] Total Pulsa: "); Serial.print(pulseCount2s);
      Serial.print(" | Hz Rata-rata: "); Serial.print(hzSum2s / pulseCount2s, 3);
      Serial.print(" Hz | Duty Rata-rata: "); Serial.print(dutySum2s / pulseCount2s, 2); Serial.println("%");
    }

    // CETAK RINGKASAN AKHIR SESI (FINAL REPORT BENCHMARK)
    Serial.println("\n=================== HASIL AKHIR REKAMAN ===================");
    Serial.print("Status Sesi      : SELESAI (Waktu Terpenuhi)\n");
    Serial.print("Durasi Perekaman : "); Serial.print((float)elapsedMillis / 1000.0f, 2); Serial.println(" detik");
    Serial.print("Total Pulsa Raw  : "); Serial.println(totalPulsesInSession);
    if (totalPulsesInSession > 0) {
      Serial.print("Prediksi Hz Riil : "); Serial.print(hzSessionSum / totalPulsesInSession, 3); Serial.println(" Hz");
      Serial.print("Rata-rata Duty   : "); Serial.print(dutySessionSum / totalPulsesInSession, 2); Serial.println("%");
    } else {
      Serial.println("Hasil            : Tidak ada pulsa sinyal terdeteksi selama rentang waktu.");
    }
    Serial.println("===========================================================");
    Serial.println("Sistem masuk ke mode IDLE. Ketik 'raw' atau 'record' untuk mengulang.");
    
    currentState = MODE_DONE;
    return;
  }

  // 3. LOGIKA PROSES DATA PULSA JALUR MURNI RAW
  if (pulseReady) {
    noInterrupts();
    uint32_t highDur = rawHigh;
    uint32_t period = rawPeriod;
    pulseReady = false;
    interrupts();

    if (period > 0 && highDur > 0 && highDur <= period) {
      float actualHz = 1000000.0f / (float)period;
      float actualDuty = ((float)highDur * 100.0f) / (float)period;

      // Akumulasi data global untuk laporan akhir sesi
      totalPulsesInSession++;
      hzSessionSum += actualHz;
      dutySessionSum += actualDuty;

      if (currentState == MODE_RAW) {
        // Output secepat kilat (Tanpa jeda komputasi)
        Serial.print("[MEGA RAW] ON: "); Serial.print(highDur);
        Serial.print(" us | Periode: "); Serial.print(period);
        Serial.print(" us | Hz: "); Serial.print(actualHz, 3);
        Serial.print(" | Duty: "); Serial.print(actualDuty, 2); Serial.println("%");
      } 
      else if (currentState == MODE_RECORD) {
        // Simpan data ke akumulator interval 2 detik
        hzSum2s += actualHz;
        dutySum2s += actualDuty;
        pulseCount2s++;
      }
    }
  }

  // 4. OUTPUT KHUSUS RECORD MODE (INTERVAL PER 2 DETIK)
  if (currentState == MODE_RECORD) {
    if (currentMillis - lastOutputMillis >= 2000) {
      Serial.print("[MEGA RECORD 2s] ");
      if (pulseCount2s > 0) {
        Serial.print("Total Pulsa: "); Serial.print(pulseCount2s);
        Serial.print(" | Hz Rata-rata: "); Serial.print(hzSum2s / pulseCount2s, 3);
        Serial.print(" Hz | Duty Rata-rata: "); Serial.print(dutySum2s / pulseCount2s, 2); Serial.println("%");
      } else {
        Serial.print("Sinyal statis/macet! Status Pin saat ini: ");
        Serial.println((PINE & 0x10) ? "HIGH" : "LOW");
      }
      
      // Reset akumulator parsial 2 detik
      pulseCount2s = 0;
      hzSum2s = 0.0f;
      dutySum2s = 0.0f;
      lastOutputMillis = currentMillis;
    }
  }
}


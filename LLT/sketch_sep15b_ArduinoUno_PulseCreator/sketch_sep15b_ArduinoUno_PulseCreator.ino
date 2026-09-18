#define LASER_PIN 11

float flashingRate = 30.0f; 
float dutyCycle = 50.0f;   

uint32_t lastCycleStart = 0;
uint32_t periodMicros = 0;
uint32_t onMicros = 0;
bool isPinHigh = false;

void updateTimings() {
  // Hitung periode dan durasi ON dalam mikrodetik
  periodMicros = (uint32_t)(1000000.0f / flashingRate);
  onMicros = (uint32_t)((float)periodMicros * (dutyCycle / 100.0f));
}

void setup() {
  pinMode(LASER_PIN, OUTPUT);
  digitalWrite(LASER_PIN, LOW);
  Serial.begin(9600);

  updateTimings();
  lastCycleStart = micros();
}

void loop() {
  uint32_t now = micros();
  uint32_t elapsed = now - lastCycleStart;

  // 1. Logika Pergantian State HIGH / LOW Berdasarkan Waktu Mikrodetik
  if (elapsed < onMicros) {
    if (!isPinHigh) {
      digitalWrite(LASER_PIN, HIGH);
      isPinHigh = true;
    }
  } else {
    if (isPinHigh) {
      digitalWrite(LASER_PIN, LOW);
      isPinHigh = false;
    }
  }

  // 2. Transisi ke Siklus Berikutnya (Mencegah Drift/Pergeseran Waktu)
  if (elapsed >= periodMicros) {
    lastCycleStart += periodMicros; // Sinkronisasi tepat pada kelipatan periode
  }

  // 3. Pembacaan Serial Input
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    int hzEndIndex = input.indexOf(',', 0);
    if (hzEndIndex != -1) {
      flashingRate = input.substring(0, hzEndIndex).toFloat();
      dutyCycle = input.substring(hzEndIndex + 1).toFloat();
      if (flashingRate <= 0.0f) flashingRate = 1.0f;
      updateTimings();

      Serial.print("Laser set to "); Serial.print(flashingRate, 2);
      Serial.print(" Hz and "); Serial.print(dutyCycle, 1); Serial.println("%");
    }
  }
}

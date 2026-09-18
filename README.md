# 🔬 Embedded Systems & IoT Engineering Lab

A centralized monorepo containing microcontroller engineering projects, high-precision signal instrumentation, and Internet of Things (IoT) systems developed for AVR (Arduino Uno & Mega 2560) and Espressif ESP32 platforms.

---

## 📂 Project Directory

| Project Folder | Summary | Target Hardware | Protocols & Peripherals |
| :--- | :--- | :--- | :--- |
| [**`Boyolali_IoT/`**](./Boyolali_IoT) | Dual-station microclimate telemetry system (Indoor & Outdoor) with battery optimization and real-time LCD monitoring. | ESP32 (Lolin D32 & DevKit) | WiFiMulti, ThingSpeak REST API, DHT22, ESP32 Deep Sleep |
| [**`LLT/`**](./LLT) | Precision laser pulse generator (zero-drift) and high-speed frequency/duty cycle analyzer utilizing hardware register interrupts. | Arduino Uno & Mega 2560 | Hardware Interrupt (PE4), Direct Register I/O, UART Serial |
| [**`Flexiforce/`**](./Flexiforce) | Piezoresistive tactile force sensing instrumentation with Exponential Moving Average (EMA) digital filtering and 5-LED bargraph. | Arduino Uno / Nano | Analog ADC, TUR-AMP Op-Amp, EMA Filter, 5-LED Bargraph |
| [**`Other/`**](./Other) | Miscellaneous prototyping experiments, peripheral checks, and buzzer tone sequences. | Arduino Uno / Nano | Digital I/O, PWM Tone Generation |

---

## 🗂️ Directory Layout

```text
.
├── .gitignore                      # Git exclusion rules (libraries, build artifacts, secrets)
├── README.md                       # Monorepo overview and navigation
│
├── Boyolali_IoT/                   # IoT Environmental Monitoring Station
│   ├── README.md                   # Station documentation, pinouts & cloud setup
│   ├── Indoor/
│   │   ├── Indoor.ino              # Low-power indoor sensing firmware
│   │   ├── secrets.h.example       # Template for credentials
│   │   ├── .env.example            # Environment variables template
│   │   └── Validation_Test/
│   └── Outdoor/
│       └── Outdoor/
│           ├── Outdoor.ino         # Continuous outdoor sensing & LCD display
│           ├── secrets.h.example   # Template for credentials
│           └── .env.example        # Environment variables template
│
├── LLT/                            # Laser Pulse Generator & Signal Checker
│   ├── README.md                   # Architecture, ISR mechanics & serial commands
│   ├── sketch_sep15a_ArduinoMega_Checker/
│   │   └── sketch_sep15a_ArduinoMega_Checker.ino
│   └── sketch_sep15b_ArduinoUno_PulseCreator/
│       └── sketch_sep15b_ArduinoUno_PulseCreator.ino
│
├── Flexiforce/                     # Tactile Force Sensing Instrumentation
│   ├── README.md                   # Calibration, EMA filter theory & circuit setup
│   └── Flexiforce.ino              # Force acquisition & bargraph firmware
│
└── Other/                          # Miscellaneous & Experimental Sketches
    ├── README.md                   # Descriptions of test sketches
    ├── sketch_sep17a/
    └── sketch_sep17c/
```

---

## 🚀 Getting Started

### 1. Software Requirements
* **[Arduino IDE 2.x](https://www.arduino.cc/en/software)**
* **Board Core Packages**:
  * `Arduino AVR Boards` (for Arduino Uno and Mega 2560)
  * `esp32 by Espressif Systems` (for ESP32 boards)

### 2. Dependency Libraries
Install the following libraries via the **Arduino Library Manager** (`Sketch` -> `Include Library` -> `Manage Libraries...`):
* `WiFiManager` by tzapu
* `ThingSpeak` by MathWorks
* `DHT sensor library` by Adafruit
* `Adafruit Unified Sensor` by Adafruit
* `LiquidCrystal` (Standard Arduino library)

### 3. Security & Credentials Configuration
To protect sensitive credentials (Wi-Fi passwords, ThingSpeak API keys):
1. In each station folder under `Boyolali_IoT/`, copy `secrets.h.example` to `secrets.h`:
   ```bash
   cp secrets.h.example secrets.h
   ```
2. Populate `secrets.h` with your actual Wi-Fi SSID, password, and ThingSpeak Write API Key.
3. Both `secrets.h` and `.env` are listed in `.gitignore` and will never be tracked or committed to GitHub.

---

## 📄 License & Maintainer
Maintained as an embedded systems research and development repository.

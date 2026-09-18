# 🌦️ Boyolali IoT - Dual-Station Microclimate & Telemetry System

An Internet of Things (IoT) microclimate monitoring system (temperature & humidity) consisting of two distinct stations: **Indoor Station** (low-power, battery-operated with ESP32 Deep Sleep) and **Outdoor Station** (continuous real-time telemetry with parallel LCD display). Both stations ingest environmental data into the **ThingSpeak** cloud analytics platform.

---

## 📌 Key Features

- **Dual-Station Architecture**:
  - **Indoor Station**: Employs two redundant DHT22 sensors for high-accuracy cross-validation, integrated Lolin D32 battery voltage sensing, and **ESP32 Deep Sleep** (5-minute duty cycle) for long-term battery deployment.
  - **Outdoor Station**: Real-time continuous sampling with a **16x2 Parallel LCD** showing current environmental readings and Wi-Fi connection status, dispatching telemetry packets every 30 seconds.
- **Robust Wi-Fi Failover (`WiFiMulti`)**: Configured with a prioritized list of Wi-Fi Access Points, automatic failover reconnect, and max transmission power (`WIFI_POWER_19_5dBm`).
- **Data Calibration & Quality Control**: Software temperature and humidity offset compensations applied to raw readings before cloud transmission.
- **ThingSpeak Cloud Integration**: Automated metric streaming for remote graphs, field data analysis, and historical tracking.

---

## 🛠️ Hardware Specifications & Pinout

### 1. Indoor Station (`Indoor/Indoor.ino`)
* **Microcontroller**: ESP32 (Lolin D32)
* **Sensors**: 2x DHT22 (Temperature & Relative Humidity)
* **Power Source**: 3.7V Li-ion / LiPo Battery

| Component / Function | ESP32 GPIO | Description |
| :--- | :--- | :--- |
| **DHT22 Sensor 1** | `GPIO 17` | Primary temperature & humidity sensor |
| **DHT22 Sensor 2** | `GPIO 16` | Secondary / redundancy sensor |
| **Battery Voltage Sense** | `GPIO 35` | Built-in voltage divider ADC on Lolin D32 |

### 2. Outdoor Station (`Outdoor/Outdoor/Outdoor.ino`)
* **Microcontroller**: ESP32 DevKit
* **Sensor**: 1x DHT22
* **Display**: 16x2 Parallel LCD

| Component / LCD Pin | ESP32 GPIO | Description |
| :--- | :--- | :--- |
| **DHT22 Outdoor** | `GPIO 4` | Ambient weather sensor |
| **LCD RS** | `GPIO 19` | Register Select |
| **LCD Enable (E)** | `GPIO 23` | Enable operation signal |
| **LCD D4** | `GPIO 14` | Data bit 4 |
| **LCD D5** | `GPIO 27` | Data bit 5 |
| **LCD D6** | `GPIO 25` | Data bit 6 |
| **LCD D7** | `GPIO 26` | Data bit 7 |

---

## 📦 Required Arduino Libraries

Install the following libraries using the **Arduino Library Manager**:
- `WiFi` (Built-in ESP32 core)
- `WiFiMulti` (Built-in ESP32 core)
- `WiFiManager` by tzapu
- `ThingSpeak` by MathWorks
- `DHT sensor library` by Adafruit
- `Adafruit Unified Sensor` by Adafruit
- `LiquidCrystal` (Built-in Arduino library)

---

## 🔒 Security & Credentials Setup

Network and cloud credentials are kept strictly out of version control. To configure:

1. Create a `secrets.h` file from the provided template:
   ```bash
   cp secrets.h.example secrets.h
   ```
2. Or use the `.env` template for environment record keeping:
   ```bash
   cp .env.example .env
   ```
3. Update `secrets.h` with your actual network and ThingSpeak keys:
   ```cpp
   #define SECRET_CH_ID 1234567
   #define SECRET_WRITE_API_KEY "YOUR_THINGSPEAK_KEY"

   const WifiCredential WIFI_NETWORKS[] = {
       {"YOUR_PRIMARY_SSID", "YOUR_PRIMARY_PASSWORD"},
       {"YOUR_BACKUP_SSID", "YOUR_BACKUP_PASSWORD"}
   };
   ```

> ⚠️ **Security Warning**: `secrets.h` and `.env` are listed in `.gitignore`. Never hardcode plain-text production credentials into `.ino` files.

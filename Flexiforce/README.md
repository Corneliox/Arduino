# 🦾 FlexiForce A201 - Tactile Force Sensing & LED Bargraph

A piezoresistive tactile force instrumentation system utilizing the **Tekscan FlexiForce A201** sensor conditioned through a **TUR-AMP** drive circuit and visually mapped to a 5-level LED bargraph indicator.

---

## 📌 Key Features

- **Digital Signal Filtering (EMA)**: Implements an *Exponential Moving Average* filter (`ALPHA = 0.18`) to damp electronic noise and mechanical vibration without inducing significant phase delay.
- **Deterministic Non-Blocking Sampling**: Stable 50 Hz acquisition cycle (20 ms period) managed through `millis()` timers, leaving CPU headroom for peripherals.
- **Dynamic LED Bargraph Indicator**: Linearly scales calibrated force values (`ADC_MIN` to `ADC_MAX`) across a 5-stage sequential LED array.
- **Serial Telemetry**: Streams clean telemetry to the Arduino Serial Monitor / Serial Plotter at 115200 bps for real-time sensor response profiling.

---

## 🛠️ Hardware Specifications & Pinout

* **Microcontroller**: Arduino Uno / Nano (ATmega328P)
* **Sensor**: Tekscan FlexiForce A201
* **Signal Conditioning**: TUR-AMP Module

| Component / Function | Arduino Pin | Description |
| :--- | :--- | :--- |
| **TUR-AMP 'OUT'** | `Pin A0` | Conditioned analog voltage signal proportional to applied force |
| **LED Bar 1** | `Pin D2` | Bargraph stage 1 (via 220Ω resistor to GND) |
| **LED Bar 2** | `Pin D3` | Bargraph stage 2 (via 220Ω resistor to GND) |
| **LED Bar 3** | `Pin D4` | Bargraph stage 3 (via 220Ω resistor to GND) |
| **LED Bar 4** | `Pin D5` | Bargraph stage 4 (via 220Ω resistor to GND) |
| **LED Bar 5** | `Pin D6` | Bargraph stage 5 (via 220Ω resistor to GND) |

---

## ⚙️ Calibration

Adjust the calibration thresholds in [Flexiforce.ino](file:///c:/Users/Pongo/OneDrive/Documents/Arduino/Flexiforce/Flexiforce.ino#L15-L16) to match the mechanical force range of your specific setup:

```cpp
const int ADC_MIN = 5;   // Raw ADC baseline reading with no force applied
const int ADC_MAX = 50;  // Raw ADC reading under maximum desired load
```

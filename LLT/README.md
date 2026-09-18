# ⚡ LLT - Laser Pulse Generator & High-Speed Signal Analyzer

A precision digital pulse instrumentation system composed of two cooperating microcontroller units:
1. **Pulse Creator (Arduino Uno)**: Deterministic laser pulse generator with microsecond timing and zero-drift compensation.
2. **Signal Checker (Arduino Mega 2560)**: High-speed diagnostic signal capture unit utilizing direct hardware register inspection inside an Interrupt Service Routine (ISR) to compute real-time frequency and duty cycle.

---

## 📌 System Architecture & Interconnection

```text
+-----------------------+                    +-------------------------+
|      Arduino Uno      |                    |    Arduino Mega 2560    |
|   (Pulse Creator)     |                    |    (Signal Analyzer)    |
|                       |                    |                         |
|   Pin D11 (Output) ---+------------------->+ Pin D2 (PE4 Interrupt)  |
|   GND              ---+------------------->+ GND                     |
+-----------------------+                    +-------------------------+
```

---

## 🛠️ Module 1: Pulse Creator (`sketch_sep15b_ArduinoUno_PulseCreator`)
* **Board**: Arduino Uno (ATmega328P)
* **Output Pin**: Pin D11 (`LASER_PIN`)
* **Serial Baud Rate**: `9600 bps`

### Features:
* **Anti-Drift Timing Loop**: Employs `micros()` with deterministic cycle advancement (`lastCycleStart += periodMicros`), ensuring zero cumulative timing drift over prolonged operation.
* **Interactive Dynamic Configuration**:
  Accepts runtime updates via Serial Monitor in the format: `<Frequency_Hz>,<DutyCycle_Percent>`
  * Example: `30,50` -> 30 Hz at 50% duty cycle.
  * Example: `100,20` -> 100 Hz at 20% duty cycle.

---

## 🛠️ Module 2: Signal Checker (`sketch_sep15a_ArduinoMega_Checker`)
* **Board**: Arduino Mega 2560 (ATmega2560)
* **Input Pin**: Pin D2 (Port E Bit 4 / `PE4`)
* **Serial Baud Rate**: `115200 bps`

### Technical Highlights:
* **Direct Register Capture (Zero Overhead)**:
  Directly samples hardware register `PINE & 0x10` within `signalChangeISR`, eliminating `digitalRead()` overhead to reliably timestamp microsecond pulses.
* **Operating Modes**:
  - `MODE_RAW`: Real-time streaming of pulse periods and high durations.
  - `MODE_RECORD`: Statistical window analysis computing mean frequency, duty cycle, and total pulse volume across 2-second sampling windows.
* **Configurable Session Duration**:
  Controlled via `recordDurationSec` (0 = indefinite monitoring, >0 = automated termination upon deadline).

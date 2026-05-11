# 🤖 Gesture Recognition on ESP32-S3 using Edge Impulse (TinyML)

A complete **TinyML** pipeline that recognizes hand/wrist gestures in real time on an **ESP32-S3** microcontroller using an **MPU6050** accelerometer and a neural network model trained on **Edge Impulse**.

No cloud required at inference time — the model runs entirely **on-device** at the edge.

---

## 📋 Table of Contents

- [Project Overview](#-project-overview)
- [Hardware Required](#-hardware-required)
- [System Architecture](#-system-architecture)
- [Wiring Diagram](#-wiring-diagram)
- [Library Dependencies](#-library-dependencies)
- [Step-by-Step Workflow](#-step-by-step-workflow)
- [Project Structure](#-project-structure)
- [Edge Impulse Project](#-edge-impulse-project)
- [Troubleshooting](#-troubleshooting)
- [Future Improvements](#-future-improvements)

---

## 🔭 Project Overview

| Item | Detail |
|---|---|
| **Microcontroller** | ESP32-S3 |
| **Sensor** | MPU6050 (3-axis accelerometer + gyroscope) |
| **ML Platform** | Edge Impulse Studio |
| **Model Type** | Neural Network Classifier (spectral features) |
| **Inference** | On-device (TensorFlow Lite Micro via Edge Impulse SDK) |
| **Communication** | Serial (data collection) / Standalone (inference) |

**Gestures you can train (examples):**
- ✊ Shake / Flick
- ⬆️ Tilt Up / Tilt Down
- ↔️ Swipe Left / Swipe Right
- 🔄 Rotate / Circle
- 😶 Idle / No Gesture

---

## 🔧 Hardware Required

| Component | Qty |
|---|---|
| ESP32-S3 Development Board | 1 |
| MPU6050 Accelerometer + Gyroscope Module | 1 |
| Jumper Wires | 4 |
| USB Cable (data-capable) | 1 |
| Breadboard (optional) | 1 |

---

## 🏗 System Architecture

```
Phase 1 — Data Collection
  [ESP32-S3 + MPU6050]
      │  Serial CSV (accX, accY, accZ @ 10 Hz)
      ▼
  [Edge Impulse Data Forwarder]
      │
      ▼
  [Edge Impulse Studio — studio.edgeimpulse.com/studio/746585]
      │  Label gestures, design impulse, train NN model
      ▼
  [Export: Arduino Library (.zip)]

Phase 2 — Inference (On-Device)
  [ESP32-S3 + MPU6050]
      │  Raw accelerometer data
      ▼
  [TensorFlow Lite Micro (Edge Impulse SDK)]
      │
      ▼
  [Serial Monitor — Predicted Gesture + Confidence]
```

---

## 🔌 Wiring Diagram

### MPU6050 → ESP32-S3

| MPU6050 Pin | ESP32-S3 Pin | Description |
|---|---|---|
| VCC | 3.3V | Power (use 3.3V — not 5V) |
| GND | GND | Ground |
| SDA | GPIO 8 | I2C Data |
| SCL | GPIO 9 | I2C Clock |
| AD0 | GND | I2C Address = 0x68 |
| INT | (not used) | Interrupt (optional) |

> **Note:** Some ESP32-S3 boards use different default I2C pins. If `Wire.begin()` fails, call `Wire.begin(SDA_PIN, SCL_PIN)` with your board's actual pins.

---

## 📦 Library Dependencies

Install via **Arduino IDE → Tools → Manage Libraries**:

| Library | Purpose |
|---|---|
| `MPU6050` by Electronic Cats | Read raw accelerometer data |
| `Wire` (built-in) | I2C communication |
| **Edge Impulse Arduino Library** | Run the trained model (exported from EI Studio) |

---

## 🚀 Step-by-Step Workflow

### Step 1 — Set Up Arduino IDE for ESP32-S3

1. Open Arduino IDE → **File → Preferences**
2. Add to *Additional Boards Manager URLs*:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. **Tools → Board → Boards Manager** → search `esp32` → install **esp32 by Espressif Systems**
4. Select board: `ESP32S3 Dev Module`

---

### Step 2 — Install MPU6050 Library

**Tools → Manage Libraries** → search `MPU6050` → install **MPU6050 by Electronic Cats**

---

### Step 3 — Collect Training Data

1. Wire the MPU6050 to the ESP32-S3 (see wiring table above)
2. Open and upload `data_collection/code_1_accelerometer_code_esp32.ino`
3. Install the **Edge Impulse CLI**:
   ```bash
   npm install -g edge-impulse-cli
   ```
4. Run the data forwarder (replace COM port with yours):
   ```bash
   edge-impulse-data-forwarder --frequency 10
   ```
5. Log in to your Edge Impulse account when prompted
6. In **Edge Impulse Studio → Data Acquisition**, record samples for each gesture (minimum ~3 minutes per class recommended)

---

### Step 4 — Train the Model in Edge Impulse Studio

1. Go to [studio.edgeimpulse.com/studio/746585](https://studio.edgeimpulse.com/studio/746585)
2. **Impulse Design:**
   - Input: Time Series (window size ~2000ms, stride ~200ms)
   - Processing block: **Spectral Analysis**
   - Learning block: **Classification (Neural Network)**
3. **Spectral Features** → Save parameters → Generate features
4. **Classifier** → Start training (adjust epochs / learning rate as needed)
5. Check accuracy in the confusion matrix — aim for >90%

---

### Step 5 — Export & Deploy to ESP32-S3

1. **Deployment → Arduino Library** → Build → Download `.zip`
2. In Arduino IDE: **Sketch → Include Library → Add .ZIP Library** → select the downloaded file
3. Open example: **File → Examples → `<project>_inferencing` → nano_ble33_sense → nano_ble33_sense_accelerometer**
4. Adapt the example to your ESP32-S3 pin configuration
5. Upload to your board — open Serial Monitor at **115200 baud** to see live predictions

---

## 📁 Project Structure

```
Gesture-Recognition-on-ESP32-S3/
│
├── data_collection/
│   └── code_1_accelerometer_code_esp32.ino   # Streams CSV data for Edge Impulse
│
├── deployment/
│   └── gesture_inference_esp32s3.ino         # Inference sketch (add after export)
│
├── README.md
└── .gitignore
```

> **Note:** The Edge Impulse exported Arduino library (`.zip`) is not included in this repo as it is generated per-build. Download it directly from the Edge Impulse project link below.

---

## 🔗 Edge Impulse Project

| Item | Link |
|---|---|
| **Live Project** | [studio.edgeimpulse.com/studio/746585](https://studio.edgeimpulse.com/studio/746585) |
| **Data Forwarder Docs** | [docs.edgeimpulse.com/docs/tools/edge-impulse-cli/cli-data-forwarder](https://docs.edgeimpulse.com/docs/tools/edge-impulse-cli/cli-data-forwarder) |
| **Arduino Deployment Docs** | [docs.edgeimpulse.com/docs/deployment/arduino-library](https://docs.edgeimpulse.com/docs/deployment/arduino-library) |

---

## 🐛 Troubleshooting

| Problem | Fix |
|---|---|
| `MPU6050 ready` not printed | Check SDA/SCL wiring; run I2C scanner sketch to find address |
| Data forwarder can't connect | Check COM port; ensure Serial Monitor is closed before running CLI |
| Low model accuracy | Collect more samples; reduce gestures; adjust window/stride |
| Inference sketch won't compile | Ensure EI library is installed; select correct ESP32-S3 board |
| All predictions show "idle" | Threshold may be too high; check confidence scores in Serial Monitor |

---

## 🔮 Future Improvements

- [ ] Add gyroscope data (6-DOF) for richer gesture features
- [ ] Integrate more gesture classes
- [ ] Trigger actions via GPIO based on recognized gesture (e.g. LED, relay, BLE)
- [ ] Add OLED display to show predicted gesture on-device
- [ ] Deploy over BLE to a mobile app for wireless gesture control
- [ ] Benchmark model accuracy vs. model size / latency tradeoff

---

## 📄 License

This project is open-source under the [MIT License](LICENSE).

---

*Built with ❤️ using ESP32-S3, MPU6050, and Edge Impulse TinyML.*

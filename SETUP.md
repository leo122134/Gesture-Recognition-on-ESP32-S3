# ⚡ Quick Setup Guide — Gesture Recognition on ESP32-S3

A condensed step-by-step to get from hardware to running inference.  
See [README.md](README.md) for full architecture, wiring diagrams, and details.

---

## What You Need Before Starting

- ESP32-S3 development board
- MPU6050 accelerometer module
- 4 jumper wires
- USB cable (data-capable, not charge-only)
- Arduino IDE v2.x — [download here](https://www.arduino.cc/en/software)
- Node.js (for Edge Impulse CLI) — [download here](https://nodejs.org)
- Edge Impulse account — [sign up free](https://studio.edgeimpulse.com)

---

## Phase 1 — Data Collection

### Step 1 — Wire the MPU6050

| MPU6050 Pin | ESP32-S3 Pin |
|---|---|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 8 |
| SCL | GPIO 9 |
| AD0 | GND |

> ⚠️ Use **3.3V** — connecting VCC to 5V will damage the MPU6050.

---

### Step 2 — Add ESP32 Board Support

1. Arduino IDE → **File → Preferences**
2. Paste into *Additional Boards Manager URLs*:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
3. **Tools → Board → Boards Manager** → search `esp32` → install **esp32 by Espressif Systems**
4. Select: **Tools → Board → ESP32S3 Dev Module**

---

### Step 3 — Install MPU6050 Library

**Tools → Manage Libraries** → search `MPU6050` → install **MPU6050 by Electronic Cats**

---

### Step 4 — Upload the Data Collection Sketch

1. Open `data_collection/code_1_accelerometer_code_esp32.ino`
2. Select your COM port: **Tools → Port**
3. Click **Upload**
4. Open Serial Monitor at **115200 baud** — you should see numbers like:
   ```
   MPU6050 ready
   -312,8740,-1024
   -308,8756,-1020
   ```
   If you see this, your hardware is working correctly.

---

### Step 5 — Install Edge Impulse CLI

Open a terminal (Command Prompt / PowerShell) and run:

```bash
npm install -g edge-impulse-cli
```

Verify it installed:

```bash
edge-impulse-data-forwarder --version
```

---

### Step 6 — Stream Data to Edge Impulse

**Close the Arduino Serial Monitor first** — only one program can use the COM port at a time.

Then run:

```bash
edge-impulse-data-forwarder --frequency 10
```

- Log in with your Edge Impulse credentials when prompted
- Select your project: **746585** (or create a new one)
- Name your axes when asked: `accX, accY, accZ`

---

### Step 7 — Record Gesture Samples

1. Go to [studio.edgeimpulse.com/studio/746585](https://studio.edgeimpulse.com/studio/746585)
2. Open **Data Acquisition**
3. For each gesture you want to recognize:
   - Type a label (e.g. `shake`, `tilt_up`, `idle`)
   - Set sample length to **3000 ms**
   - Click **Start sampling** and perform the gesture
   - Collect at least **20–30 samples per gesture**

---

## Phase 2 — Train the Model

### Step 8 — Design the Impulse

In Edge Impulse Studio → **Impulse Design**:

| Block | Setting |
|---|---|
| Input | Time Series — window 2000 ms, stride 200 ms, freq 10 Hz |
| Processing | Spectral Analysis |
| Learning | Classification (Neural Network) |

Click **Save Impulse**.

---

### Step 9 — Generate Features & Train

1. **Spectral Features** → click **Save parameters** → **Generate features**
2. **Classifier** → click **Start training**
3. Wait for training to complete — check the confusion matrix
4. Aim for **>90% accuracy** — if lower, collect more samples or reduce gesture classes

---

## Phase 3 — Deploy to ESP32-S3

### Step 10 — Export Arduino Library

1. **Deployment** → select **Arduino Library**
2. Click **Build** → download the `.zip` file

---

### Step 11 — Install the Exported Library

In Arduino IDE:

**Sketch → Include Library → Add .ZIP Library** → select the downloaded `.zip`

---

### Step 12 — Open & Adapt the Inference Example

1. **File → Examples → `<your_project>_inferencing` → nano_ble33_sense → nano_ble33_sense_accelerometer**
2. The example targets Arduino Nano BLE — adapt it for ESP32-S3:
   - Replace `IMU.begin()` / `IMU.readAcceleration()` with MPU6050 calls
   - Replace `IMU.accelerationSampleRate()` with your sample rate (`10.0f`)
3. Save the adapted sketch into the `deployment/` folder as `gesture_inference_esp32s3.ino`
4. Upload to your ESP32-S3

---

### Step 13 — Test Live Inference

Open Serial Monitor at **115200 baud**. You should see output like:

```
Predictions:
  idle:    0.02
  shake:   0.95
  tilt_up: 0.03

Gesture detected: shake (95.0% confidence)
```

Perform each gesture and verify the correct label is predicted.

---

## Troubleshooting

| Problem | Fix |
|---|---|
| Serial Monitor shows nothing | Check baud rate is 115200; re-upload sketch |
| MPU6050 returns all zeros | Check SDA/SCL wiring; run I2C scanner to find address |
| Data forwarder port busy | Close Arduino Serial Monitor before running CLI |
| Low training accuracy | Add more samples; check for mislabelled data |
| Inference sketch won't compile | Make sure the EI `.zip` library is installed |
| All predictions show `idle` | Adjust confidence threshold or collect more varied samples |

---

## Quick Reference — Key Links

| Resource | Link |
|---|---|
| Edge Impulse Project | [studio.edgeimpulse.com/studio/746585](https://studio.edgeimpulse.com/studio/746585) |
| Edge Impulse CLI Docs | [docs.edgeimpulse.com/docs/tools/edge-impulse-cli](https://docs.edgeimpulse.com/docs/tools/edge-impulse-cli) |
| Arduino Deployment Guide | [docs.edgeimpulse.com/docs/deployment/arduino-library](https://docs.edgeimpulse.com/docs/deployment/arduino-library) |
| ESP32 Boards Manager | [github.com/espressif/arduino-esp32](https://github.com/espressif/arduino-esp32) |

#include <Wire.h>
#include <MPU6050.h>
#include <leo_16572-project-1_inferencing.h>  // Your model header

MPU6050 mpu;
ei_impulse_result_t result;
float features[3];  // Update if your model input uses more

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // ESP32-WROOM default I²C pins

  mpu.initialize();
  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed");
    while (1);
  }

  Serial.println("Gesture detection ready 🎬");
}

void loop() {
  int16_t ax, ay, az;
  mpu.getAcceleration(&ax, &ay, &az);

  // Format or scale based on your Edge Impulse training input
  features[0] = ax;
  features[1] = ay;
  features[2] = az;

  signal_t signal;
  signal.total_length = sizeof(features) / sizeof(float);
  signal.get_data = [](size_t offset, float *value) -> int {
    *value = features[offset];
    return 0;
  };

  EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);
  if (res != EI_IMPULSE_OK) {
    Serial.println("Model inference failed");
    return;
  }

  Serial.println("→ Predicted gesture:");
  for (size_t i = 0; i < result.classification_count; i++) {
    Serial.print(result.classification[i].label);
    Serial.print(" = ");
    Serial.println(result.classification[i].value);
  }

  delay(500);  // Adjust based on gesture sampling needs
}

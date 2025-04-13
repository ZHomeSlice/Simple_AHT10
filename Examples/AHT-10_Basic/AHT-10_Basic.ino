#include <Simple_AHT10.h>

#define SDA_PIN 14
#define SCL_PIN 15

Simple_AHT10 aht(AHT10_ADDR_GND);  // Default address = 0x38

// Callback function to handle measurement results
void SensorCallback(float temp, float hum) {
  Serial.printf("Temp: %.2f °C, Hum: %.2f %%\n", temp, hum);
}

void setup() {
  Serial.begin(115200);
  while (!Serial);  // Wait for Serial monitor (USB)

  // Initialize I2C communication
  aht.begin(SDA_PIN, SCL_PIN);

  // Set the callback function to receive data
  aht.SetCallbackFunction(SensorCallback);

  // Start automatic measurements every 2000 ms
  aht.TriggerMeasurement(2000);
}

void loop() {
  aht.loop();  // Handle non-blocking logic
}

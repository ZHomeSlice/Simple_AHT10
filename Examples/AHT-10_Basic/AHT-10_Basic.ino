/* ============================================
  Simple_AHT-10 device library code is placed under the MIT license
  Copyright (c) 2024 Homer Creutz

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT, OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
  ===============================================
*/

#include <Simple_AHT10.h>

#define SDA_PIN 14
#define SCL_PIN 15

Simple_AHT10 aht;  // Default address = 0x38

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
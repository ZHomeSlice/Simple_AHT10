# 🌡️ Simple_AHT10

**Simple_AHT10** is a lightweight, macro-driven, non-blocking library for AHT10 and AHT20 temperature/humidity sensors.  
Built on top of the [Simple_Wire](https://github.com/YourRepo/Simple_Wire) I²C communication library, it's designed for **performance**, **portability**, and **ease of use** across a range of platforms.

---

## 🚀 Features

- ✅ **Non-blocking measurements** using `loop()`
- ✅ **Callback-based API** for real-time sensor updates
- ✅ **Supports AHT10 & AHT20** sensors
- ✅ **I²C auto-scan and address configuration**
- ✅ **Fully compatible with ESP32-WROOM-32U**, Arduino UNO, STM32, and more
- ✅ **No `std::function` or dynamic memory** — safe for memory-limited MCUs

---

## 🧰 Usage Example (ESP32)

```cpp
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
```

---

## 📁 Contents

| File               | Description                      |
|--------------------|----------------------------------|
| `Simple_AHT10.h`   | Core AHT10/AHT20 sensor logic    |
| `Simple_Wire.h`    | Base I²C macro utility library   |
| `examples/`        | Ready-to-run Arduino sketches    |

---

## 🛠 Platforms Tested

- ✅ ESP32-WROOM-32U
- ✅ Arduino UNO (no dynamic memory used)
- ✅ STM32F103 "Blue Pill"
- ✅ RP2040 (Raspberry Pi Pico)

---

## 📘 License

This library is released under the **MIT License**.  
Feel free to use, modify, and contribute!

---

## 👷 Maintained by

**Homer Creutz**  
Open for contributions and collaboration!

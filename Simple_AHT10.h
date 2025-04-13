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

#ifndef Simple_AHT10_h
#define Simple_AHT10_h

#include "Simple_Wire.h"


#define SIMPLE_AHT10_ADDR_GND      0x38
#define SIMPLE_AHT10_ADDR_VCC      0x39

#define AHT_CMD_INIT_AHT10         0xE1
#define AHT_CMD_INIT_AHT20         0xBE
#define AHT_CMD_START_MEASURE      0xAC
#define AHT_CMD_NORMAL_MODE        0xA8
#define AHT_CMD_SOFT_RESET         0xBA

#define AHT_MODE_NORMAL            0x00
#define AHT_MODE_CYCLE             0x20
#define AHT_MODE_COMMAND           0x40
#define AHT_MODE_CAL_ENABLE        0x08
#define AHT_MEASUREMENT_PARAM      0x33
#define AHT_NOP                    0x00

#define AHT_DELAY_MEASURE_MS       80
#define AHT_DELAY_POWER_ON_MS      40
#define AHT_DELAY_CMD_MS           350
#define AHT_DELAY_RESET_MS         20

#define AHT_FORCE_READ             true
#define AHT_USE_CACHED_DATA        false
#define AHT_ERROR_CODE             0xFF

typedef void (*AHTCallback)(float temperature, float humidity);

class Simple_AHT10 : public Simple_Wire {
public:
    enum SensorType {
        SENSOR_AHT10 = 0,
        SENSOR_AHT20 = 1
    };

    uint8_t rawData[6] = {0};
    SensorType sensorType = SENSOR_AHT10;

    Simple_AHT10();
    Simple_AHT10(uint8_t address, SensorType type = SENSOR_AHT10);

    Simple_AHT10 &begin(int sdaPin, int sclPin);
    uint8_t ReadRawData();
    float ReadTemperature(bool forceRead = AHT_FORCE_READ);
    float ReadHumidity(bool forceRead = AHT_FORCE_READ);
    bool SoftReset();
    bool SetNormalMode();
    bool SetCycleMode();
    bool EnableFactoryCalCoeff();
    uint8_t ReadStatusByte();
    uint8_t GetCalibrationBit(bool forceRead = AHT_FORCE_READ);
    uint8_t GetBusyBit(bool forceRead = AHT_FORCE_READ);
    Simple_AHT10 &SetCallbackFunction(AHTCallback cb);
    void TriggerMeasurement(int32_t every = -1);
    void DisableAutoTrigger();
    bool IsBusy();
    void loop();

private:
    enum State { IDLE, TRIGGERED, WAITING_FOR_DATA };
    State currentState = IDLE;
    unsigned long lastActionTime = 0;
    uint16_t measurementDelayMs = AHT_DELAY_MEASURE_MS;
    AHTCallback callback = nullptr;
    uint32_t TriggerDelay = 1000;
    uint32_t LastTriggerDelayTime = 0;

    void MaybeAutoTrigger();
    void CheckMeasurementReady();
};

#endif

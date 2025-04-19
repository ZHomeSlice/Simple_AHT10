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

#include "Simple_AHT10.h"

Simple_AHT10::Simple_AHT10() {
    SetAddress(SIMPLE_AHT10_ADDR_GND);
}

Simple_AHT10::Simple_AHT10(uint8_t address, SensorType type) {
    SetAddress(address);
    sensorType = type;
}

Simple_AHT10 &Simple_AHT10::begin(int sdaPin, int sclPin) {
    Simple_Wire::begin(sdaPin, sclPin);
    Delay(AHT_DELAY_POWER_ON_MS);
    return *this;
}

uint8_t Simple_AHT10::ReadRawData() {
    uint8_t cmdBuf[2] = { AHT_MEASUREMENT_PARAM, AHT_NOP };
    WriteBytes(AHT_CMD_START_MEASURE, 2, cmdBuf);

    uint8_t status = 0;
    ReadByte(0x00, &status);
    if ((status & 0x08) == 0) return AHT_ERROR_CODE;
    if (status & 0x80) Delay(AHT_DELAY_MEASURE_MS);

    ReadBytes(0x00, 6, rawData);
    if (ReadCount() != 6) {
        rawData[0] = AHT_ERROR_CODE;
        return AHT_ERROR_CODE;
    }

    return true;
}

float Simple_AHT10::ReadTemperature(bool forceRead) {
    if (forceRead) {
        if (ReadRawData() == AHT_ERROR_CODE) return AHT_ERROR_CODE;
    }

    if (rawData[0] == AHT_ERROR_CODE) return AHT_ERROR_CODE;

    uint32_t tempRaw = ((uint32_t)(rawData[3] & 0x0F) << 16) |
                       ((uint16_t)rawData[4] << 8) |
                       rawData[5];

    return (float)tempRaw * 0.000191f - 50.0f;
}

float Simple_AHT10::ReadHumidity(bool forceRead) {
    if (forceRead) {
        if (ReadRawData() == AHT_ERROR_CODE) return AHT_ERROR_CODE;
    }

    if (rawData[0] == AHT_ERROR_CODE) return AHT_ERROR_CODE;

    uint32_t raw = (((uint32_t)rawData[1] << 16) |
                    ((uint16_t)rawData[2] << 8) |
                    rawData[3]) >> 4;

    float humidity = (float)raw * 0.000095f;
    if (humidity < 0.0f) return 0.0f;
    if (humidity > 100.0f) return 100.0f;
    return humidity;
}

bool Simple_AHT10::SoftReset() {
    if (!WriteByte(AHT_CMD_SOFT_RESET, 0x00).Success())
        return false;
    Delay(AHT_DELAY_RESET_MS);
    return SetNormalMode() && EnableFactoryCalCoeff();
}

bool Simple_AHT10::SetNormalMode() {
    uint8_t cmdBuf[2] = { AHT_NOP, AHT_NOP };
    if (!WriteBytes(AHT_CMD_NORMAL_MODE, 2, cmdBuf).Success())
        return false;
    Delay(AHT_DELAY_CMD_MS);
    return true;
}

bool Simple_AHT10::SetCycleMode() {
    uint8_t initCmd = (sensorType == SENSOR_AHT20) ? AHT_CMD_INIT_AHT20 : AHT_CMD_INIT_AHT10;
    uint8_t buf[2] = { AHT_MODE_CYCLE | AHT_MODE_CAL_ENABLE, AHT_NOP };
    return WriteBytes(initCmd, 2, buf).Success();
}

bool Simple_AHT10::EnableFactoryCalCoeff() {
    uint8_t initCmd = (sensorType == SENSOR_AHT20) ? AHT_CMD_INIT_AHT20 : AHT_CMD_INIT_AHT10;
    uint8_t buf[2] = { AHT_MODE_CAL_ENABLE, AHT_NOP };
    if (!WriteBytes(initCmd, 2, buf).Success())
        return false;
    Delay(AHT_DELAY_CMD_MS);
    return (GetCalibrationBit(AHT_FORCE_READ) == 0x01);
}

uint8_t Simple_AHT10::ReadStatusByte() {
    uint8_t status = 0;
    if (!ReadByte(0x00, &status).Success())
        return AHT_ERROR_CODE;
    return status;
}

uint8_t Simple_AHT10::GetCalibrationBit(bool forceRead) {
    if (forceRead)
        rawData[0] = ReadStatusByte();
    if (rawData[0] != AHT_ERROR_CODE)
        return (rawData[0] >> 3) & 0x01;
    return AHT_ERROR_CODE;
}

uint8_t Simple_AHT10::GetBusyBit(bool forceRead) {
    if (forceRead)
        rawData[0] = ReadStatusByte();
    if (rawData[0] != AHT_ERROR_CODE)
        return (rawData[0] >> 7) & 0x01;
    return AHT_ERROR_CODE;
}

Simple_AHT10 &Simple_AHT10::SetCallbackFunction(AHTCallback cb) {
    callback = cb;
    return *this;
}

void Simple_AHT10::TriggerMeasurement(int32_t everyXms) {
    if (everyXms >= 0) TriggerDelay = (uint32_t)abs(everyXms);
    currentState = TRIGGERED;
    lastActionTime = millis();
    uint8_t cmdBuf[2] = { AHT_MEASUREMENT_PARAM, AHT_NOP };
    if(!WriteBytes(AHT_CMD_START_MEASURE, 2, cmdBuf).WriteSucess()){
        currentState = ERROR_DETECTED;
    }

}

Simple_AHT10 &Simple_AHT10::DisableAutoTrigger() {
    TriggerDelay = 0;
    currentState = IDLE;
    return *this;
}

Simple_AHT10 &Simple_AHT10::SetTriggerDelay(int32_t everyXms) {
    if (everyXms >= 0) TriggerDelay = (uint32_t)abs(everyXms);
    return *this;
}
Simple_AHT10 &Simple_AHT10::SetRetryDelay(uint32_t everyXms) {
    RetryDelay = everyXms;
    return *this;
}

bool Simple_AHT10::IsBusy() {
    return currentState != IDLE;
}

void Simple_AHT10::MaybeAutoTrigger() {
    if (TriggerDelay && currentState == IDLE &&  (millis() - LastTriggerDelayTime >= TriggerDelay)) {
        TriggerMeasurement();
    }
}

void Simple_AHT10::CheckMeasurementReady() {
    if (currentState == WAITING_FOR_DATA && (millis() - lastActionTime >= measurementDelayMs)) {
        if (ReadBytes(0x00, 6, rawData).ReadSuccess()) {
            float temp = ReadTemperature(AHT_USE_CACHED_DATA);
            float hum  = ReadHumidity(AHT_USE_CACHED_DATA);
            if (callback) callback(temp, hum);
            LastTriggerDelayTime = millis();
        } else {
            currentState = ERROR_DETECTED;
            return;  
        }
        currentState = IDLE;
    }
}

void Simple_AHT10::loop() {
    if (currentState == ERROR_DETECTED) {
        for (static unsigned long Timer; (unsigned long)(millis() - Timer) >= (RetryDelay); Timer = millis()){ // if an error is detected we will try again emediatly then wait for 2 seconds if error repeats
            currentState = IDLE; 
        }
        return;
    }
    if (currentState == TRIGGERED) {
        currentState = WAITING_FOR_DATA;
        lastActionTime = millis();
        return;
    }
    CheckMeasurementReady();
    MaybeAutoTrigger();
}

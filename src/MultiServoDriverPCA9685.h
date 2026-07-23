#pragma once
#include <Arduino.h>
#include <Wire.h>

#include "MultiServoDriver.h"

// MultiServoDriverPCA9685
class MultiServoDriverPCA9685 : public MultiServoDriver {
   public:
    explicit MultiServoDriverPCA9685(uint8_t address = 0x40, TwoWire& wire = Wire) : _wire(wire), _addr(address) {}

    // подключен
    bool isConnected() {
        _wire.beginTransmission(_addr);
        return _wire.endTransmission() == 0;
    }

    // запустить
    bool begin(uint16_t freq = 50) {
        return isConnected() &&
               _writeReg(MODE1, MODE1_AI) &&
               _writeReg(MODE2, MODE2_OUTDRV) &&
               setPWMFrequency(freq) &&
               disable();
    }

    // отключить выходы и перевести PCA9685 в режим сна
    bool end() {
        uint8_t mode;
        if (!_readReg(MODE1, mode)) return false;

        if (!(mode & MODE1_SLEEP)) {
            if (!_writeReg(ALL_LED_OFF_H, FULL_BIT >> 8)) return false;
            if (!_writeReg(MODE1, mode | MODE1_SLEEP)) return false;
        }

        _pwmF = 0;
        return true;
    }

    // установить частоту ШИМ в Гц
    bool setPWMFrequency(uint16_t freq) {
        if (freq == 0 || _oscF == 0) return false;

        uint32_t divider = 4096UL * (uint32_t)freq;
        uint32_t prescale = (_oscF + divider / 2UL) / divider;

        if (prescale > 0) --prescale;
        prescale = constrain(prescale, 3, 255);

        uint8_t oldMode;
        if (!_readReg(MODE1, oldMode)) return false;

        uint8_t sleepMode = (oldMode & ~MODE1_RESTART) | MODE1_SLEEP;
        uint8_t wakeMode = oldMode & ~(MODE1_SLEEP | MODE1_RESTART);

        if (!_writeReg(MODE1, sleepMode) ||
            !_writeReg(PRE_SCALE, prescale) ||
            !_writeReg(MODE1, wakeMode)) return false;

        delayMicroseconds(500);
        if (!_writeReg(MODE1, wakeMode | MODE1_RESTART | MODE1_AI)) return false;

        uint32_t actualDivider = 4096UL * (prescale + 1UL);
        _pwmF = (_oscF + actualDivider / 2UL) / actualDivider;
        return true;
    }

    // получить реальную частоту ШИМ
    uint16_t getPWMFrequency() const {
        return _pwmF;
    }

    // установить измеренную частоту генератора (умолч. 25000000) - после неё нужно перенастроить частоту ШИМ
    void setOscFreq(uint32_t freq) {
        _oscF = freq;
    }

    // получить установленную частоту генератора
    uint32_t getOscFreq() const {
        return _oscF;
    }

    // записать микросекунды
    bool writeMicroseconds(uint8_t channel, uint16_t us) override {
        if (us == 0) return disable(channel);

        uint32_t value = (uint32_t)us * _pwmF;
        uint32_t ticks = value / 1000000UL * 4096UL;
        ticks += (value % 1000000UL * 4096UL + 500000UL) / 1000000UL;
        if (ticks == 0) ticks = 1;
        if (ticks > 4095) return false;
        return _writePWM(channel, ticks);
    }

    // записать угол или микросекунды
    bool write(uint8_t channel, uint16_t angle, uint16_t minUs = MinPulse, uint16_t maxUs = MaxPulse, uint16_t maxAngle = 180) {
        return writeMicroseconds(channel, angleToUs(angle, minUs, maxUs, maxAngle));
    }

    // прочитать микросекунды
    int32_t readMicroseconds(uint8_t channel) {
        if (channel >= 16 || _pwmF == 0) return -1;

        uint8_t data[2];
        uint8_t reg = LED0_OFF_L + channel * 4;

        if (!_beginRead(reg, 2)) return -1;

        data[0] = _wire.read();
        data[1] = _wire.read();

        if (data[1] & 0x10) return 0;

        uint16_t ticks = (uint16_t)data[0] | ((uint16_t)(data[1] & 0x0F) << 8);
        uint32_t divider = 4096UL * _pwmF;

        return ((uint32_t)ticks * 1000000UL + divider / 2UL) / divider;
    }

    // отключить канал
    bool disable(uint8_t channel) {
        return _writePWM(channel, FULL_BIT);
    }

    // отключить все каналы
    bool disable() {
        if (_pwmF == 0) return false;

        _wire.beginTransmission(_addr);
        _wire.write(ALL_LED_ON_L);
        _wire.write((uint8_t)0);
        _wire.write((uint8_t)0);
        _wire.write((uint8_t)0);
        _wire.write((uint8_t)(FULL_BIT >> 8));
        return _wire.endTransmission(true) == 0;
    }

   private:
    static const uint8_t MODE1 = 0x00;
    static const uint8_t MODE2 = 0x01;
    static const uint8_t LED0_ON_L = 0x06;
    static const uint8_t LED0_OFF_L = 0x08;
    static const uint8_t ALL_LED_ON_L = 0xFA;
    static const uint8_t ALL_LED_OFF_H = 0xFD;
    static const uint8_t PRE_SCALE = 0xFE;

    static const uint8_t MODE1_RESTART = 0x80;
    static const uint8_t MODE1_AI = 0x20;
    static const uint8_t MODE1_SLEEP = 0x10;
    static const uint8_t MODE2_OUTDRV = 0x04;

    static const uint16_t FULL_BIT = 0x1000;

    TwoWire& _wire;
    uint32_t _oscF = 25000000UL;
    uint16_t _pwmF = 0;
    uint8_t _addr;

    bool _writePWM(uint8_t channel, uint16_t off) {
        if (channel >= 16 || _pwmF == 0) return false;

        _wire.beginTransmission(_addr);
        _wire.write((uint8_t)(LED0_ON_L + channel * 4));
        _wire.write((uint8_t)0);                    // ON_L
        _wire.write((uint8_t)0);                    // ON_H
        _wire.write((uint8_t)(off & 0xFF));         // OFF_L
        _wire.write((uint8_t)((off >> 8) & 0x1F));  // OFF_H
        return _wire.endTransmission(true) == 0;
    }

    bool _writeReg(uint8_t reg, uint8_t value) {
        _wire.beginTransmission(_addr);
        _wire.write(reg);
        _wire.write(value);
        return _wire.endTransmission(true) == 0;
    }

    bool _beginRead(uint8_t reg, uint8_t len = 1) {
        _wire.beginTransmission(_addr);
        _wire.write(reg);
        return _wire.endTransmission(false) == 0 &&
               _wire.requestFrom(_addr, len) == len;
    }

    bool _readReg(uint8_t reg, uint8_t& value) {
        if (!_beginRead(reg)) return false;
        value = _wire.read();
        return true;
    }
};

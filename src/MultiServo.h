#pragma once
#include "MultiServoDriver.h"

// MultiServo
class MultiServo {
   public:
    MultiServo(MultiServoDriver& drv) : _drv(drv) {}

    bool attach(uint8_t channel, uint16_t pulse = MultiServoDriver::DefPulse) {
        detach();
        if (!_drv.writeMicroseconds(channel, pulse)) return false;

        _ch = channel;
        _us = pulse;
        return true;
    }

    bool attach(uint8_t channel, uint16_t minUs, uint16_t maxUs) {
        _minUs = minUs;
        _maxUs = maxUs;
        return attach(channel);
    }

    void detach() {
        if (attached()) _drv.writeMicroseconds(_ch, 0);
        _ch = -1;
    }

    bool writeMicroseconds(uint16_t value) {
        if (value < _minUs) value = _minUs;
        else if (value > _maxUs) value = _maxUs;
        if (!attached() || !_drv.writeMicroseconds(_ch, value)) return false;
        _us = value;
        return true;
    }

    uint16_t readMicroseconds() const {
        return _us;
    }

    bool write(uint16_t value, uint16_t maxAngle = 180) {
        return writeMicroseconds(MultiServoDriver::angleToUs(value, _minUs, _maxUs, maxAngle));
    }

    uint16_t read(uint16_t maxAngle = 180) const {
        return MultiServoDriver::usToDeg(readMicroseconds(), _minUs, _maxUs, maxAngle);
    }

    bool attached() const {
        return _ch >= 0;
    }

   private:
    MultiServoDriver& _drv;
    uint16_t _minUs = MultiServoDriver::MinPulse;
    uint16_t _maxUs = MultiServoDriver::MaxPulse;
    uint16_t _us = 0;
    int8_t _ch = -1;
};

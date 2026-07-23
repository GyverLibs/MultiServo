#pragma once
#include <stdint.h>

// MultiServoDriver
class MultiServoDriver {
   public:
    static constexpr uint16_t MinPulse = 544;
    static constexpr uint16_t MaxPulse = 2400;
    static constexpr uint16_t DefPulse = 1500;

    // utils
    static uint16_t degToUs(uint16_t angle, uint16_t minUs = MinPulse, uint16_t maxUs = MaxPulse, uint16_t maxAngle = 180) {
        if (maxUs <= minUs || !maxAngle) return DefPulse;
        if (angle > maxAngle) angle = maxAngle;
        return minUs + (uint32_t)(maxUs - minUs) * angle / maxAngle;
    }

    static uint16_t usToDeg(uint16_t us, uint16_t minUs = MinPulse, uint16_t maxUs = MaxPulse, uint16_t maxAngle = 180) {
        if (maxUs <= minUs || !maxAngle) return 0;
        if (us <= minUs) return 0;
        if (us >= maxUs) return maxAngle;
        return (uint32_t)(us - minUs) * maxAngle / (maxUs - minUs);
    }

    static uint16_t angleToUs(uint16_t angle, uint16_t minUs = MinPulse, uint16_t maxUs = MaxPulse, uint16_t maxAngle = 180) {
        return angle <= maxAngle ? degToUs(angle, minUs, maxUs, maxAngle) : angle;
    }

    // DriverProxy
    class DriverProxy {
       public:
        DriverProxy(uint8_t channel, MultiServoDriver& drv) : _drv(drv), _ch(channel) {}

        // записать микросекунды. 0 отключает канал
        bool writeMicroseconds(uint16_t us) {
            return _drv.writeMicroseconds(_ch, us);
        }

        // записать угол или микросекунды
        bool write(uint16_t angle, uint16_t minUs = MinPulse, uint16_t maxUs = MaxPulse, uint16_t maxAngle = 180) {
            return writeMicroseconds(angleToUs(angle, minUs, maxUs, maxAngle));
        }

       private:
        MultiServoDriver& _drv;
        uint8_t _ch;
    };

    // доступ через []
    DriverProxy operator[](uint8_t channel) {
        return DriverProxy(channel, *this);
    }

    // записать микросекунды. 0 отключает канал
    virtual bool writeMicroseconds(uint8_t channel, uint16_t us) = 0;
};

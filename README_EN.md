This is an automatic translation and may be incorrect in some places. See the source README and examples for authoritative information.

[![latest](https://img.shields.io/github/v/release/GyverLibs/MultiServo.svg?color=brightgreen)](https://github.com/GyverLibs/MultiServo/releases/latest/download/MultiServo.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/MultiServo.svg)](https://registry.platformio.org/libraries/gyverlibs/MultiServo)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/MultiServo?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# MultiServo
Library for servo drive control via PCA9685

### Compatibility
Compatible with all Arduino platforms (Arduino features are used)

## Contents
- [Use of use](#usage)
- [Versions](#versions)
- [Installation](#install)
- [Bugs and feedback](#feedback)

<a id="usage"></a>

## Use of use
### MultiServoDriver
The basic class of the driver, on its basis you can make support for other drivers

```cpp
class DriverProxy {
    // write down microseconds. 0 shuts down the channel
    bool writeMicroseconds(uint16_t us);

    // record an angle or microsecond
    bool write(uint16_t angle, uint16_t minUs = MinPulse, uint16_t maxUs = MaxPulse, uint16_t maxAngle = 180);
};

// access through []
DriverProxy operator[](uint8_t channel)
```

### MultiServoDriverPCA9685
Class to work with the driver PCA9685, inherits`MultiServoDriver`:

```cpp
MultiServoDriverPCA9685(uint8_t address = 0x40, TwoWire& wire = Wire) : _wire(wire), _addr(address) {}

// connected
bool isConnected();

// launch
bool begin(uint16_t freq = 50);

// Turn off exits and put PCA9685 into sleep mode
bool end();

// fuse
bool setPWMFrequency(uint16_t freq);

// get the actual frequency of the PHIM
uint16_t getPWMFrequency();

// set the measured frequency of the generator (silent. 25000000) - after it you need to reconfigure the frequency of the PWM
void setOscFreq(uint32_t freq);

// Get a set frequency of the generator
uint32_t getOscFreq();

// record
bool writeMicroseconds(uint8_t channel, uint16_t us);

// record an angle or microsecond
bool write(uint8_t channel, uint16_t angle, uint16_t minUs = MinPulse, uint16_t maxUs = MaxPulse, uint16_t maxAngle = 180);

// microsecond
int32_t readMicroseconds(uint8_t channel);

// switch off
bool disable(uint8_t channel);

// shut off all channels
bool disable();
```

### MultiServo
Servo class tied to driver`MultiServoDriver`. Complies with the Arduino Servo API, except for the constructor:

```cpp
MultiServo(MultiServoDriver& drv);

bool attach(uint8_t channel, uint16_t pulse = MultiServoDriver::DefPulse);
bool attach(uint8_t channel, uint16_t minUs, uint16_t maxUs);
void detach();
bool writeMicroseconds(uint16_t value);
uint16_t readMicroseconds();
bool write(uint16_t value, uint16_t maxAngle = 180);
uint16_t read(uint16_t maxAngle = 180);
bool attached();
```

### Management
The library can be used in two modes:

- Control directly through the driver indicating the channel. The driver does not store the minimum and maximum momentum, does not automatically turn to the corner.
- Creating a separate servo object tied to the driver and managing through it. Stores minimum and maximum momentum and has the usual Servo API. This can be convenient if you want to transfer a servo object with a standard API, for example for template polymorphism.

## Examples
```cpp
#include <Arduino.h>
#include <MultiServo.h>
#include <MultiServoDriverPCA9685.h>

MultiServoDriverPCA9685 driver;  // create a driver
MultiServo servo(driver);        // servo on this driver

void setup() {
    Serial.begin(115200);

    Wire.begin();
    if (!driver.begin()) {
        Serial.println("Driver error");
    }

    servo.attach(1);                    // Connect the servo to driver channel 1
    servo.write(100);                   // corner
    driver.writeMicroseconds(3, 1500);  // rotate the channel 3 servo through the driver directly
    driver[3].writeMicroseconds(1500);  // Another option to control through the driver, also on channel 3
}

void loop() {
    // servo
    for (int angle = 0; angle <= 180; angle++) {
        servo.write(angle);
        delay(5);
    }

    for (int angle = 180; angle >= 0; angle--) {
        servo.write(angle);
        delay(5);
    }

    // driver
    for (int servo = 0; servo < 16; servo++) {
        for (int angle = 0; angle <= 180; angle++) {
            driver[servo].write(angle, 500, 2500);
            delay(5);
        }

        for (int angle = 180; angle >= 0; angle--) {
            driver[servo].write(angle, 500, 2500);
            delay(5);
        }
    }
}
```

<a id="versions"></a>

## Versions
- v1.0.0

<a id="install"></a>
## Installation
- The library can be found under the name **MultiServo** and installed through the library manager in:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Download the library](https://github.com/GyverLibs/MultiServo/archive/refs/heads/main.zip).zip archive for manual installation:
    - Unpack and put in *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Unpack and put in *C:\Program Files\Arduino\libraries* (Windows x32)
    - Unpack and put in *Documents/Arduino/libraries/ *
    - (Arduino IDE) Automatic installation from .zip: *Sketch/Connect library/Add .ZIP library...* and specify downloaded archive
- Read more detailed instructions for installing libraries[here](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Update
- I recommend always updating the library: new versions fix errors and bugs, as well as optimize and add new features.
- Through the library manager IDE: find the library as when installing and click "Update"
- Manually: **Delete the folder with the old version** and then put the new one in its place. “Replacement” can not be done: sometimes new versions delete files that will remain when replaced and can lead to errors!

<a id="feedback"></a>

## Bugs and feedback
If you find bugs, create **Issue**, or better write to the mail immediately.[alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
The library is open for revision and your **Pull Requests*!

When reporting bugs or incorrect work of the library, it is necessary to specify:
- Library version
- What is used by the IC
- SDK version (for ESP)
- Arduino IDE version
- Are embedded examples that use features and designs that cause bugs in your code working correctly?
- What code was downloaded, what work was expected from it and how it works in reality
- Ideally, attach the minimum code in which the bug is observed. Not a canvas of a thousand lines, but a minimum code.

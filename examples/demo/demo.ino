#include <Arduino.h>
#include <MultiServo.h>
#include <MultiServoDriverPCA9685.h>

MultiServoDriverPCA9685 driver;  // создаём драйвер
MultiServo servo(driver);        // серво на этом драйвере

void setup() {
    Serial.begin(115200);

    Wire.begin();
    if (!driver.begin()) {
        Serial.println("Driver error");
    }

    servo.attach(1);                    // подключаем серво к 1 каналу драйвера
    servo.write(100);                   // повернуть серво на угол
    driver.writeMicroseconds(3, 1500);  // повернуть серво на 3 канале через драйвер напрямую
    driver[3].writeMicroseconds(1500);  // ещё один вариант управления через драйвер
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
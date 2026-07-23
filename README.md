[![latest](https://img.shields.io/github/v/release/GyverLibs/MultiServo.svg?color=brightgreen)](https://github.com/GyverLibs/MultiServo/releases/latest/download/MultiServo.zip)
[![PIO](https://badges.registry.platformio.org/packages/gyverlibs/library/MultiServo.svg)](https://registry.platformio.org/libraries/gyverlibs/MultiServo)
[![Foo](https://img.shields.io/badge/Website-AlexGyver.ru-blue.svg?style=flat-square)](https://alexgyver.ru/)
[![Foo](https://img.shields.io/badge/%E2%82%BD%24%E2%82%AC%20%D0%9F%D0%BE%D0%B4%D0%B4%D0%B5%D1%80%D0%B6%D0%B0%D1%82%D1%8C-%D0%B0%D0%B2%D1%82%D0%BE%D1%80%D0%B0-orange.svg?style=flat-square)](https://alexgyver.ru/support_alex/)
[![Foo](https://img.shields.io/badge/README-ENGLISH-blueviolet.svg?style=flat-square)](https://github-com.translate.goog/GyverLibs/MultiServo?_x_tr_sl=ru&_x_tr_tl=en)  

[![Foo](https://img.shields.io/badge/ПОДПИСАТЬСЯ-НА%20ОБНОВЛЕНИЯ-brightgreen.svg?style=social&logo=telegram&color=blue)](https://t.me/GyverLibs)

# MultiServo
Библиотека для управления сервоприводами через PCA9685

### Совместимость
Совместима со всеми Arduino платформами (используются Arduino-функции)

## Содержание
- [Использование](#usage)
- [Версии](#versions)
- [Установка](#install)
- [Баги и обратная связь](#feedback)

<a id="usage"></a>

## Использование
### MultiServoDriver
Базовый класс драйвера, на его основе можно сделать поддержку других драйверов

```cpp
class DriverProxy {
    // записать микросекунды. 0 отключает канал
    bool writeMicroseconds(uint16_t us);

    // записать угол или микросекунды
    bool write(uint16_t angle, uint16_t minUs = MinPulse, uint16_t maxUs = MaxPulse, uint16_t maxAngle = 180);
};

// доступ через []
DriverProxy operator[](uint8_t channel);

// записать микросекунды. 0 отключает канал
virtual bool writeMicroseconds(uint8_t channel, uint16_t us) = 0;
```

### MultiServoDriverPCA9685
Класс для работы с драйвером PCA9685, наследует `MultiServoDriver`:

```cpp
MultiServoDriverPCA9685(uint8_t address = 0x40, TwoWire& wire = Wire) : _wire(wire), _addr(address) {}

// подключен
bool isConnected();

// запустить
bool begin(uint16_t freq = 50);

// отключить выходы и перевести PCA9685 в режим сна
bool end();

// установить частоту ШИМ в Гц
bool setPWMFrequency(uint16_t freq);

// получить реальную частоту ШИМ
uint16_t getPWMFrequency();

// установить измеренную частоту генератора (умолч. 25000000) - после неё нужно перенастроить частоту ШИМ
void setOscFreq(uint32_t freq);

// получить установленную частоту генератора
uint32_t getOscFreq();

// записать микросекунды
bool writeMicroseconds(uint8_t channel, uint16_t us);

// записать угол или микросекунды
bool write(uint8_t channel, uint16_t angle, uint16_t minUs = MinPulse, uint16_t maxUs = MaxPulse, uint16_t maxAngle = 180);

// прочитать микросекунды
int32_t readMicroseconds(uint8_t channel);

// отключить канал
bool disable(uint8_t channel);

// отключить все каналы
bool disable();
```

### MultiServo
Класс серво, привязанной к драйверу `MultiServoDriver`. Соответствует Arduino Servo API, за исключением конструктора:

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

### Управление
Библиотека может использоваться в двух режимах:

- Управление напрямую через драйвер с указанием канала. Драйвер не хранит минимум и максимум импульса, не умеет автоматически поворачивать на угол
- Создание отдельного объекта серво, привязанного к драйверу, и управление через него. Хранит минимум и максимум импульса и имеет привычный Servo API. Это может быть удобно, если нужно передать куда-то именно объект сервопривода со стандартным API, например для шаблонного полиморфизма

## Примеры
```cpp
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
    driver[3].writeMicroseconds(1500);  // ещё один вариант управления через драйвер, тоже на 3 канал
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

## Версии
- v1.0.0

<a id="install"></a>
## Установка
- Библиотеку можно найти по названию **MultiServo** и установить через менеджер библиотек в:
    - Arduino IDE
    - Arduino IDE v2
    - PlatformIO
- [Скачать библиотеку](https://github.com/GyverLibs/MultiServo/archive/refs/heads/main.zip) .zip архивом для ручной установки:
    - Распаковать и положить в *C:\Program Files (x86)\Arduino\libraries* (Windows x64)
    - Распаковать и положить в *C:\Program Files\Arduino\libraries* (Windows x32)
    - Распаковать и положить в *Документы/Arduino/libraries/*
    - (Arduino IDE) автоматическая установка из .zip: *Скетч/Подключить библиотеку/Добавить .ZIP библиотеку…* и указать скачанный архив
- Читай более подробную инструкцию по установке библиотек [здесь](https://alexgyver.ru/arduino-first/#%D0%A3%D1%81%D1%82%D0%B0%D0%BD%D0%BE%D0%B2%D0%BA%D0%B0_%D0%B1%D0%B8%D0%B1%D0%BB%D0%B8%D0%BE%D1%82%D0%B5%D0%BA)
### Обновление
- Рекомендую всегда обновлять библиотеку: в новых версиях исправляются ошибки и баги, а также проводится оптимизация и добавляются новые фичи
- Через менеджер библиотек IDE: найти библиотеку как при установке и нажать "Обновить"
- Вручную: **удалить папку со старой версией**, а затем положить на её место новую. "Замену" делать нельзя: иногда в новых версиях удаляются файлы, которые останутся при замене и могут привести к ошибкам!

<a id="feedback"></a>

## Баги и обратная связь
При нахождении багов создавайте **Issue**, а лучше сразу пишите на почту [alex@alexgyver.ru](mailto:alex@alexgyver.ru)  
Библиотека открыта для доработки и ваших **Pull Request**'ов!

При сообщении о багах или некорректной работе библиотеки нужно обязательно указывать:
- Версия библиотеки
- Какой используется МК
- Версия SDK (для ESP)
- Версия Arduino IDE
- Корректно ли работают ли встроенные примеры, в которых используются функции и конструкции, приводящие к багу в вашем коде
- Какой код загружался, какая работа от него ожидалась и как он работает в реальности
- В идеале приложить минимальный код, в котором наблюдается баг. Не полотно из тысячи строк, а минимальный код
1. используется arduino ide вместо platform io. так оказалось быстрее.
2. иннструкция по установке:
  2.1. установить arduino ide. https://www.arduino.cc/en/main/software
  2.2. добавить поддержку esp32 в arduino. https://github.com/espressif/arduino-esp32
  2.3. установить плагин для загрузки SPIFFS (использование внутренней памяти) https://github.com/me-no-dev/arduino-esp32fs-plugin
  2.4. установить библиотеки:
    скачать zip с github:
      https://github.com/bblanchon/ArduinoJson
      https://github.com/me-no-dev/AsyncTCP
      https://github.com/me-no-dev/ESPAsyncWebServer
    импортировать в arduino ide:
      Sketch -> Include Library -> Add .ZIP Library...
3. загрузить скетч
4. загрузить SPIFFS.
  Tools -> ESP32 Sketch Data Upload

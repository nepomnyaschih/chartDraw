используется arduino ide вместо platform io. так оказалось быстрее.
## иннструкция по установке
  1. установить arduino ide. https://www.arduino.cc/en/main/software
  2. добавить поддержку esp32 в arduino. https://github.com/espressif/arduino-esp32
  3. установить плагин для загрузки SPIFFS (использование внутренней памяти) https://github.com/me-no-dev/arduino-esp32fs-plugin
  4. установить библиотеки:
    скачать zip с github:
    *  https://github.com/bblanchon/ArduinoJson
    *  https://github.com/me-no-dev/AsyncTCP
    *  https://github.com/me-no-dev/ESPAsyncWebServer
    импортировать в arduino ide:
      Sketch -> Include Library -> Add .ZIP Library...
  5. загрузить скетч
  6. загрузить SPIFFS.
    Tools -> ESP32 Sketch Data Upload

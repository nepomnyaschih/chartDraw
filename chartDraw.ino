#include <Arduino.h>
#include "ArduinoJson.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include <FS.h>
#include <SPIFFS.h>
#include "WiFi.h"


AsyncWebServer server(80);
AsyncWebSocket ws("/ws"); // access at ws://[esp ip]/ws

const char *ssid = "esp32test";
const char *password = "12345678";

#define SENSOR1 32
#define PORTIONDATA 500

struct SensorData {
  int time;
  int value;
};

int iterator = 0;

SensorData sensor1DataCollection[PORTIONDATA];

void sendDataWs() {

  // int sensorVal = analogRead(SENSOR1);

  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject(); //создание объекта json
  JsonArray &test = root.createNestedArray("test");

  for (int i = 0; i < PORTIONDATA; i++) {

    JsonArray &tmp = test.createNestedArray();

    tmp.add(sensor1DataCollection[i].time);
    tmp.add(sensor1DataCollection[i].value);

    test[i] = tmp;
  }

  // root["val0"] = sensorVal;
  // root["val1"] = random(255);

  // Serial.println("+++++++++++++++++++++++++++++");
  // root.prettyPrintTo(Serial);
  // Serial.println("+++++++++++++++++++++++++++++");

  size_t len = root.measureLength(); //вычисление длины объекта
  AsyncWebSocketMessageBuffer *buffer =
    ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
  if (buffer) {
    root.printTo((char *)buffer->get(), len + 1); // json в буфер
    ws.textAll(buffer); //отправка буффера
  }
}

void loopChart() {

  int val = analogRead(SENSOR1);

  if (iterator >= PORTIONDATA) {
    iterator = 0;
    // Serial.println(sensor1DataCollection[0].value);

    // Serial.println();
    // Serial.print("heap: ");
    // Serial.println(system_get_free_heap_size());

    sendDataWs();
  }

  sensor1DataCollection[iterator].time = millis();
  sensor1DataCollection[iterator].value = val;

  // Serial.print("value: ");
  // Serial.println(val);

  iterator++;
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
               AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.println("Connected");
  }
}

void firstCoreTask(void *pvParameters) {
  while (true) {
    // Serial.println("tic");
    delay(10);
    loopChart();
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Start... ver. 0.0.2");

  //запуск wifi в режиме точки доступа
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();

  Serial.print("AP IP address: ");
  Serial.println(myIP);

  //запуск обработчиков событий вебсокетов
  server.addHandler(&ws);
  //запуск вебсервера
  server.begin();

  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS Mount Failed");
    return;
  } else {
    Serial.println("SPIFFS Mount Success!!");
  }
  //установка index.html умолчательным файлом для сервера
  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  ws.onEvent(onWsEvent);

  xTaskCreatePinnedToCore(firstCoreTask,   /* Function to implement the task */
                          "firstCoreTask", /* Name of the task */
                          10000,           /* Stack size in words */
                          NULL,            /* Task input parameter */
                          0,               /* Priority of the task */
                          NULL,            /* Task handle. */
                          0);              /* Core where the task should run */
}

void loop() {}

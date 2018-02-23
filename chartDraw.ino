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
#define SENSOR2 33
#define SENSOR3 34

#define PORTIONDATA 100
#define LOOPCHARTDELAY 10

struct SensorData {
  int time;
  int value;
};

int iterator = 0;

SensorData sensor1DataCollection[PORTIONDATA];
SensorData sensor2DataCollection[PORTIONDATA];
SensorData sensor3DataCollection[PORTIONDATA];

SensorData sensorSUMDataCollection[PORTIONDATA];

void sendDataWs() {

  DynamicJsonBuffer jsonBuffer;
  JsonObject &root = jsonBuffer.createObject();

  //sensor 1
  JsonObject &sens1 = root.createNestedObject("sens1");;
  sens1["port"] = SENSOR1;
  JsonArray &sens1data = sens1.createNestedArray("data");

  //sensor 2
  JsonObject &sens2 = root.createNestedObject("sens2");;
  sens2["port"] = SENSOR2;
  JsonArray &sens2data = sens2.createNestedArray("data");

  //sensor 3
  JsonObject &sens3 = root.createNestedObject("sens3");;
  sens3["port"] = SENSOR3;
  JsonArray &sens3data = sens3.createNestedArray("data");

  //sum
  JsonObject &sum = root.createNestedObject("sum");;
  sum["port"] = 0;
  JsonArray &sumData = sum.createNestedArray("data");
  
  for (int i = 0; i < PORTIONDATA; i++) {

      //sensor 1
      JsonObject &sens1DataObj = sens1data.createNestedObject();
      sens1DataObj["time"] = sensor1DataCollection[i].time;
      sens1DataObj["value"] = sensor1DataCollection[i].value;

      //sensor 2
      JsonObject &sens2DataObj = sens2data.createNestedObject();
      sens2DataObj["time"] = sensor2DataCollection[i].time;
      sens2DataObj["value"] = sensor2DataCollection[i].value;

      //sensor 3
      JsonObject &sens3DataObj = sens3data.createNestedObject();
      sens3DataObj["time"] = sensor3DataCollection[i].time;
      sens3DataObj["value"] = sensor3DataCollection[i].value;

      //sensor 3
      JsonObject &sumDataObj = sumData.createNestedObject();
      sumDataObj["time"] = sensorSUMDataCollection[i].time;
      sumDataObj["value"] = sensorSUMDataCollection[i].value;
   
    }

/*
  for (int i = 0; i < PORTIONDATA; i++) {

    JsonArray &tmp = test.createNestedArray();

    tmp.add(sensor1DataCollection[i].time);
    tmp.add(sensor1DataCollection[i].value);

    test[i] = tmp;
  }
*/

  size_t len = root.measureLength(); //вычисление длины объекта
  AsyncWebSocketMessageBuffer *buffer =
    ws.makeBuffer(len); //  creates a buffer (len + 1) for you.
  if (buffer) {
    root.printTo((char *)buffer->get(), len + 1); // json в буфер
    ws.textAll(buffer); //отправка буффера
  }
}

void loopChart() {
  int time = millis();
  
  int val1 = analogRead(SENSOR1);
  int val2 = analogRead(SENSOR2);
  int val3 = analogRead(SENSOR3);

  if (iterator >= PORTIONDATA) {
    iterator = 0;
    sendDataWs();
  }

  sensor1DataCollection[iterator].time = time;
  sensor1DataCollection[iterator].value = val1;

  sensor2DataCollection[iterator].time = time;
  sensor2DataCollection[iterator].value = val2;

  sensor3DataCollection[iterator].time = time;
  sensor3DataCollection[iterator].value = val3;

  sensorSUMDataCollection[iterator].time = time;
  sensorSUMDataCollection[iterator].value = val1+val2+val3;

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
    delay(LOOPCHARTDELAY);
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

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <ArduinoOTA.h>
//#define ENABLE_GxEPD2_GFX 0

// #include <GxEPD2_BW.h>
// #include <GxEPD2_3C.h>
// #include <Fonts/FreeMonoBold9pt7b.h>

#ifndef STASSID
#define STASSID "Huck!Lan"
#define STAPSK  "HuckSpeed2018+"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;
const int oneWireBus = D6; 

OneWire oneWire(oneWireBus);
ESP8266WebServer server(80);

DallasTemperature sensors(&oneWire);
//GxEPD2_BW<GxEPD2_154, GxEPD2_154::HEIGHT>display(GxEPD2_154(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));

void handleSetApi();
//void drawFullDisplay();

float tempOut = 0.0;
float humiOut = 0.0;
float tempIn = 0.0;

LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  sensors.begin();
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  lcd.setCursor(0,0);
  lcd.print("Verbinde mit");
  lcd.setCursor(0,1);
  lcd.print(ssid);
  Serial.print("\nConnecting to ");
  Serial.println(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print("#");
  }
  lcd.clear();
  lcd.home();
  lcd.print("Verbunden! IP:");
  lcd.setCursor(0,1);
  lcd.print(WiFi.localIP());
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(200);
  if (MDNS.begin("weather")) {
    Serial.println("MDNS responder started");
  }
  server.on("/api/set",handleSetApi) ;
  server.begin();
  //display.init(115200);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //drawFullDisplay()
  sensors.requestTemperatures();
  float tmp = sensors.getTempCByIndex(0);
  if (tmp != tempIn) {
    tempIn = tmp;
    lcd.setCursor(0,0);
    lcd.print("T.in: " + String(tempIn));
  }
  server.handleClient();
}

void handleSetApi() {
  if (server.hasArg("temp-out")) {
    tempOut = server.arg("temp-out").toFloat();
    Serial.print("set tempOut = ");
    Serial.println(tempOut);
    server.send(200,"text/plain","OK");
  }
  if (server.hasArg("humi-out")) {
    humiOut = server.arg("humi-out").toFloat();
    Serial.print("set humiOut = ");
    Serial.println(humiOut);
    server.send(200,"text/plain","OK");
  }
  lcd.setCursor(0,1);
  lcd.print("T:" + String(tempOut) + " F:" + String(humiOut) + "%");
}

// const char HelloWorld[] = "Hello World!";
// void drawFullDisplay() {
//   Serial.println("helloWorld");
//   display.setRotation(1);
//   display.setFont(&FreeMonoBold9pt7b);
//   display.setTextColor(GxEPD_BLACK);
//   int16_t tbx, tby; uint16_t tbw, tbh;
//   display.getTextBounds(HelloWorld, 0, 0, &tbx, &tby, &tbw, &tbh);
//   // center bounding box by transposition of origin:
//   uint16_t x = ((display.width() - tbw) / 2) - tbx;
//   uint16_t y = ((display.height() - tbh) / 2) - tby;
//   display.setFullWindow();
//   display.firstPage();
//   do
//   {
//     display.fillScreen(GxEPD_WHITE);
//     display.setCursor(x, y);
//     display.print(HelloWorld);
//   }
//   while (display.nextPage());
//   Serial.println("helloWorld done");
// }
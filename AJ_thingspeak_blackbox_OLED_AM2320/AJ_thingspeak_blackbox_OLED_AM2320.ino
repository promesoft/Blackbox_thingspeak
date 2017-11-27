// ESP8266 DS18B20 ArduinoIDE Thingspeak IoT Example code
// Made into AM2320, DS1621, DHT11, BMP180 and FC-37 monitor by Anders Johansen

// ===============================================
// -----------------------------------------------
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
//#include <OneWire.h>
//#include <DallasTemperature.h>
//#include <SFE_BMP180.h>
//#include <DHT.h>
//#include <DS1621.h>
#include <Wire.h>
#include <SPI.h>
#include "SSD1306.h"
#include "OLEDDisplayUi.h" //new style
//#include "SSD1306Ui.h"
#include "images.h"
#include <AM2320.h>
//#include <AM2321.h>
#include "wifi.h"
// ===============================================
char degSymbol[3] = { 0xC2, 0xB0, 0x00 }; // degree symbol(0xB0 or 176) in the extended ascii set

// ------------------PIN Definitions-------------
//#define DHTTYPE DHT11   // DHT Shield uses DHT 11
//#define DHTPIN D4       // DHT Shield uses pin D4
//#define RAINPIN A0       // Only analogue pin on ESP8266

// -----------------------------------------------
//#define ONE_WIRE_BUS D4

// -----------------------------------------------
// Pin definitions for I2C SSD_1306 OLED
//#define OLED_SDA    D2  // pin 14
//#define OLED_SDC    D4  // pin 12
#define OLED_SDA    D14
#define OLED_SDC    D15
#define OLED_ADDR   0x3C

// ------------------HTTP GET settings------------
const char* host = "api.thingspeak.com"; // Your domain  
String ApiKey = "ENTERKEYHERE"; //channel #
String path = "/update?key=" + ApiKey;
String field1 = "&field1=";  
String field2 = "&field2=";
String field3 = "&field3=";  
String field4 = "&field4=";
String field5 = "&field5=";
String field6 = "&field6=";
String field7 = "&field7=";
String field8 = "&field8=";
String get_end = "Connection: keep-alive\r\n\r\n";

// ------------------DS18B20 sensor text output---
//char temperatureString[6] = "test"; //DS18B20
// ------------------DS1621 sensor --------------
//byte addr = (0x90 >> 1) | 0;  // replace the 0 with the value you set on pins A2, A1 and A0
//DS1621 sensor=DS1621(addr);
//char DS1621temperatureString[6] = "test";
// ------------------BMP180 sensor----------------
//SFE_BMP180 pressure;
//char BMP180temperatureString[6] = "test";
//char BMP180pressureString[6] = "test";
// ------------------AM2320 sensor----------------
//AM2321 th;
AM2320 th;
char AM2320temperatureString[8] = "test";
char AM2320humidityString[8] = "test_RH";
// ------------------DHT sensor-------------------
//DHT dht(DHTPIN, DHTTYPE);
//char DHT11temperatureString[6] = "test";
//char DHT11humidityString[6] = "test";
//unsigned long previousTempMillis, previousHumMillis = 0;  // When the sensor was last read
//const long interval = 2000;                  // Wait this long until reading again
// ------------------Rain sensor------------------
//char rainString[6] = "test";
// ------------------DS18B20 Sensor---------------
// Initialize DS18B20 sensor
//OneWire oneWire(ONE_WIRE_BUS);
//DallasTemperature DS18B20(&oneWire);
// ------------------Initialize Display-----------
SSD1306   display(OLED_ADDR, OLED_SDA, OLED_SDC);    // For I2C
//SSD1306Ui ui     ( &display );
OLEDDisplayUi ui     ( &display ); //new style

//Explicit prototype
void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state);
void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);
void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y);

FrameCallback frames[] = { drawFrame1, drawFrame2, drawFrame3 };
OverlayCallback overlays[] = { msOverlay };

// how many frames are there?
int frameCount = 3;
int overlaysCount = 1;

//Explicit prototype - old
//bool msOverlay(SSD1306 *display, SSD1306UiState* state);
//bool drawFrame1(SSD1306 *display, SSD1306UiState* state, int x, int y);
//bool drawFrame2(SSD1306 *display, SSD1306UiState* state, int x, int y);
//bool drawFrame3(SSD1306 *display, SSD1306UiState* state, int x, int y);

//bool (*frames[])(SSD1306 *display, SSD1306UiState* state, int x, int y) = { drawFrame1, drawFrame2, drawFrame3 };
//bool (*overlays[])(SSD1306 *display, SSD1306UiState* state)             = { msOverlay };


// ===============================================
// -----------------------------------------------
void setup(void){
  Serial.begin(115200);
  Serial.println("");

  // -----------------------------------------------
  // Initialize the DS1621 sensor 
  // -----------------------------------------------
  /*sensor.startConversion(false);                       // stop if presently set to continuous
  sensor.setConfig(DS1621::POL | DS1621::ONE_SHOT);    // Tout = active high; 1-shot mode
  sensor.setThresh(DS1621::ACCESS_TH, 27);             // high temp threshold = 80F
  sensor.setThresh(DS1621::ACCESS_TL, 24);             // low temp threshold = 75F
  */

  // -----------------------------------------------
  // Initialize the BMP180 sensor 
  // (it is important to get calibration values stored on the device).
  // -----------------------------------------------
  /*Serial.println("BMP180 init start");
  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    Serial.println("BMP180 init fail\n\n");
  }
  */
  delay(10);

  // -----------------------------------------------
  // Wifi setup
  // -----------------------------------------------
  WiFi.begin(ssid, pass);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // -----------------------------------------------
  // Initialize the DS18B20 sensor 
  // -----------------------------------------------
  //DS18B20.begin();

  // -----------------------------------------------
  // Initialize the DHT11 sensor 
  // -----------------------------------------------
  //dht.begin();

  // -----------------------------------------------
  // Setup fonts for OLED 
  // -----------------------------------------------
  ui.setTargetFPS(30);

//  ui.setActiveSymbole(activeSymbole);
//  ui.setInactiveSymbole(inactiveSymbole);

  ui.setIndicatorPosition(RIGHT);
  ui.setIndicatorDirection(LEFT_RIGHT);
  ui.setFrameAnimation(SLIDE_LEFT);
  ui.disableAutoTransition();

  // Add frames and overlays
  ui.setFrames(frames, frameCount);
  ui.setOverlays(overlays, overlaysCount);

  // Initalialize display 
  ui.init();

//  display.flipScreenVertically();

  // -----------------------------------------------
  Serial.println("");
  Serial.println("WeMos Initialized");
  Serial.println("");
}

// ===============================================
// ------------------BMP180 ---------------------
/*void getBMP180readings() {
  char status;
  double T,P;
  status = pressure.startPressure(3);
  if (status != 0)
  {
   // Wait for the measurement to complete:
    delay(status);
    status = pressure.getPressure(P,T);
    if (status != 0)
    {
      // Print out the measurement:
      Serial.print("pressure: ");
      Serial.print(P,2);
      Serial.println(" deg C, ");
    }
  }
  dtostrf(P, 2, 2, BMP180pressureString);
  Serial.println(BMP180pressureString);
  dtostrf(T, 2, 2, BMP180temperatureString);
  Serial.println(BMP180temperatureString);
  return;
}
*/
// ===============================================
// ------------------DHT11 Temp-------------------
/*void getDHT11Temperature() {
  float temp = 100; //set value above range for detecting error readning
  unsigned long currentMillis = millis();

  if (currentMillis - previousTempMillis >= interval) {
    // Save the last time you read the sensor
    previousTempMillis = currentMillis;

    temp = dht.readTemperature();  // Read temperature as Celsius

    // Check if any reads failed and exit early (to try again).
    if (isnan(temp)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    else temp = temp - 3; //adjust well known issue with the Wemos D1 mini DHT11 shield
  }
  
  //if it returns 100 it is an error readning and we return the old measurement
  if (temp !=100) dtostrf(temp, 1, 2, DHT11temperatureString); 
  Serial.print("Temp DHT11 ");
  Serial.println(DHT11temperatureString);

  return;
}
*/
// ===============================================
// ------------------DHT11 Humidity---------------
/*void getDHT11Humidity() {
  float hum = 100; //set value above range for detecting error readning
  unsigned long currentMillis = millis();

  if (currentMillis - previousHumMillis >= interval) {
    // Save the last time you read the sensor
    previousHumMillis = currentMillis;

    hum = dht.readHumidity();  // Read humidity 

    // Check if any reads failed and exit early (to try again).
    if (isnan(hum)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
  }
  //if it returns 100 it is an error readning and we return the old measurement
  if (hum !=100) dtostrf(hum, 1, 2, DHT11humidityString);
  Serial.print("Hum DHT11 ");
  Serial.println(DHT11humidityString);
}
*/
// ===============================================
// ------------------DS18B20----------------------
/*void getTemperature() {
  float temp;
  do {
    DS18B20.requestTemperatures(); 
    temp = DS18B20.getTempCByIndex(0);
    delay(100);
  } while (temp == 85.0 || temp == (-127.0));

  dtostrf(temp, 2, 2, temperatureString);
  // send temperature to the serial console
  Serial.println(temp);
  return;
}
*/

// ===============================================
// ------------------Get DS1621 Temp--------------
/*float get1621Temp()
{
  int tC;
  tC = sensor.getHrTemp();                             // read high-resolution temperature
  float t = float(tC) / 100;
  return t;
}
*/
// ===============================================
// ------------------Get AM2320 readings----------
void get2320readings()
{
  dtostrf(th.getTemperature()/10.0, 2, 1, AM2320temperatureString);
  dtostrf(th.getHumidity()/10.0, 2, 1, AM2320humidityString);
  Serial.println(AM2320temperatureString);  
  Serial.println(AM2320humidityString);
}

// ===============================================
// ------------------Get AM2321 Readings----------
/*void get2320readings()
{
  th.read();
  dtostrf(th.temperature/10.0, 2, 2, AM2320temperatureString);
  dtostrf(th.humidity/10.0, 2, 2, AM2320humidityString);
  Serial.println(AM2320temperatureString);  
  Serial.println(AM2320humidityString);
}
*/
// ===============================================
// ------------------Post to thingspeak-----------
void postToThingspeak()
{
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("GET ") + path 
//               + field2 + temperatureString 
               + field1 + AM2320temperatureString
               + field2 + AM2320humidityString
//               + field5 + DHT11temperatureString
//               + field6 + DHT11humidityString
//               + field7 + rainString*/
               + " HTTP/1.1\r\n"
               + "Host: " + host + "\r\n" 
               + get_end);
  Serial.println("End of measurement");
}  
// ===============================================
// ------------------OLED Frame 1-----------------
void drawFrame1(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
//bool drawFrame1(SSD1306 *display, SSD1306UiState* state, int x, int y) {
  display->drawXbm(x + 34, y + 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
  }
// ===============================================
// ------------------OLED Frame 2-----------------
void drawFrame2(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {
//  bool drawFrame2(SSD1306 *display, SSD1306UiState* state, int x, int y) {
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0 + x, 10 + y, "Home Automation");

  display->setFont(ArialMT_Plain_16);
  display->drawString(0 + x, 20 + y, "Temperature:");

  display->setFont(ArialMT_Plain_24);
  String s = String(AM2320temperatureString);
  s.concat( degSymbol );
  s.concat( "C" );
  display->drawString(0 + x, 34 + y, s);
}
// ===============================================
// ------------------OLED Frame 3-----------------
//bool drawFrame3(SSD1306 *display, SSD1306UiState* state, int x, int y) {
void drawFrame3(OLEDDisplay *display, OLEDDisplayUiState* state, int16_t x, int16_t y) {  
  display->setTextAlignment(TEXT_ALIGN_LEFT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(0 + x, 10 + y, "Home Automation");

  display->setFont(ArialMT_Plain_16);
  display->drawString(0 + x, 20 + y, "Humidity:");

  display->setFont(ArialMT_Plain_24);
  String s = String(AM2320humidityString);
  s.concat( "%" );
  s.concat( "RH" );
  display->drawString(0 + x, 34 + y, s);
}
// ===============================================
// ------------------OLED Overlay-----------------
// bool msOverlay(SSD1306 *display, SSD1306UiState* state) {
void msOverlay(OLEDDisplay *display, OLEDDisplayUiState* state) {
  display->setTextAlignment(TEXT_ALIGN_RIGHT);
  display->setFont(ArialMT_Plain_10);
  display->drawString(128, 0, String(millis()));
}

// ===============================================
// ===============================================
// ===============================================
void loop() {
  int remainingTimeBudget = ui.update();

// ------------------DS18B20----------------------
//  getTemperature();
// ------------------BMP180 ----------------------
//  getBMP180readings();  
// ------------------DHT-11 ----------------------
//  getDHT11Temperature();
//  getDHT11Humidity();
// ------------------AM2320 ----------------------
  get2320readings();
// ------------------Rain sensor -----------------
/*  int rain = analogRead(RAINPIN);
  itoa(rain, rainString, 10);
  Serial.print("Rain ");
  Serial.println(rainString);
*/  

// ------------------Post to thingspeak-----------
  postToThingspeak();
// ------------------Repeat----------------------
  Serial.println("------------------");
  delay(1000);
}


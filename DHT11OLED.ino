//GFX
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//NodeMCU 1.0 SCL:D1/5 or D3/0 SDA:D2/4 or D4/2
//(x,y) Height: 1x text 7px | 2x Text 14px | 3x text 21px
//DHT Delay without delay
const int ledPin =  LED_BUILTIN;// the number of the LED pin
int ledState = LOW;             // ledState used to set the LED
unsigned long previousMillis = 0;  // will store last time LED was updated
const long interval = 1000;        // interval at which to blink (milliseconds)

//DHT Read
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN D5
#define DHTTYPE    DHT11
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

//Web

//----------------------------------------------------------------------------------------------------------

void DHTRead(void) {
}
//----------------------------------------------------------------------------------------------------------

void SplashScreen(void) {
  display.clearDisplay();
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner

  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.write("Text1X");

  display.setCursor(0, 9);
  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.write("Text2X");

  display.setCursor(0, 24);
  display.setTextSize(3);      // Normal 1:1 pixel scale
  display.write("Text3X");
  display.display();
}

//----------------------------------------------------------------------------------------------------------

void DHT_initialize(void) {
  dht.begin();
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

//----------------------------------------------------------------------------------------------------------

void OLED_initialize(void) {
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { //idk why but need this for display to function
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay(); //get rid of default adafruit logo when display is initializeed
  display.display();
}

//==========================================================================================================
//==========================================================================================================

void setup() {
  int MaxTemp = 0;
  int MinTemp = 0;
  int MaxRH = 0;
  int MinRH = 0;
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  digitalWrite(ledPin, ledState);
  //DHT
  DHT_initialize();

  //OLED
  OLED_initialize();

  SplashScreen();

}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    if (ledState == LOW) {
      ledState = HIGH;
      analogWrite(ledPin, 1000);
    }
    else {
      ledState = LOW;
      digitalWrite(ledPin, HIGH);
    }

    display.clearDisplay();
    display.cp437(true);         // Use full 256 char 'Code Page 437' font
    display.setTextColor(SSD1306_WHITE); // Draw white text
    display.setCursor(0, 0);     // Start at top-left corner
    display.setTextSize(3);
    display.write("DHT11");

    display.setTextSize(2);

    display.setCursor(0, 24);
    display.write("Temp:");

    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println(F("Error reading temperature!"));
      display.write("Error");
    }
    else {
      Serial.print(F("Temperature: "));
      Serial.print(event.temperature);
      Serial.println(F("°C"));

      String Temp = String(event.temperature, 1);
      display.write(Temp.c_str());
      display.write("C");
    }


    // Get humidity event and print its value.

    display.setCursor(0, 41);
    display.write("RH:");

    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("Error reading humidity!"));
      display.write("Error");
    }
    else {
      Serial.print(F("Humidity: "));
      Serial.print(event.relative_humidity);
      Serial.println(F("%"));
      String RH = String(event.relative_humidity, 1);
      char* buf2 = (char*) malloc(sizeof(char) * RH.length() + 1);
      RH.toCharArray(buf2, RH.length() + 1);
      display.write(buf2);
      free(buf2);
      display.write("%");
    }
    display.display();
  }
}

//-----------------------------------------------------------------------------------OLED
//OLED REFERENCES
//FONT SIZE: (x,y) Height: 1x text 7px | 2x Text 14px | 3x text 21px

//I2C protocol default SCL,SDA Pins
//NodeMCU 1.0               SCL: D1/GPIO5    SDA: D2/GPIO4
//Arduino UNO or Pro Mini   SCL: A5          SDA: A4
//ESP32 DevKit V1 36 Pin    SCL: D22/GPIO22  SDA: D21/GPIO21
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET     -1   // OLED reset pin # (or -1 if none or sharing board reset pin)
#define SCREEN_ADDRESS 0x3C //See datasheet for Address
//Wire.begin(sda, scl);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
bool invertDisplay = false;

//------------------------------------------------------------------------------------DHT
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
//define DHTTYPE DHT22  //Change if you are using DHT22
#define DHTTYPE DHT11   //Set sensor type
#define DHTPIN D5       //Set to pin where the DHT Sensor data is connected
DHT_Unified dht(DHTPIN, DHTTYPE);
//uint32_t delayMS;
unsigned int delayMS;
float currentTemp;
float currentRH;
float averageTemp;
float averageRH;
unsigned long samples;
String Tavg;
String RHavg;
String totalSamples;
float MaxTemp;
float MinTemp;
float MaxRH;
float MinRH;
String MT;
String mT;
String MRH;
String mRH;

//Delay without delay
const byte ledPin = D0;    // pin of LED
const byte ledPin2 = LED_BUILTIN;
const bool invertedLED = true;
bool ledState = LOW;               // state to set LED to
unsigned long previousMillis = 0;  // last time function was called
const long interval = 1000;        // interval

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void SplashScreen(void) {
  display.clearDisplay();      // Always clear before writing new screen
  display.setCursor(0, 0);     // Start at top-left corner
  display.setTextSize(2);      // Set text size
  display.write("Phuwanut");
  display.setCursor(0, 16);    // Set second text starting location
  display.write("DHT OLED");
  display.display();           // Display written text on the display
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void DHT_initialize(void) { //Mostly from Adafruit DHT_Unified_Sensor example
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor")); // Print temperature sensor details.
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));    // Print humidity sensor details.
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  const unsigned long delayMS = sensor.min_delay / 1000;       // Set delay between sensor readings based on sensor details.
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------


void OLED_initialize(void) { // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) { // Idk why but need this for display to function
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.clearDisplay(); //get rid of default adafruit logo when display is initializeed
  display.display();
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void setup() {
  OLED_initialize();
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.setTextColor(SSD1306_WHITE); // Draw white text
  pinMode(ledPin, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  Serial.begin(9600);
  digitalWrite(ledPin, invertedLED);
  //DHT
  DHT_initialize();

  SplashScreen();
  if (invertedLED == true) {
    ledState = !ledState;
  }
}

//=========================================================================================================================================================================================
//=========================================================================================================================================================================================

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) { // Past the sensor interval
    previousMillis = currentMillis;        // Save last time function was called
    //ROW 1-HEADER-------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    display.clearDisplay();                // Always clear old stuff before drawing new stuff
    display.setTextColor(SSD1306_WHITE);   // Draw white text

    display.setCursor(0, 0);               // Start at top-left corner
    display.setTextSize(2);                // 2x font
    display.write("DHT11");                // Actual text
    sensors_event_t event;                 // Create a sensors_event_t object in memory to hold results
    //Row 2-TEMP---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    display.setCursor(0, 17);              // Move cursor Down
    display.write("Temp:");                // Actual text 2

    dht.temperature().getEvent(&event);    // Get a new sensor event, passing in our 'event' placeholder
    if (isnan(event.temperature)) {        // If temperature is not a number
      Serial.println(F("Error reading temperature!"));
      display.write("Error");
    }
    else {
      Serial.print(F("Temperature: "));
      Serial.print(event.temperature);
      Serial.println(F("°C"));
      currentTemp = event.temperature;
      String Temp = String(event.temperature, 1);
      display.write(Temp.c_str());
      display.write("C");
    }
    display.setCursor(0, 34);
    display.write("RH:");

    dht.humidity().getEvent(&event);      // Read relative humidity data
    if (isnan(event.relative_humidity)) { // If relative humidity is not a number
      Serial.println(F("Error reading humidity!"));
      display.write("Error");
    }
    else {
      Serial.print(F("Humidity: "));
      Serial.print(event.relative_humidity);
      Serial.println(F("%"));
      currentRH = event.relative_humidity;
      String RH = String(event.relative_humidity, 1);
      display.write(RH.c_str());
      display.write("%");
    }

    //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    ledState = !ledState; // Toggle LED state
    if (isnan(event.temperature) || isnan(event.relative_humidity)) { // Not getting any data from DHT sensor
      invertDisplay = !invertDisplay;
      if (ledState == HIGH) {
        digitalWrite(ledPin, ledState); // if invertedLED is true, LED turns off and vice versa
        digitalWrite(ledPin2, ledState);
      }
      else if (ledState == LOW) {
        digitalWrite(ledPin, ledState);
        digitalWrite(ledPin2, ledState);
      }
    }
    else if (!isnan(event.temperature) && !isnan(event.relative_humidity)) {
      invertDisplay = false;
      digitalWrite(ledPin, invertedLED);
      digitalWrite(ledPin2, invertedLED);

      samples++;
      averageTemp += (currentTemp - averageTemp) / samples;
      averageRH += (currentRH - averageRH) / samples;
      Serial.print("averageTemp: ");
      Serial.println(averageTemp);
      Serial.print("averageRH: ");
      Serial.println(averageRH);
      Tavg = String(averageTemp, 1);
      RHavg = String(averageRH, 1);
      totalSamples = String(samples);
    }
    display.setTextSize(1);
    //display.setCursor(0, 49);
    display.setCursor(60, 0);
    display.write("Tavg:");
    display.write(Tavg.c_str());
    display.write("C");
    //display.setCursor(64, 49);
    display.setCursor(60, 8);
    display.write("RHavg:");
    display.write(RHavg.c_str());
    display.write("%");
    display.setCursor(0, 57);
    display.write("count:");
    display.write(totalSamples.c_str());

    display.invertDisplay(invertDisplay);
    display.display();
  }
}

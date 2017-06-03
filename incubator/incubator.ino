#include <OneWire.h>
#include <DallasTemperature.h>

#include "DHT.h"
#define DHTPIN 2        // digital pin of temp sensor
#define DHTTYPE DHT11   // DHT 11
#define ONE_WIRE_BUS 3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

int incubatorPin = 7;
int cookerPin = 6;
int errorPin = 13;

float incubatorLow = 92.0; // below this temp the heater/lights activate
float incubatorHigh = 88.0; // above this temp the heater/lights deactivate

// cooker temps are in celsius

// best eggs
// float cookerLow = 62.5; // 144.5 F
// float cookerHigh = 62.8; // 145.0 F
// see http://www.seriouseats.com/2013/10/sous-vide-101-all-about-eggs.html

// for float tank
float cookerLow = 34.15; // 93.47 F
float cookerHigh = 34.2; // 93.56 F

// garlic stuff
// float cookerLow = 88.0;
// float cookerHigh = 88.5;
//float cookerLow = 0;
//float cookerHigh = 0;

// medium rare steak 130 F
// float cookerLow = 54.4;
// float cookerHigh = 54.7;


void setup() {
  Serial.begin(9600);
  dht.begin();
  tempSensor.begin();
  pinMode(incubatorPin, OUTPUT);
  pinMode(cookerPin, OUTPUT);
  pinMode(errorPin, OUTPUT);

  digitalWrite(cookerPin, LOW);
}

void errorBlink(int times, int wait) {
  for (int i = 0; i < times; i++) {
    digitalWrite(errorPin, LOW);
    delay(wait);
    digitalWrite(errorPin, HIGH);
    delay(wait);
  }
  digitalWrite(errorPin, LOW);
}

void loop() {
  // Wait a few seconds between measurements.
  delay(2000);

  bool ok = true;
  ok = incubatorLoop() && ok;
  ok = cookerLoop() && ok;
  if (!ok) {
    errorBlink(3, 100);
    errorBlink(3, 200);
    errorBlink(3, 100);
  }
}

bool cookerLoop() {
  Serial.print("Updating liquid temperature sensor...");
  tempSensor.requestTemperatures();
  int t = tempSensor.getTempCByIndex(0);
  Serial.print(t);
  if (t < cookerLow) {
    digitalWrite(cookerPin, LOW);
    Serial.print(", so we're below cooker threshold");
  } else if (t > cookerHigh) {
    digitalWrite(cookerPin, HIGH);
    Serial.print(", so we're above cooker threshold");
  }
  Serial.print("\n");
  return true;
}

bool incubatorLoop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float t = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from incubator temp sensor, disabling incubator.");
    digitalWrite(incubatorPin, HIGH);
    return false;
  }

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *F\t");

  if (t < incubatorLow) {
    digitalWrite(incubatorPin, LOW);
    Serial.print("below incubator threshold");
  } else if (t > incubatorHigh) {
    digitalWrite(incubatorPin, HIGH);
    Serial.print("above incubator threshold");
  }
  Serial.print("\n");

  return true;
}


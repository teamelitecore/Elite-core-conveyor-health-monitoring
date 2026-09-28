#include <Wire.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_SSD1306.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "HX711.h"

#define RFID_SS_PIN 5
#define RFID_RST_PIN 4

#define HX711_DT_PIN 32
#define HX711_SCK_PIN 33

#define ONE_WIRE_BUS 27

#define POT_PIN 34

#define ENCODER_CLK 14
#define ENCODER_DT 12

#define BUZZER_PIN 2

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN);
HX711 scale;
Adafruit_MPU6050 mpu;
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, -1);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

int lastClkState = HIGH;
int encoderPos = 0;

float temperatureC = 0.0;
float totalAccel = 0.0;

long weightRaw = 0;

float tempThreshold = 50.0;
float accelThreshold = 15.0;
long weightThreshold = 250000;

bool tempAlert = false;
bool accelAlert = false;
bool weightAlert = false;
bool alertTriggered = false;

void setup() {

  Serial.begin(115200);
  delay(500);

  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);

  lastClkState = digitalRead(ENCODER_CLK);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED initialization failed!");
  } 
  else {

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0, 0);
    display.println("SIH EDGE NODE");

    display.setCursor(0, 15);
    display.println("Initializing...");

    display.display();

    delay(1500);
  }

  SPI.begin(18, 19, 23, 5);

  rfid.PCD_Init();

  Serial.println("RFID initialized");

  if (!mpu.begin()) {

    Serial.println("MPU6050 NOT FOUND!");

  } 
  else {

    Serial.println("MPU6050 OK");

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  }

  scale.begin(HX711_DT_PIN, HX711_SCK_PIN);

  Serial.println("HX711 initialized");

  tempSensor.begin();

  Serial.println("DS18B20 initialized");

  tone(BUZZER_PIN, 1000, 200);

  Serial.println("-----------------------------");
  Serial.println("SYSTEM READY");
  Serial.println("Potentiometer controls limits");
  Serial.println("-----------------------------");

  delay(1000);
}

void loop() {

  // =========================
  // POTENTIOMETER
  // =========================

  int potValue = analogRead(POT_PIN);

  tempThreshold = map(potValue, 0, 4095, 20, 80);

  accelThreshold = map(potValue, 0, 4095, 100, 200);
  accelThreshold = accelThreshold / 10.0;

  weightThreshold = map(potValue, 0, 4095, 10000, 500000);


  // =========================
  // TEMPERATURE SENSOR
  // =========================

  tempSensor.requestTemperatures();

  temperatureC = tempSensor.getTempCByIndex(0);

  if (temperatureC == DEVICE_DISCONNECTED_C) {

    Serial.println("Temperature sensor error!");

    tempAlert = false;

  } 
  else {

    tempAlert = temperatureC > tempThreshold;
  }


  // =========================
  // MPU6050
  // =========================

  sensors_event_t a;
  sensors_event_t g;
  sensors_event_t sensorTemp;

  if (mpu.getEvent(&a, &g, &sensorTemp)) {

    totalAccel =
      sqrt(
        (a.acceleration.x * a.acceleration.x) +
        (a.acceleration.y * a.acceleration.y) +
        (a.acceleration.z * a.acceleration.z)
      );

    accelAlert = totalAccel > accelThreshold;
  }


  // =========================
  // HX711
  // =========================

  if (scale.is_ready()) {

    weightRaw = scale.read();

    weightAlert = weightRaw > weightThreshold;

  } 
  else {

    weightRaw = 0;
    weightAlert = false;
  }


  // =========================
  // ROTARY ENCODER
  // =========================

  int currentClkState = digitalRead(ENCODER_CLK);

  if (currentClkState != lastClkState &&
      currentClkState == LOW) {

    if (digitalRead(ENCODER_DT) != currentClkState) {

      encoderPos++;

    } 
    else {

      encoderPos--;
    }
  }

  lastClkState = currentClkState;


  // =========================
  // RFID
  // =========================

  if (rfid.PICC_IsNewCardPresent() &&
      rfid.PICC_ReadCardSerial()) {

    Serial.print("RFID UID: ");

    for (byte i = 0; i < rfid.uid.size; i++) {

      Serial.print(rfid.uid.uidByte[i], HEX);
      Serial.print(" ");
    }

    Serial.println();

    tone(BUZZER_PIN, 2000, 100);

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
  }


  // =========================
  // FINAL ALERT
  // =========================

  alertTriggered =
    tempAlert ||
    accelAlert ||
    weightAlert;


  if (alertTriggered) {

    tone(BUZZER_PIN, 1500, 100);

  }


  // =========================
  // SERIAL MONITOR
  // =========================

  Serial.println();
  Serial.println("========== STATUS ==========");

  Serial.print("Potentiometer: ");
  Serial.println(potValue);

  Serial.print("Temperature: ");
  Serial.print(temperatureC, 1);
  Serial.print(" C / Limit: ");
  Serial.print(tempThreshold, 0);
  Serial.print(" C   ");

  if (tempAlert)
    Serial.println("ALERT");
  else
    Serial.println("OK");


  Serial.print("Acceleration: ");
  Serial.print(totalAccel, 1);
  Serial.print(" m/s2 / Limit: ");
  Serial.print(accelThreshold, 1);
  Serial.print("   ");

  if (accelAlert)
    Serial.println("ALERT");
  else
    Serial.println("OK");


  Serial.print("HX711 Raw: ");
  Serial.print(weightRaw);
  Serial.print(" / Limit: ");
  Serial.print(weightThreshold);
  Serial.print("   ");

  if (weightAlert)
    Serial.println("ALERT");
  else
    Serial.println("OK");


  Serial.print("Encoder: ");
  Serial.println(encoderPos);


  // =========================
  // OLED DISPLAY
  // =========================

  display.clearDisplay();

  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);


  // STATUS

  display.setCursor(0, 0);

  if (alertTriggered)
    display.println("!!! ALERT !!!");
  else
    display.println("SYSTEM NORMAL");


  // TEMPERATURE

  display.setCursor(0, 12);

  display.print("T:");
  display.print(temperatureC, 1);
  display.print("/");
  display.print(tempThreshold, 0);
  display.print("C ");

  if (tempAlert)
    display.print("LIMIT");
  else
    display.print("OK");


  // ACCELERATION

  display.setCursor(0, 24);

  display.print("A:");
  display.print(totalAccel, 1);
  display.print("/");
  display.print(accelThreshold, 1);
  display.print(" ");

  if (accelAlert)
    display.print("LIMIT");
  else
    display.print("OK");


  // LOAD

  display.setCursor(0, 36);

  display.print("L:");
  display.print(weightRaw);


  // LOAD LIMIT

  display.setCursor(0, 48);

  display.print("Limit:");
  display.print(weightThreshold);


  display.display();


  delay(150);
}

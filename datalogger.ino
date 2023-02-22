#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include "RTClib.h"
// for SD card
#include "FS.h"
#include "SD.h"
#include "SPI.h"
// for MPU
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>


Adafruit_BMP085 bmp;
RTC_DS3231 rtc;
Adafruit_MPU6050 mpu;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

void setupBmp()
{
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    while (1) {}
  }
  Serial.println("BMP180 Initialized.");
}

void setupRtc()
{
  if (! rtc.begin()) {
  Serial.println("Couldn't find RTC");
  while (1);
  }
  // rtc.adjust(DateTime(_DATE, __TIME_));
  Serial.println("RTC Initialized.");
}

void setupMpu()
{
  if (!mpu.begin()) {
  Serial.println("Sensor init failed");
  while (1)
      yield();
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);

  Serial.println("MPU6050 successfully initialized.");
}

void setupSdCard()
{
   if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
}

void getBmpReadings(float* temp, float* altitude, int* pressure)
{
  *altitude = bmp.readAltitude();
  *pressure = bmp.readPressure();
  *temp = bmp.readTemperature();
}


void appendFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        Serial.println("Failed to open file for appending");
        return;
    }
    if(file.print(message)){
        Serial.println("Message appended");
    } else {
        Serial.println("Append failed");
    }
    file.close();
}

void setup() {
  Serial.begin(115200);
  // put your setup code here, to run once:
  setupBmp();
  setupSdCard();
  setupMpu();
  setupRtc();
}

void loop() {
  float temp, altitude;
  int pressure;

  getBmpReadings(&temp, &altitude, &pressure);

  sensors_event_t a, g, temp2;
  mpu.getEvent(&a, &g, &temp2);

  char buffer[200];
  DateTime now = rtc.now();
  sprintf(buffer, "%d:%d:%d %f, %f, %d, %f, %f, %f, %f, %f, %f\n", now.hour(), now.minute(), now.second(), temp, altitude, pressure,
    a.acceleration.x,
    a.acceleration.y,
    a.acceleration.z,
    g.gyro.x,
    g.gyro.y,
    g.gyro.z
  );
  Serial.println(buffer); // replace with SDCard appendfile
  // appendFile(SD, "/readings.csv", buffer);
  delay(500);
}
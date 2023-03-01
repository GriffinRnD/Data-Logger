// ********************************************* TGI DATALOGGER ***********************************************


// ************************************************************************************************************
// ******************************************** BASIC LIBRARIES ***********************************************
// ************************************************************************************************************

#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <SPI.h>
#include "RTClib.h"


// ************************************************************************************************************
// ********************************************* SD CARD SETUP ************************************************
// ************************************************************************************************************
#include "FS.h"
#include "SD.h"


// ************************************************************************************************************
// *********************************************** MPU SETUP **************************************************
// ************************************************************************************************************

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#define MPU_ADDR 0x69
Adafruit_MPU6050 mpu;


// ************************************************************************************************************
// *********************************************** BMP SETUP **************************************************
// ************************************************************************************************************

Adafruit_BMP085 bmp;


// ************************************************************************************************************
// *********************************************** RTC SETUP **************************************************
// ************************************************************************************************************
RTC_DS3231 rtc;


// ************************************************************************************************************
// ******************************************* OLED DISPLAY SETUP *********************************************
// ************************************************************************************************************

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library.
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET 4         // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C  ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


// ************************************************************************************************************
// ********************************************** VARIABLES ***************************************************
// ************************************************************************************************************

char daysOfTheWeek[7][12] = { "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday" };
int inputPin1 = 15;
int inputPin2 = 2;
int inputPin3 = 4;
int buttonPin = 39;
bool recording = false;
int16_t cross = 158;

// ************************************************************************************************************
// **************************************** SENSORS SETUP FUNCTIONS *******************************************
// ************************************************************************************************************

void setupBmp() {
  if (!bmp.begin()) {
    Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
    //display.clearDisplay();
    //display.setCursor(0, 0);  // Start at top-left corner
    display.println(F("BAROMETER : FAILED"));
    display.display();
    delay(500);
    while (1) {
    }
  }
  //display.clearDisplay();
  //display.setCursor(0, 0);  // Start at top-left corner
  display.println(F("BAROMETER : ./"));
  display.display();
  delay(500);
  Serial.println("BMP180 Initialized.");
}

void setupRtc() {
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    //display.clearDisplay();
    //display.setCursor(0, 0);  // Start at top-left corner
    display.println(F("CLOCK : FAILED"));
    display.display();
    delay(1500);
    while (1)
      ;
  }
  rtc.adjust(DateTime(__DATE__, __TIME__));
  //display.clearDisplay();
  //display.setCursor(0, 0);  // Start at top-left corner
  display.println(F("CLOCK : ./"));
  display.display();
  delay(1500);
  Serial.println("RTC Initialized.");
}

void setupMpu() {
  if (!mpu.begin(MPU_ADDR)) {
    Serial.println("Gyro Sensor failed");
    //display.clearDisplay();
    //display.setCursor(0, 0);  // Start at top-left corner
    display.println(F("GYROSCOPE : FAILED"));
    display.display();
    delay(500);
    while (1)
      yield();
  }
  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
  //display.clearDisplay();
  //display.setCursor(0, 0);  // Start at top-left corner
  display.println(F("GYROSCOPE : ./"));
  display.display();
  delay(500);
  Serial.println("MPU6050 successfully initialized.");
}

void setupDisp() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(1000); 
  display.clearDisplay();
  display.setTextSize(3);               // Normal 3:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(30, 10);              // Start at top-left corner
  display.println(F("TGI"));
  display.setTextSize(2); 
  display.println(F("Datalogger"));
  display.display();
  delay(2000);
  display.setTextSize(1); 
  display.clearDisplay();              // Normal 3:1 pixel scale  // Draw white text
  display.setCursor(0, 0);              // Start at top-left corner
  display.println(F("DISPLAY : ./"));
  display.display();
  delay(500);
}


// ************************************************************************************************************
// ************************************ FILE & DATA RECORD SETUP **********************************************
// ************************************************************************************************************

void setupSdCard() {
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    //display.clearDisplay();
    //display.setCursor(0, 0);  // Start at top-left corner
    display.println(F("MEMORY : FAILED"));
    display.display();
    delay(500);
    while (1)
      ;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if (cardType == CARD_MMC) {
    Serial.println("MMC");
  } else if (cardType == CARD_SD) {
    Serial.println("SDSC");
  } else if (cardType == CARD_SDHC) {
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  //display.clearDisplay();
  //display.setCursor(0, 0);  // Start at top-left corner
  display.print(F("MEMORY : ./ "));
  //display.display();
  //delay(1500);
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  //display.clearDisplay();
  //display.setCursor(0, 0);  // Start at top-left corner
  display.printf("(%lluMB)\n", cardSize);
  display.display();
  delay(500);
}

void setupExcelHead(){
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0,0);          
  char buffer[200];
  DateTime now = rtc.now();
  sprintf(buffer, "Recorded on, -> , %d:%d:%d, At, ->, %d:%d:%d\n",
    now.day(), now.month(), now.year(), 
    now.hour(), now.minute(), now.second());
  Serial.println(buffer);  // replace with SDCard appendfile
  appendFile(SD, "/readings.csv", buffer);
  display.clearDisplay();
  display.setCursor(0, 0);  // Start at top-left corner
  display.setTextSize(2); 
  display.println(F("NEW ENTRY IS SET"));
  display.println(F(" "));
  display.display();
  delay(500);
  display.setTextSize(1); 
  display.printf("Date : %d:%d:%d\n",now.day(), now.month(), now.year());
  display.display();
  delay(500);
  display.printf("Time : %d:%d:%d\n",now.hour(), now.minute(), now.second());
  display.display();
  delay(2000);
  Serial.println("New entry created");
}

// ************************************************************************************************************
// ************************************** SENSORS READINGS FUNCTIONS ******************************************
// ************************************************************************************************************

void getBmpReadings(float *temp, float *altitude, int *pressure) {
  *altitude = bmp.readAltitude();
  *pressure = bmp.readPressure();
  *temp = bmp.readTemperature();
}


// ************************************************************************************************************
// ************************************ FILE & DATA RECORD FUNCTIONS ******************************************
// ************************************************************************************************************

void notRecordingSD(){
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(2);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0,10);              // Start at top-left corner
  display.println(F("NOT \nRECORDING"));
  display.display();
  Serial.println("Not Recording");
}

void appendFile(fs::FS &fs, const char *path, const char *message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}

void recordingSD(){
    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }
  display.clearDisplay();
  display.setTextSize(2);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0, 20);              // Start at top-left corner
  display.println(F("RECORDING\n..."));
  display.display();
  Serial.println("Recording...");
    display.display();
}


// ************************************************************************************************************
// **************************************** VOID SETUP ********************************************************
// ************************************************************************************************************

void setup() {
  Serial.begin(115200);
  setupDisp();
  setupBmp();
  setupSdCard();
  setupMpu();
  setupRtc();
  pinMode(inputPin1, INPUT);
  pinMode(inputPin2, INPUT);
  pinMode(inputPin3, INPUT);
  pinMode(buttonPin, INPUT);
  setupExcelHead();
}


// ************************************************************************************************************
// **************************************** VOID LOOP *********************************************************
// ************************************************************************************************************

void loop() {
  float temp, altitude;
  int pressure;

  getBmpReadings(&temp, &altitude, &pressure);

  sensors_event_t a, g, temp2;
  mpu.getEvent(&a, &g, &temp2);

  int pwmValue1 = pulseIn(inputPin1, HIGH);
  int pwmValue2 = pulseIn(inputPin2, HIGH);
  int pwmValue3 = pulseIn(inputPin3, HIGH);

  char buffer[200];
  DateTime now = rtc.now();

  if (digitalRead(buttonPin) == LOW) {
    recording = !recording;
    delay(200);
  }

  if (recording) {
    recordingSD();
    sprintf(buffer, "%d:%d:%d, %f, %f, %d, %d, %d, %d, %f, %f, %f, %f, %f, %f\n", now.hour(), now.minute(), now.second(), temp, altitude, pressure,
            pwmValue1,
            pwmValue2,
            pwmValue3,
            a.acceleration.x,
            a.acceleration.y,
            a.acceleration.z,
            g.gyro.x,
            g.gyro.y,
            g.gyro.z);
    Serial.println(buffer);  // replace with SDCard appendfile
    appendFile(SD, "/readings.csv", buffer);
  } 
  else {
    notRecordingSD();
  }
}


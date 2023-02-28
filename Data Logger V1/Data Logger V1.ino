#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_BMP180.h>
#include <Adafruit_SSD1306.h>
#include <SD.h>

#define OLED_ADDR 0x3C
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST 16

Adafruit_MPU6050 mpu;
Adafruit_BMP180 bmp;
Adafruit_SSD1306 display(OLED_ADDR, OLED_SDA, OLED_SCL, OLED_RST);

File dataFile;
int currentSensorPin1 = 34;
int currentSensorPin2 = 35;
int currentSensorPin3 = 32;
int currentSensorPin4 = 33;
int currentSensorPin5 = 25;
int buttonPin = 35;
int pwmInputPin1 = 26;
int pwmInputPin2 = 27;
int pwmInputPin3 = 14;
int pwmInputPin4 = 12;
int pwmInputPin5 = 13;

bool recording = false;

// All the required functions
float readCurrent(int pin) {
  // Read raw value from current sensor
  if(pin==1)
  int sensorValue = analogRead(currentSensorPin1);
  if(pin==2)
  int sensorValue = analogRead(currentSensorPin2);
  if(pin==3)
  int sensorValue = analogRead(currentSensorPin3);
  if(pin==4)
  int sensorValue = analogRead(currentSensorPin4);
  if(pin==5)
  int sensorValue = analogRead(currentSensorPin5);

  // Convert raw value to current (in amps)
  float current = (sensorValue - 512) * 0.0244;

  return current;
}
float getBatteryVoltage() {
  // Read raw value from voltage divider
  int sensorValue = analogRead(36);

  // Convert raw value to voltage
  float voltage = (sensorValue / 4095.0) * 3.3 * 2;

  return voltage;
}

// Main Code
void setup() {
  Serial.begin(115200);

  // Initialize sensors
  if (!mpu.begin(0x68)) {
    Serial.println("Error initializing MPU6050!");
    while (1);
  }
  if (!bmp.begin()) {
    Serial.println("Error initializing BMP180!");
    while (1);
  }

  // Initialize OLED display
  display.begin(SSD1306_SWITCHCAPVCC);
  display.clearDisplay();
  display.display();

  // Initialize SD card
  if (!SD.begin(4)) {
    Serial.println("Error initializing SD card!");
    while (1);
  }

  // Set up button pin as an input
  pinMode(buttonPin, INPUT);

  // Set up input pin as an input
  pinMode(inputPin, INPUT);
}

void loop() {
  // Read data from sensors
  sensors_event_t accel, gyro, temp;
  mpu.getEvent(&accel, &gyro, &temp);
  float pressure = bmp.readPressure();
  float temperature = bmp.readTemperature();

  float current1 = readCurrent(1);
  float current2 = readCurrent(2);
  float current3 = readCurrent(3);
  float current4 = readCurrent(4);
  float current5 = readCurrent(5);

  int pwmValue1 = pulseIn(pwmInputPin1, HIGH);
  int pwmValue2 = pulseIn(pwmInputPin2, HIGH);
  int pwmValue3 = pulseIn(pwmInputPin3, HIGH);
  int pwmValue4 = pulseIn(pwmInputPin4, HIGH);
  int pwmValue5 = pulseIn(pwmInputPin5, HIGH);

  // Update OLED display with battery level and recording status
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Battery: ");
  display.print(getBatteryVoltage());
  display.print("V");
  if (recording) {
    display.setCursor(0, 16);
    display.print("Recording...");
  }
  display.display();

  // Check if button is pressed to toggle recording
  if (digitalRead(buttonPin) == LOW) {
    recording = !recording;
    delay(200);
  }

  if (recording) {
    // Write data to SD card
    dataFile = SD.open("data.csv", FILE_WRITE);
    dataFile.println(pwmValue1);
    dataFile.print(",");
    dataFile.println(pwmValue2);
    dataFile.print(",");
    dataFile.println(pwmValue3;
    dataFile.print(",");
    dataFile.println(pwmValue4);
    dataFile.print(",");
    dataFile.println(pwmValue5);
    dataFile.print(",");
    dataFile.print(accel.acceleration.x);
    dataFile.print(",");
    dataFile.print(accel.acceleration.y);
    dataFile.print(",");
    dataFile.print(accel.acceleration.z);
    dataFile.print(",");
    dataFile.print(gyro.gyroscope.x);
    dataFile.print(",");
    dataFile.print(gyro.gyroscope.y);
    dataFile.print(",");
    dataFile.print(gyro.gyroscope.z);
    dataFile.print(",");
    dataFile.print(current1);
    dataFile.print(",");
    dataFile.print(current2);
    dataFile.print(",");
    dataFile.print(current3);
    dataFile.print(",");
    dataFile.print(current4);
    dataFile.print(",");
    dataFile.print(current5);
    dataFile.print(",");
    dataFile.print(temperature);
    dataFile.print(",");
    dataFile.println(pressure);
    dataFile.println();
    dataFile.close();
  }
}
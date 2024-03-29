// Basic demo for accelerometer readings from Adafruit MPU6050
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>

/** The clock select pin for the SD card module */
#define CS_PIN 5

Adafruit_MPU6050 mpu;
Adafruit_BMP280 bmp; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();

// Timekeeping
unsigned long startTime, currentTime, elapsedTime;

void setup(void)
{
  // Start timekeeping
  startTime = millis();

  // Check the module is connected
  if (!SD.begin(CS_PIN))
  {
    Serial.println("Error, SD Initialization Failed");
    return;
  }

  // Serial comunications (this will go away when in production)
  Serial.begin(9600);
  while (!Serial)
  {
    delay(10); // will pause Zero, Leonardo, etc until serial console opens
  }

  // MPU280 start
  unsigned status;
  status = bmp.begin(0x76);
  if (!status)
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    Serial.print("SensorID was: 0x");
    Serial.println(bmp.sensorID(), 16);
    Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
    Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
    Serial.print("        ID of 0x60 represents a BME 280.\n");
    Serial.print("        ID of 0x61 represents a BME 680.\n");
    while (1)
      delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  bmp_temp->printSensorDetails();

  /* END OF BMP280 */

  /* START OF MPU6050 */

  Serial.println("Adafruit MPU6050 test!");
  // Try to initialize!
  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange())
  {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange())
  {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth())
  {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}

void loop()
{

  /* Get elapsed time since beginning, in seconds */
  currentTime = millis() - startTime;

  // Creating file object
  File testFile = SD.open("/TEST_06.txt", FILE_APPEND);

  // Writing currentTime
  testFile.print(currentTime);
  testFile.print(",");

  /* Get new sensor events with the readings */
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  /* Print out the values */

  /* Serial.print("Acceleration X: ");
  Serial.print(a.acceleration.x); */
  testFile.print(a.acceleration.x);
  testFile.print(",");
  /* Serial.print(", Y: ");
  Serial.print(a.acceleration.y); */
  testFile.print(a.acceleration.y);
  testFile.print(",");
  /* Serial.print(", Z: ");
  Serial.print(a.acceleration.z); */
  testFile.print(a.acceleration.z);
  testFile.print(",");
  /* Serial.println(" m/s^2"); */

  /* Serial.print("Rotation X: ");
  Serial.print(g.gyro.x); */
  testFile.print(g.gyro.x);
  testFile.print(",");
  /* Serial.print(", Y: ");
  Serial.print(g.gyro.y); */
  testFile.print(g.gyro.y);
  testFile.print(",");
  /* Serial.print(", Z: ");
  Serial.print(g.gyro.z); */
  testFile.print(g.gyro.z);
  testFile.print(",");
  /* Serial.println(" rad/s"); */

  /* Serial.print("Temperature: ");
  Serial.print(temp.temperature); */
  testFile.print(temp.temperature);
  testFile.print(",");
  /* Serial.println(" degC");

  Serial.println(""); */

  sensors_event_t temp_event, pressure_event;
  bmp_temp->getEvent(&temp_event);
  bmp_pressure->getEvent(&pressure_event);

  /* Serial.print(F("Temperature = "));
  Serial.print(temp_event.temperature); */
  testFile.print(temp_event.temperature);
  testFile.print(",");
  /*  Serial.println(" *C"); */

  /* Serial.print(F("Pressure = "));
  Serial.print(pressure_event.pressure); */
  testFile.print(pressure_event.pressure);
  testFile.print(",");
  /* Serial.println(" hPa"); */

  /* Serial.print(F("Approx altitude = "));
  Serial.print(bmp.readAltitude(1013)); */
  // The "1019.66" is the pressure(hPa) at sea level in day in your region
  testFile.println(bmp.readAltitude(1013));
  // testFile.print(","); // If you don't know it, modify it until you get your current altitude
  /* Serial.println(" m"); */

  /* Serial.println(); */
  Serial.print(currentTime);
  Serial.print(">>>");
  testFile.close();
  Serial.println("Loop foi.");
  delay(5);
}
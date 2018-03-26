/*
* Project Greenuino
* Description:
* Author: James Lafa
* Date:
*/

#include <application.h>
#include <spark_wiring_i2c.h>
#include <ClosedCube_SHT31D.h>
#include <Adafruit_TSL2591.h>

// Status LEDs
int ledBlue = D4;
int ledGreen = D5;
int ledRed = D6;

// Temperature and humidity
SHT31D_CC::ClosedCube_SHT31D sht31d;
double temperature = 0;
double humidity = 0;

// Light
Adafruit_TSL2591 tsl = Adafruit_TSL2591(2591);
int lightLux = 0;
int lightInfraRed = 0;
int lightFullSpectrum = 0;

// Setup the program. it's run only once
void setup() {
  // Init status LEDs
  pinMode(ledBlue, OUTPUT);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  digitalWrite(ledBlue, LOW);
  digitalWrite(ledRed, LOW);
  digitalWrite(ledGreen, LOW);

  bool setupError = false;

  // Declare Particle variables
  Particle.variable("temp", temperature);
  Particle.variable("humidity", humidity);
  Particle.variable("l_lux", lightLux);
  Particle.variable("l_inf_red", lightInfraRed);
  Particle.variable("l_full_spec", lightFullSpectrum);

  // Init SHT31D
  int sht31Connection = sht31d.begin(0x44);
  if (sht31Connection == SHT31D_CC::NO_ERROR) {
    Particle.publish("setup/SHT31D/status", "OK");
  }
  else{
    setupError = true;
    Particle.publish("setup/SHT31D/status", sht31Connection);
  }

  // Init TSL2591
  if (tsl.begin()) {
    tsl.setGain(TSL2591_GAIN_MED);
    tsl.setTiming(TSL2591_INTEGRATIONTIME_300MS);
    String configuration = "gain=";
    configuration.concat(TSL2591_GAIN_MED);
    configuration.concat(",timing=");
    configuration.concat(TSL2591_INTEGRATIONTIME_300MS);
    Particle.publish("setup/TSL2591/status", "OK");
    Particle.publish("setup/TSL2591/config", configuration);
  }
  else{
    setupError = true;
    Particle.publish("setup/TSL2591/status", "Unknown error");
  }

  if(setupError){
    digitalWrite(ledRed, HIGH);
  }
  else{
    digitalWrite(ledGreen, HIGH);
  }
}

// Read air temperature and humidity
void readSHT31(){
  SHT31D_CC::SHT31D sht31Result = sht31d.readTempAndHumidity(SHT31D_CC::REPEATABILITY_LOW, SHT31D_CC::MODE_CLOCK_STRETCH, 50);

  if (sht31Result.error == SHT31D_CC::NO_ERROR) {
    temperature = sht31Result.t;
    humidity = sht31Result.rh;
    Particle.variable("temperature", temperature);
    Particle.variable("humidity", humidity);
  }
  else{
    Particle.publish("reading/SHT31D/status", sht31Result.error);
    digitalWrite(ledRed, HIGH);
  }
}

// Read light characteristics
void readTSL2591(){
  uint32_t lum = tsl.getFullLuminosity();
  uint16_t ir, full;
  ir = lum >> 16;
  full = lum & 0xFFFF;
  lightLux = tsl.calculateLux(full, ir);
  lightInfraRed = ir;
  lightFullSpectrum = full;
  Particle.variable("l_lux", lightLux);
  Particle.variable("l_inf_red", lightInfraRed);
  Particle.variable("l_full_spec", lightFullSpectrum);
}

// Read data from every sensors
void readSensors(){
  digitalWrite(ledBlue, HIGH);
  readSHT31();
  readTSL2591();
  delay(100);
  digitalWrite(ledBlue, LOW);
}

void loop() {
  readSensors();
  delay(5000);
}

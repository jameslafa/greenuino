/*
* Project Greenuino
* Description:
* Author: James Lafa
* Date:
*/

#include <ClosedCube_SHT31D.h>
#include <application.h>
#include <spark_wiring_i2c.h>

// Status LEDs
int ledGreen = D5;
int ledRed = D6;

// Temperature and humidity
SHT31D_CC::ClosedCube_SHT31D sht31d;
double temp = 0;
double humidity = 0;

// Setup the program. it's run only once
void setup() {
  // Init status LEDs
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  digitalWrite(ledRed, LOW);
  digitalWrite(ledGreen, LOW);

  // Declare Particle variables
  Particle.variable("temp", temp);
  Particle.variable("humidity", humidity);

  // Init SHT31D
  int sht31Connection = sht31d.begin(0x44);
  if (sht31Connection != SHT31D_CC::NO_ERROR) {
    digitalWrite(ledRed, HIGH);
    Particle.publish("SHT31D-SETUP-ERROR", sht31Connection);
  }
  else{
    Particle.publish("SHT31D-SETUP", "Sensor connected");
  }
}

void readSHT31(){
  digitalWrite(ledGreen, HIGH);
  digitalWrite(ledRed, LOW);
  SHT31D_CC::SHT31D sht31Result = sht31d.readTempAndHumidity(SHT31D_CC::REPEATABILITY_LOW, SHT31D_CC::MODE_CLOCK_STRETCH, 50);

  if (sht31Result.error == SHT31D_CC::NO_ERROR) {
    temp = sht31Result.t;
    humidity = sht31Result.rh;
    Particle.variable("temp", temp);
    Particle.variable("humidity", humidity);
  }
  else{
    Particle.publish("SHT31D-READ", sht31Result.error);    
    digitalWrite(ledRed, HIGH);
  }
  delay(100);
  digitalWrite(ledGreen, LOW);
}

void loop() {
  readSHT31();
  delay(2000);
}

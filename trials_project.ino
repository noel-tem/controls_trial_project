#include <Wire.h>
#include <math.h>
#include <Servo.h>

#define mpu 0x68

Servo myservo;

void setup() {
  //setting up mpu
  Wire.begin();
  Wire.beginTransmission(mpu);
  Wire.write(0x6B);
  Wire.write(0); //waking up the mpu
  Wire.endTransmission(true);

  Serial.begin(9600);

  myservo.attach(9);

}

void loop() {
  //reading acceleration of x axis
  Wire.beginTransmission(mpu);
  Wire.write(0x3b);
  Wire.endTransmission();
  Wire.requestFrom(mpu, 2); //requesting 2 bytes at 3b (ACCEL_XOUT)
  uint8_t ACCEL_X_H = Wire.read();
  uint8_t ACCEL_X_L = Wire.read();

  int16_t ACCEL_X_OUT = (ACCEL_X_H << 8) | ACCEL_X_L; //combining high and low bits
  float xAcc = (float)ACCEL_X_OUT / 16384.0; //converting raw data to g using LSB sens
  
  //Serial.print(xAcc);
  //Serial.println();

  //reading acceleration of y axis
  Wire.beginTransmission(mpu);
  Wire.write(0x3d);
  Wire.endTransmission();
  Wire.requestFrom(mpu, 2);
  uint8_t ACCEL_Y_H = Wire.read();
  uint8_t ACCEL_Y_L = Wire.read();

  int16_t ACCEL_Y_OUT = (ACCEL_Y_H << 8) | ACCEL_Y_L; //combining high and low bits
  float yAcc = (float)ACCEL_Y_OUT / 16384.0; //converting raw data to g using LSB sens

  //Serial.print(yAcc);
  //Serial.println();
  
  //computing acceleration due to gravity using angle between xAcc and yAcc
  float gAcc = atan2(yAcc, xAcc);

  //converting from rads to degrees
  float gDeg = gAcc * 180.0 / PI;
  float servoDeg = 90.0 + gDeg;

  Serial.print(servoDeg); 
  Serial.println();

  myservo.write((int)servoDeg);
}

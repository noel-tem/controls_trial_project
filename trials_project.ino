#include <Wire.h>
#include <math.h>
#include <Servo.h>

#define mpu 0x68

Servo myservo;

float offset = 0;

void setup() {
  //setting up mpu
  Wire.begin();
  Wire.beginTransmission(mpu);
  Wire.write(0x6B); //PWR_MGMT
  Wire.write(0); //waking up the mpu
  Wire.endTransmission(true);

  Serial.begin(250000);

  myservo.attach(9);

}

void loop() {
  //reading acceleration of x axis
  Wire.beginTransmission(mpu);
  Wire.write(0x3b); //accelerometer output registers are 3b through 3f
  Wire.endTransmission();
  Wire.requestFrom(mpu, 2); //requesting 2 bytes; gets both high and low for full 16 bit value
  uint8_t ACCEL_X_H = Wire.read();
  uint8_t ACCEL_X_L = Wire.read();
  int16_t ACCEL_X_OUT = (ACCEL_X_H << 8) | ACCEL_X_L; //combining high and low bits
  float xAcc = (float)ACCEL_X_OUT / 16384.0; //converting raw data to g using LSB sens

  //reading acceleration of y axis
  Wire.beginTransmission(mpu);
  Wire.write(0x3d);
  Wire.endTransmission();
  Wire.requestFrom(mpu, 2);
  uint8_t ACCEL_Y_H = Wire.read();
  uint8_t ACCEL_Y_L = Wire.read();
  int16_t ACCEL_Y_OUT = (ACCEL_Y_H << 8) | ACCEL_Y_L; //combining high and low bits
  float yAcc = (float)ACCEL_Y_OUT / 16384.0; //converting raw data to g using LSB sens

  //reading acceleration of z axis (not needed for operation, but the trial project document asks for it)
  Wire.beginTransmission(mpu);
  Wire.write(0x3f);
  Wire.endTransmission();
  Wire.requestFrom(mpu, 2);
  uint8_t ACCEL_Z_H = Wire.read();
  uint8_t ACCEL_Z_L = Wire.read();
  int16_t ACCEL_Z_OUT = (ACCEL_Z_H << 8) | ACCEL_Z_L;
  float zAcc = (float)ACCEL_Z_OUT / 16384.0;
  
  //computing acceleration due to gravity using angle between xAcc and yAcc
  float gAcc = atan2(yAcc, xAcc);

  //getting offset from serial monitor
  if(Serial.available() > 0){ //if there are more than 0 bytes available to be read
    String input = Serial.readStringUntil('\n');
    offset = input.toFloat();
  }

  //converting from rads to degrees
  float gDeg = gAcc * 180.0 / PI;

  //calculating serco position
  float servoDeg = 97.0 + gDeg + offset;
  //               ^^^^ was 90 before, but the servo was a little off from pointing directly upwards;
  //                    this 7 degree addition came from testing with the serial monitor offset

  //printing to serial monitor
  Serial.print("X: "); Serial.print(xAcc); Serial.print(" Y: "); Serial.print(yAcc);
  Serial.print(" Z: "); Serial.print(zAcc);
  Serial.print(" Servo: "); Serial.print(servoDeg);
  Serial.println();

  myservo.write((int)servoDeg);
}

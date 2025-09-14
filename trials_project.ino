#include <Wire.h>
#include <math.h>
#include <Servo.h>

#define mpu 0x68
#define SENS_SF 16384.0
Servo myservo;
float offset = 0.0;

void setup() {
  //setting up mpu
  Wire.begin();
  Wire.beginTransmission(mpu);
  Wire.write(0x6b); //PWR_MGMT
  Wire.write(0); //waking up the mpu
  Wire.endTransmission(true); //stop = true
  Serial.begin(19200);
  myservo.attach(9);
}

void loop() {
  //reading acceleration of x axis
  Wire.beginTransmission(mpu);
  Wire.write(0x3b); //accelerometer output registers are 3b through 3f
  Wire.endTransmission();
  Wire.requestFrom(mpu, 6); //requesting 6 bytes, reading accelerometer data of all axes
  uint8_t ACCEL_X_H = Wire.read();
  uint8_t ACCEL_X_L = Wire.read();
  uint8_t ACCEL_Y_H = Wire.read();
  uint8_t ACCEL_Y_L = Wire.read();
  uint8_t ACCEL_Z_H = Wire.read();
  uint8_t ACCEL_Z_L = Wire.read();

  //combining high and low bits, then converting to g using sensitivity scale factor
  float xAcc = float((ACCEL_X_H << 8) | ACCEL_X_L) / SENS_SF;
  float yAcc = float((ACCEL_Y_H << 8) | ACCEL_Y_L) / SENS_SF; 
  float zAcc = float((ACCEL_Z_H << 8) | ACCEL_Z_L) / SENS_SF;

  //computing angle between y and x acceleration and converting from radians to degrees
  float gAngle = atan2(yAcc, xAcc) * (180.0 / PI);

  //getting offset from serial monitor
  if(Serial.available() > 0){ 
    String input = Serial.readStringUntil('\n');
    offset = input.toFloat();
  }

  //calculating servo position
  float servoDeg = 97.0 + gAngle + offset;
  //               ^^^^ was 90 before, but the servo was a little off from pointing directly upwards
  //                    this 7 degree addition came from testing with the serial monitor offset

  //printing to serial monitor
  Serial.print("X: "); Serial.print(xAcc); Serial.print(" Y: "); Serial.print(yAcc);
  Serial.print(" Z: "); Serial.print(zAcc);
  Serial.print(" Servo: "); Serial.print(servoDeg);
  Serial.println();

  if (zAcc > 0.7){ //this if statement stops the servo from jittering when placed flat
    return;
  }
  else{
    myservo.write(servoDeg);
  }
  delay(15);
}
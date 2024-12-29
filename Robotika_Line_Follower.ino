#include <L298N.h>
#include <QTRSensors.h>

// Pin Motor
#define AIN1 2  
#define AIN2 4
#define BIN1 7
#define BIN2 8
#define PWMA 3
#define PWMB 6

// Threshold dan jarak
const int analogThreshold = 80;
const int FORWARD_MAX = 100;
const int BACKWARD_MAX = -65;

// Inisialisasi Motor
L298N motor1(PWMB, BIN2, BIN1); // Motor kiri
L298N motor2(PWMA, AIN2, AIN1); // Motor kanan

// Inisialisasi Sensor QTR
QTRSensors qtr;
const uint8_t SensorCount = 5;  // Jumlah sensor QTR
uint16_t sensorValues[SensorCount];
int threshold[SensorCount];
int sensor[5]={0, 0, 0, 0, 0};

// PID Parameters
float Kp = 5.0;  // Penguatan Proportional
float Ki = 0;// Penguatan Integral
float Kd = 1.25;   // Penguatan Derivative

const int analogFlameSensorPin = A5;
const int digitalFlameSensorLeftPin = 12;
const int digitalFlameSensorRightPin = 9;

// Status deteksi api
enum FireStatus {
  NO_FIRE,
  FIRE_FRONT,
  FIRE_LEFT,
  FIRE_RIGHT,
  FIRE_CLOSE
};

const int fanPin = 11;

int P, I, D, previousError = 0;
int PIDvalue;
int lsp, rsp;
int rspeed = 100;
int lfspeed = rspeed * 0.90490;

const int digitalFlameDetectorPin = 13;



void setup() {
  Serial.begin(9600);
  pinMode(analogFlameSensorPin, INPUT);
  pinMode(digitalFlameSensorLeftPin, INPUT);
  pinMode(digitalFlameSensorRightPin, INPUT);
  pinMode(fanPin, OUTPUT);
  digitalWrite(fanPin, HIGH);

  // qtr.setTypeAnalog();
  // qtr.setSensorPins((const uint8_t[]){A4, A3, A2, A1, A0}, SensorCount);
  
  // delay(500);
  // pinMode(LED_BUILTIN, OUTPUT);
  
  // digitalWrite(LED_BUILTIN, HIGH);

  // Serial.begin(115200);
  // for (uint16_t i = 0; i < 400; i++) {
  //   qtr.calibrate();
  // }
  // digitalWrite(LED_BUILTIN, LOW);

  // for (uint8_t i = 0; i < SensorCount; i++) {
  //   threshold[i] = (qtr.calibrationOn.minimum[i] + qtr.calibrationOn.maximum[i]) / 2;
  //   Serial.print(threshold[i]);
  //   Serial.print("  ");
  // }
  // Serial.println();
  // digitalWrite(fanPin, LOW);
  
  // delay(3000);
}

void loop() {
  int analogValue = analogRead(analogFlameSensorPin);
  int leftFlameDetected = digitalRead(digitalFlameSensorLeftPin);
  int rightFlameDetected = digitalRead(digitalFlameSensorRightPin);
  // Serial.println(analogValue);

  Serial.println();
  Serial.print("ESP Pin: ");
  Serial.print(digitalRead(digitalFlameDetectorPin));
  Serial.println();

  FireStatus fireStatus = NO_FIRE;
   if (analogValue < analogThreshold) {
    motor1.setSpeed(0);
    motor1.forward();
    motor2.setSpeed(0);
    motor2.forward();
    digitalWrite(fanPin, LOW);
    delay(500);
  } else if (analogValue >= analogThreshold && digitalRead(digitalFlameDetectorPin) == HIGH) {
    // fireEstinguish(NO_FIRE);
    digitalWrite(fanPin, HIGH);
    robot_control();
  } else if (analogValue >= analogThreshold){
    digitalWrite(fanPin, HIGH);

  }
  
}

int getPosition (){
  uint16_t position;

    sensor[0]=digitalRead(A4);
  sensor[1]=digitalRead(A3);
  sensor[2]=digitalRead(A2);
  sensor[3]=digitalRead(A1);
  sensor[4]=digitalRead(A0);


  if((sensor[0]==1)&&(sensor[1]==1)&&(sensor[2]==1)&&(sensor[3]==1)&&(sensor[4]==0))
    position = 0;
else if((sensor[0]==1)&&(sensor[1]==1)&&(sensor[2]==1)&&(sensor[3]==0)&&(sensor[4]==0))
    position = 500;
else if((sensor[0]==1)&&(sensor[1]==1)&&(sensor[2]==1)&&(sensor[3]==0)&&(sensor[4]==1))
    position = 1000;
else if((sensor[0]==1)&&(sensor[1]==1)&&(sensor[2]==0)&&(sensor[3]==0)&&(sensor[4]==1))
    position = 1500;
else if((sensor[0]==1)&&(sensor[1]==1)&&(sensor[2]==0)&&(sensor[3]==1)&&(sensor[4]==1))
    position = 2000;
else if((sensor[0]==1)&&(sensor[1]==0)&&(sensor[2]==0)&&(sensor[3]==1)&&(sensor[4]==1))
    position = 2500;
else if((sensor[0]==1)&&(sensor[1]==0)&&(sensor[2]==1)&&(sensor[3]==1)&&(sensor[4]==1))
    position = 3000;
else if((sensor[0]==0)&&(sensor[1]==0)&&(sensor[2]==1)&&(sensor[3]==1)&&(sensor[4]==1))
    position = 3500;
else if((sensor[0]==0)&&(sensor[1]==1)&&(sensor[2]==1)&&(sensor[3]==1)&&(sensor[4]==1))
    position = 4000;

  return position;
}

void robot_control() {
  
  uint16_t position = getPosition();

  int error = 2000 - position;

  // if (sensorValues[2] < 100 && sensorValues[3] < 100 && sensorValues[4] < 100 && sensorValues[0] >= 900 && sensorValues[1] >= 900) {
  //   motor2.setSpeed(80);
  //   motor1.setSpeed(-200); // Mundurkan sedikit roda kiri
  //   motor2.forward();
  //   motor1.backward();
  //   delay(150);
  //   position = qtr.readLineBlack(sensorValues);
  // }

  // else if (sensorValues[2] < 100 && sensorValues[0] < 100 && sensorValues[1] < 100 && sensorValues[3] >= 900 && sensorValues[4] >= 900) {
  //   motor1.setSpeed(80);
  //   motor2.setSpeed(-200); // Mundurkan sedikit roda kanan
  //   motor2.backward();
  //   motor1.forward();
  //   delay(150);
  //   position = qtr.readLineBlack(sensorValues);
  // }

  

  while (digitalRead(A4) == 1 && digitalRead(A3) == 1 && digitalRead(A2) == 1 && digitalRead(A1) == 1 && digitalRead(A0) == 1) {
    if (previousError > 0) {
      motor1.setSpeed(100);
      motor1.forward();
      motor2.setSpeed(90);
      motor2.backward();
    } else {
      motor2.setSpeed(100);
      motor2.forward();
      motor1.setSpeed(90);
      motor1.backward();
    }
    position = getPosition();
  }


  PID_Linefollow(error);
}

void fireEstinguish(int fireStatus) {
  switch (fireStatus) {
    case FIRE_CLOSE:
      motor1.stop();
      motor2.stop();
      digitalWrite(fanPin, LOW);
      Serial.println("Kipas Menyala, Api Terdeteksi Dekat");
      break;

    case FIRE_FRONT:
      motor1.setSpeed(80);
      motor2.setSpeed(80);
      motor2.forward();
      motor1.forward();
      digitalWrite(fanPin, LOW);
      Serial.println("Api Terdeteksi di Depan, Bergerak Maju");
      break;

    case FIRE_LEFT:
      motor2.setSpeed(80);
      motor2.forward();
      digitalWrite(fanPin, LOW);
      Serial.println("Api Terdeteksi di Kiri, Bergerak ke Kanan");
      break;

    case FIRE_RIGHT:
      motor1.setSpeed(80);
      motor1.forward();
      digitalWrite(fanPin, LOW);
      Serial.println("Api Terdeteksi di Kanan, Bergerak ke Kiri");
      break;

    case NO_FIRE:
      digitalWrite(fanPin, HIGH);
      Serial.println("Tidak Ada Api, Diam");
      break;
    default:
      digitalWrite(fanPin, HIGH);
      Serial.println("Tidak Ada Api, Diam");
      break;
  }

  delay(100);
}

void PID_Linefollow(int error) {
  P = error;
  I = I + error;
  D = error - previousError;

  float Pvalue = (Kp * P);
  float Ivalue = (Ki * I);
  float Dvalue = (Kd * D);

  PIDvalue = Pvalue + Ivalue + Dvalue;
  previousError = error;

  Serial.print("Error: ");
  Serial.println(error);

  Serial.print("Postition: ");
  Serial.println(2000 + error);

  lsp = lfspeed - PIDvalue;
  rsp = rspeed + PIDvalue;

  if (error == 0) {
    lsp = lfspeed;
    rsp = rspeed;
  }

  if (lsp > FORWARD_MAX){
   lsp = FORWARD_MAX;
  }
  if (lsp < BACKWARD_MAX){
     lsp = BACKWARD_MAX;
     }
  if (rsp > FORWARD_MAX){
     rsp = FORWARD_MAX;
     }
  if (rsp < BACKWARD_MAX) {
    rsp = BACKWARD_MAX;
    }

  motor_drive(lsp, rsp);
}

void motor_drive(int left, int right) {
  Serial.print("left: ");
  Serial.println(left);
  Serial.print("right: ");
  Serial.println(right);

  if (left > 0) {
    motor2.setSpeed(left);
    motor2.forward();
  } else {
    motor2.setSpeed(-left);
    motor2.backward();
  }

  if (right > 0) {
    motor1.setSpeed(right);
    motor1.forward();
  } else {
    motor1.setSpeed(-right);
    motor1.backward();
  }

}
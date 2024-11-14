#include <Servo.h>

#define sensorAmo 14

#define sensorLedEven 9
#define sensorLedOdd 8
#define buttonPin 3
#define impPin 6

#define STBY 39
#define MAP 44
#define MBP 45
#define MA1 40
#define MA2 42
#define MB1 41
#define MB2 43

int white[sensorAmo];
int black[sensorAmo];
int gray[sensorAmo];
int sensorPin[sensorAmo] = { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13 };
int sensorKoefs[sensorAmo] = { 7, 6, 5, 4, 3, 2, 1, -1, -2, -3, -4, -5, -6, -7 };
Servo imp;
void setup() {

  Serial.begin(115200);
  imp.attach(impPin);
  imp.writeMicroseconds(2300);
  delay(2000);
  imp.writeMicroseconds(800);
  for (int i = 0; i < sensorAmo; ++i) {
    white[i] = 0;
    black[i] = 1023;
  }
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(MAP, OUTPUT);
  pinMode(MBP, OUTPUT);
  pinMode(MB1, OUTPUT);
  pinMode(MB2, OUTPUT);
  pinMode(MB1, OUTPUT);
  pinMode(MB2, OUTPUT);
  unsigned int k = millis();
  while ((millis() - k) < 3000) {
    for (int i = 0; i < sensorAmo; ++i) {
      white[i] = max(white[i], analogRead(sensorPin[i]));
      black[i] = min(black[i], analogRead(sensorPin[i]));
    }
  }
  for (int i = 0; i < sensorAmo; ++i) {
    gray[i] = (white[i] + black[i]) / 2;
  }
  imp.writeMicroseconds(1000);

  digitalWrite(STBY, HIGH);
  Serial.begin(9600);
}

bool outLine = false;
bool data[sensorAmo] = { false, false, false, false, false, false, false, false, false, false, false, false, false, false };

int readSensor() {
  int error = 0;
  int sensors[sensorAmo];
  for (int i = 0; i < sensorAmo; ++i) {
    sensors[i] = (analogRead(sensorPin[i]) - gray[i]);
  }
  int count = 0;
  for (int i = 0; i < sensorAmo; ++i) {
    if (sensors[i] > 0) {
      ++count;
      data[i] = true;
    } else {
      data[i] = false;
    }
    error += sensors[i] * sensorKoefs[i];

    /*Serial.print(sensors[i]);
        Serial.print(" ");*/
  }

  outLine = false;
  if (count == 0) {
    outLine = true;
    return 0;
  }
  error /= count;
  return error;
}

int sp = 255;


void go(int ml, int mr) {
  if (fabs(ml) < 10) {
    digitalWrite(MA1, HIGH);
    digitalWrite(MA2, HIGH);
  } else if (ml < 0) {
    digitalWrite(MA1, LOW);
    digitalWrite(MA2, HIGH);
  } else {
    digitalWrite(MA1, HIGH);
    digitalWrite(MA2, LOW);
  }
  analogWrite(MAP, constrain(abs(ml), 0, 255));
  if (fabs(mr) < 10) {
    digitalWrite(MB1, HIGH);
    digitalWrite(MB2, HIGH);
  } else if (mr < 0) {
    digitalWrite(MB1, LOW);
    digitalWrite(MB2, HIGH);
  } else {
    digitalWrite(MB1, HIGH);
    digitalWrite(MB2, LOW);
  }

  analogWrite(MBP, constrain(abs(mr), 0, 255));
}

double pe = 0;

double io = 0;

double kp = 0.1;
double kd = 0;
double ki = 0;


unsigned int timed = millis();
unsigned int timek = millis();


void loop() {

  Serial.println(sp);
  imp.writeMicroseconds(2300);
  double error = readSensor();


  if (outLine) {
    if (pe > 0) {
      go(-100, 100);
      while (!data[6] && !data[7]) error = readSensor();
    } else {
      go(100, -100);
      while (!data[6] && !data[7]) error = readSensor();
    }
    timek = millis();
  }
  if (millis() - timek > 250) {
    kp = 0.1;
    kd = 0;
    ki = 0.0002;
  } else {
    kp = 0.12;
    kd = 0;
    ki = 0;
  }
  io = io * 0.9 + error;
  double p = error * kp;
  double d = (error - pe) / (millis() - timed) * kd;
  timed = millis();
  double i = io * ki;
  pe = error;
  int r = p + d + i;
  go((sp - r), (sp + r));
}

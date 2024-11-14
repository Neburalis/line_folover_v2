#include <MCP3008.h>
#include "SoftServo.h"
#include "Servo.h"

#pragma region pinDefine
// Motor Driver
#define STBY 39
#define MAP 44
#define MBP 45
#define MA1 40
#define MA2 42
#define MB1 41
#define MB2 43

// MCP3008
#define CS_PIN 2
#define CLOCK_PIN 8
#define MOSI_PIN 4
#define MISO_PIN 6

// Other
#define btnPin 20
#define impPin 10
#define ledPin 13
#pragma endregion

#define debug true

#pragma region sensors
// back sensor bs (analogRead)
#define bSensorAmo 8
int bwhite[bSensorAmo] = { 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000 };
int bblack[bSensorAmo];
int bgrey[bSensorAmo];
int bsensorPin[bSensorAmo] = { A0, A2, A4, A6, A8, A10, A12, A14 };
const int8_t bsensorkoefs[bSensorAmo] = { -4, -3, -2, -1, 1, 2, 3, 4 };
int bdata[bSensorAmo];

//// front sensor (MCP3008))
#define fSensorAmo 8

int fwhite[fSensorAmo] = { 10000, 10000, 10000, 10000, 10000, 10000, 10000, 10000 };
int fblack[fSensorAmo];
int fgrey[fSensorAmo];
const int8_t fsensorkoefs[fSensorAmo] = { -4, -3, -2, -1, 1, 2, 3, 4 };
int fdata[fSensorAmo];
#pragma endregion

// MCP3008 object
MCP3008 fSensor(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);

// impeller
Servo imp;

#pragma region setup
void setup() {
#if debug
  Serial.begin(115200);
#endif
#pragma region pinMode_attach

  pinMode(btnPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  pinMode(MAP, OUTPUT);
  pinMode(MBP, OUTPUT);
  pinMode(MB1, OUTPUT);
  pinMode(MB2, OUTPUT);
  pinMode(MA1, OUTPUT);
  pinMode(MA2, OUTPUT);
  pinMode(STBY, OUTPUT);

  for (int i = 0; i < bSensorAmo; ++i) {
    pinMode(bsensorPin[i], INPUT);
  }

  //  imp.attach(impPin);
  //  imp.delayMode();
#pragma endregion

// Ждем нажатия кнопки перед началом калибровки сенсора (Частые мигания светодиодом)
#pragma region whaitcalibrate
  uint16_t tmr = millis();
  uint16_t btnTmr = millis();
  bool ledFlag = false;
  bool btnFlag = false;

#if debug
  Serial.println(F("Whait calibration command"));
#endif

  while (true) {
    bool btnState = !digitalRead(btnPin);

    // Обработка кнопки с антидребезгом для выхода из цикла
    if (btnState && !btnFlag && millis() - btnTmr > 100) {
      btnFlag = true;
      break;
    }
    if (!btnState && btnFlag && millis() - btnTmr > 100) {
      btnFlag = false;
      btnTmr = millis();
    }

    if (millis() - tmr >= 200) {
      tmr = millis();
      digitalWrite(13, ledFlag);
      ledFlag = !ledFlag;
    }
  }
  digitalWrite(ledPin, 0);
#pragma endregion

// calibrate sensors
#pragma region calibrate sensors
  tmr = millis();
  while (millis() - tmr < 3000) {
    for (int i = 0; i < fSensorAmo; ++i) {
      int val = fSensor.readADC(i);
      if (val != 0) {
        fwhite[i] = min(fwhite[i], val);
        fblack[i] = max(fblack[i], val);
      }
    }
    for (int i = 0; i < bSensorAmo; ++i) {
      int val = analogRead(bsensorPin[i]);
      if (val != 0) {
        bwhite[i] = min(bwhite[i], val);
        bblack[i] = max(bblack[i], val);
      }
    }
  }
  for (int i = 0; i < fSensorAmo; ++i) {
    fgrey[i] = (fwhite[i] + fblack[i]) / 2;
  }
  for (int i = 0; i < bSensorAmo; ++i) {
    bgrey[i] = (bwhite[i] + bblack[i]) / 2;
  }
#pragma endregion

// Print to serial senso calibration information
#if debug
  Serial.println(F("Front sensor params:"));
  for (int i = 0; i < fSensorAmo; ++i) {
    Serial.print(fwhite[i]);
    Serial.print("\t");
  }
  Serial.println();
  for (int i = 0; i < bSensorAmo; ++i) {
    Serial.print(fblack[i]);
    Serial.print("\t");
  }
  Serial.println();
  for (int i = 0; i < fSensorAmo; ++i) {
    Serial.print(fgrey[i]);
    Serial.print("\t");
  }
  Serial.println();

  Serial.println(F("Back sensor params:"));
  for (int i = 0; i < bSensorAmo; ++i) {
    Serial.print(bwhite[i]);
    Serial.print("\t");
  }
  Serial.println();
  for (int i = 0; i < bSensorAmo; ++i) {
    Serial.print(bblack[i]);
    Serial.print("\t");
  }
  Serial.println();
  for (int i = 0; i < bSensorAmo; ++i) {
    Serial.print(bgrey[i]);
    Serial.print("\t");
  }
  Serial.println();
#endif

// calibrate max and min of imp
#if debug
  Serial.println(F("Calibrate impeller"));
#endif
  // imp.asyncMode();
  imp.writeMicroseconds(2300);
  delay(3000);
  imp.writeMicroseconds(800);
  delay(3000);

// Ждем команды "На старт" (готовы ехать) (редкие мигания светодиодом)
#pragma region whaitstart
  tmr = millis();
  btnTmr = millis();
  btnFlag = false;

#if debug
  Serial.println(F("Whait start command"));
#endif

  while (true) {
    imp.tick();
    bool btnState = !digitalRead(btnPin);

    // Обработка кнопки с антидребезгом для выхода из цикла
    if (btnState && !btnFlag && millis() - btnTmr > 100) {
      btnFlag = true;
      break;
    }
    if (!btnState && btnFlag && millis() - btnTmr > 100) {
      btnFlag = false;
      btnTmr = millis();
    }

    if (millis() - tmr >= 800) {
      tmr = millis();
      digitalWrite(13, ledFlag);
      ledFlag = !ledFlag;
    }
  }
  digitalWrite(ledPin, 0);
#pragma endregion

  imp.tick();
  // on impeller
  //  imp.writeMicroseconds(1000);
  imp.tick();

  // enable motors
  digitalWrite(STBY, 1);
}
#pragma endregion

void go(int ml, int mr) {
  //#if debug
  //  Serial.println(ml);
  //  Serial.println(mr);
  //#endif

  // Left motor
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
  Serial.println(constrain(abs(ml), 0, 255));
  analogWrite(MAP, constrain(abs(ml), 0, 255));

  // Right motor
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

// Read front sensor
void fRead() {
  for (int i = 0; i < fSensorAmo; ++i) {
    fdata[i] = fSensor.readADC(i) - fgrey[i];
  }
}

// Read back sensor
void bRead() {
  imp.tick();
  for (int i = 0; i < bSensorAmo; ++i) {
    bdata[i] = analogRead(bsensorPin[i]) - bgrey[i];
  }
}

bool foutline = false;
int calcError() {
  imp.tick();
  double error = 0;
  fRead();
  int count = 0;
  for (int i = 0; i < fSensorAmo; ++i) {
    if (fdata[i] > 0) {
      ++count;
    }

    error += fdata[i] * fsensorkoefs[i];
  }

  foutline = false;
  if (count == 0) {
    foutline = true;
    return 0;
  }
  error /= count;
  return error;
}

void yield() {
  imp.tick();
}

double prevErr = 0;
double integralComp = 0;

double kP = 0.1;
double kD = 0;
double kI = 0;

uint32_t tmr_d = millis();
uint32_t tmr_k = millis();


#define defaultSpeed 150
#define backToLineSpeed 50

uint8_t sp = defaultSpeed;

void loop() {
  double error = calcError();

  if (foutline) {
    if (prevErr > 0) {
      go(-backToLineSpeed, backToLineSpeed);
      while (fdata[3] < 0 && fdata[4] < 0) error = calcError();
    } else {
      go(backToLineSpeed, -backToLineSpeed);
    }
    tmr_k = millis();
  }

  if (millis() - tmr_k > 250) {
    kP = 0.1;
    kD = 0;
    kI = 0.0002;
  } else {
    kP = 0.12;
    kD = 0;
    kI = 0;
  }

  integralComp = integralComp * 0.9 + error;

  int reg = (error)*kP + ((error - prevErr) / (millis() - tmr_d)) * kD + integralComp * kI;

  prevErr = error;
  tmr_d = millis();

  go((sp + reg), (sp - reg));
}

void setup() {
#ifdef debug
  Serial.begin(115200);
#endif
  // other
  pinMode(btnPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  // motors driver
  pinMode(MAP, OUTPUT);
  pinMode(MBP, OUTPUT);
  pinMode(MB1, OUTPUT);
  pinMode(MB2, OUTPUT);
  pinMode(MA1, OUTPUT);
  pinMode(MA2, OUTPUT);
  pinMode(STBY, OUTPUT);

  // back sensor
#ifdef bs
  for (int i = 0; i < bSensorAmo; ++i) {
    pinMode(bsensorPin[i], INPUT);
  }
#endif

  // impeller
  imp.attach(impPin);

#if (mode == 0)
// Ждем нажатия кнопки перед началом калибровки сенсора (Частые мигания светодиодом)
#pragma region whaitcalibrate
  uint16_t tmr = millis();
  uint16_t btnTmr = millis();
  bool ledFlag = false;
  bool btnFlag = false;

#ifdef debug
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

#ifdef debug
  Serial.println(F("Calibration..."));
#endif

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
#ifdef bs
    for (int i = 0; i < bSensorAmo; ++i) {
      int val = analogRead(bsensorPin[i]);
      if (val != 0) {
        bwhite[i] = min(bwhite[i], val);
        bblack[i] = max(bblack[i], val);
      }
    }
#endif
  }
  for (int i = 0; i < fSensorAmo; ++i) {
    fgrey[i] = (fwhite[i] + fblack[i]) / 2;
  }
#ifdef bs
  for (int i = 0; i < bSensorAmo; ++i) {
    bgrey[i] = (bwhite[i] + bblack[i]) / 2;
  }
#endif
#pragma endregion

// Print to serial senso calibration information
#ifdef debug
  Serial.println(F("Front sensor params (white, black, grey):"));
  for (int i = 0; i < fSensorAmo; ++i) {
    Serial.print(fwhite[i]);
    Serial.print("\t");
  }
  Serial.println();
  for (int i = 0; i < fSensorAmo; ++i) {
    Serial.print(fblack[i]);
    Serial.print("\t");
  }
  Serial.println();
  for (int i = 0; i < fSensorAmo; ++i) {
    Serial.print(fgrey[i]);
    Serial.print("\t");
  }
  Serial.println();

#ifdef bs
  Serial.println(F("Back sensor params (white, black, grey):"));
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
#endif

// calibrate max and min of imp
#ifdef enable_imp
#ifdef debug
  Serial.println(F("Calibrate impeller..."));
#endif

  imp.writeMicroseconds(2300);
  delay(3000);
  imp.writeMicroseconds(800);
  delay(3000);
#endif

// Ждем команды "На старт" (готовы ехать) (редкие мигания светодиодом)
#pragma region whaitstart
  tmr = millis();
  btnTmr = millis();
  btnFlag = false;

#ifdef debug
  Serial.println(F("Whait start command..."));
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

    if (millis() - tmr >= 800) {
      tmr = millis();
      digitalWrite(13, ledFlag);
      ledFlag = !ledFlag;
    }
  }
  digitalWrite(ledPin, 0);
#pragma endregion

#ifdef enable_imp
  // on impeller
  imp.writeMicroseconds(1000);
#endif

#endif

// enable motors
#ifdef enable_motors
  digitalWrite(STBY, 1);
#endif
}
#pragma endregion

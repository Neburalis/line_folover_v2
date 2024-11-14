void go(int ml, int mr) {
// #ifdef debug
//   Serial.println(ml);
//   Serial.println(mr);
//   Serial.println(constrain(abs(ml), 0, 255));
// #endif

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

#ifdef bs
// Read back sensor
void bRead() {
  for (int i = 0; i < bSensorAmo; ++i) {
    bdata[i] = analogRead(bsensorPin[i]) - bgrey[i];
  }
}
#endif

int calcError() {
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
#if (mode == 0)
void loop() {
  // Таймеры: дифферинциальной состовляющей, прямой
  static uint32_t tmr_d = millis(), tmr_k = millis();
  // Ошибка на предыдущей итерации, интегральная состовляющая
  static double prevErr = 0, integralComp = 0;
  // Коэффициенты ПИД
  static double kP, kD, kI;
  // Скорость
  static uint8_t sp = defaultSpeed;

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
#endif
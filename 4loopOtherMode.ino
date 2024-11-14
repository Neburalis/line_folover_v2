#if (mode == 1)
void loop() {

  fRead();

  for (int i = 0; i < fSensorAmo; ++i){
    Serial.print(fdata[i]);
    Serial.print("\t");
  }
  Serial.println();

}
#endif
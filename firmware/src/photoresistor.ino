int getPhotoresistorValue() {
  return analogRead(photoresistorPin);
}

int getPhotoresistorProcent() {
  int outputMin = 1;
  int outputMax = 100;

  int result = map(getPhotoresistorValue(), lightMin, lightMax, outputMin, outputMax);

  result = round(result / 10) * 10;

  if (result > outputMax) result = outputMax;
  else if (result < outputMin) result = outputMin;

  #ifdef DEBUG
    Serial.println("Розрахований відсоток " + String(result));
  #endif

  return result;
}
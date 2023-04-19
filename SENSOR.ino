void readPower() {
   vac = pzem_r.voltage();
   iac = pzem_r.current();
   freq = pzem_r.frequency();
   pf = pzem_r.pf();
   powerAC = pzem_r.power();
   energyAC = pzem_r.energy();
}

void readMode() {
  buttonValue = digitalRead(buttonPin);
  if (buttonValue == 0) {
    modeState = 1;
    digitalWrite(led, HIGH);
  }
  else if (buttonValue == 1) {
    modeState = 0;
    digitalWrite(led, LOW);
  }
}

void readPot() {
  rawValue = analogRead(potensioPin);
  percentage = map(rawValue, 1050, 4095, 0, 100);
}

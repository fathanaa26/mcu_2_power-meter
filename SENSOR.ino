void readPower() {
  //  vac = pzem_r.voltage();
  //  iac = pzem_r.current();
  //  freq = pzem_r.frequency();
  //  pf = pzem_r.pf();
  //  powerAC = pzem_r.power();
  //  energyAC = pzem_r.energy();

   vac = random(0,10) / 100.00;
   iac = random(10,20) / 100.00;
   freq = random(20,30) / 100.00;
   pf = random(30,40) / 100.00;
   powerAC = random(50,60) / 100.00;
   energyAC = random(60,80) / 100.00;
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

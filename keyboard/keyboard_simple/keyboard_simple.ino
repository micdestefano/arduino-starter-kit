void setup() {
  // put your setup code here, to run once:
  pinMode(8, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int a0_input = analogRead(A0);

  int NONE = 0;
  int DO = 262;
  int RE = 294;
  int MI = 330;
  int FA = 349;

  Serial.print("Valore letto da A0: ");
  Serial.println(a0_input);
  //delay(500);

  if (a0_input < 100) {
    noTone(8);
  } else if (a0_input > 100 && a0_input < 500) {
    tone(8, DO);
  } else if (a0_input > 500 && a0_input < 700) {
    tone(8, RE);
  } else if (a0_input > 700 && a0_input < 1000) { 
    tone(8, MI);
  } else {
    tone(8, FA);
  } 
}

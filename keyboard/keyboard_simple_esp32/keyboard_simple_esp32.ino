/*
 * This is the version running on an ESP32 board (Nde MCU ESP 32S).
 * In this case a smaller buzzer must be used and this buzzer needs to be
 * controlled through an NPN transistor (I used S8050, suggested by Gemini)
 * because we need to provide it with more power (and 3.3V are not sufficient).
 * 
 * We must use the 5V source by the ESP 32 for powering the
 * transistor + buzzer circuit but we must ensure this is decoupled as much
 * as possible from the board.
 *
 * The input part of the circuit gets powerd by 3.3V and is identical to that
 * presented into the starter kit manual.
 *
 * How to connect the transistor:
 *
 * By looking at the flat transistor face, with pin pointing downwards, we have,
 * from left to right:
 *  - emitter: connect to ground directly
 *  - base: connect it to pin 18 (that brings the output with the square wave)
 *          through a 2.2k resistor. The resistor protects the board by not
 *          allowing too much current to flow from pin 18.
 *  - collector: connect it to the negative pin of the buzzer (actually the
 *          buzzer has a polarity and you need to connect it properly!).
 *
 * Then, connect the other pin of the buzzer to the 5V source.
 */
const int buzzerPin = 18; // GPIO 18 (Pin P18 on my board)
const int analogPin = 34; // GPIO 34 (Pin P34 on my board)

void setup() {
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(115200); // Standard speed for ESP32 (remember to configure it correctly into the serial monitor)
  
  // This sets up analog read with the same number of bits used by Arduino UNO R3 (by default this uses 12 bits)
  analogReadResolution(10);

  // We could use ledcAttach and ledcWriteTone (more precise), but the final effect is the same (verified experimentally)
}

int lastTone = 0;

void loop() {
  int a0_input = analogRead(analogPin);
  int currentTone = 0;

  int DO = 262;
  int RE = 294;
  int MI = 330;
  int FA = 349;

  Serial.print("Valore letto da P34: ");
  Serial.println(a0_input);

  if (a0_input < 100) {
    currentTone = 0;
  } else if (a0_input >= 100 && a0_input < 300) {
    currentTone = DO;
  } else if (a0_input >= 300 && a0_input < 600) {
    currentTone = RE;
  } else if (a0_input >= 700 && a0_input < 1000) { 
    currentTone = MI;
  } else {
    currentTone = FA;
  }

  if (currentTone != lastTone) {
    if (currentTone == 0) {
      noTone(buzzerPin);
    } else {
      tone(buzzerPin, currentTone);
    }
    lastTone = currentTone;
  }
}
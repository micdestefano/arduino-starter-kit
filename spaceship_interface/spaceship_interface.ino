/**
 * This is project n. 3 into the Arduino starter kit manual.
 *
 * Author: Michele De Stefano
 */

static uint8_t const switch_pin = 2;
static uint8_t const green_pin = 3;
static uint8_t const red_pin = 4;
static uint8_t const yellow_pin = 11;

void switch_on(uint8_t pin) {
  digitalWrite(pin, HIGH);
}

void switch_off(uint8_t pin) {
  digitalWrite(pin, LOW);
}

void setup() {
  // put your setup code here, to run once:
  pinMode(switch_pin, INPUT);
  pinMode(green_pin, OUTPUT);
  pinMode(red_pin, OUTPUT);
  pinMode(yellow_pin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  int const switch_state = digitalRead(switch_pin);
  if (switch_state == LOW) {
    switch_on(green_pin);
    switch_off(red_pin);
    switch_off(yellow_pin);
  } else {
    switch_off(green_pin);
    switch_on(yellow_pin);
    switch_off(red_pin);
    delay(500);
    switch_off(yellow_pin);
    switch_on(red_pin);
    delay(500);
    switch_on(red_pin);
    switch_on(yellow_pin);
    delay(500);
    switch_off(red_pin);
    switch_off(yellow_pin);
    delay(500);
  }
}

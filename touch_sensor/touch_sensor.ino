/**
 * Implementation of project n. 13 into the starter kit.
 * The equivalent circuit of the experiment is the following:
 *
 *     send-pin ---------- R ----------- receive-pin
 *                 |              |
 *                Vin             C
 *                 |              |
 *                GND            GND
 *
 * The capacitor is formed, on one end, by the 
 * metal foil. On the other end there is our hand
 * (or finger, or any object that gets close to the
 * foil). The CapacitiveSensor library changes the send
 * pin from low to high and measures the charge time,
 * i.e. it measures the time the receive-pin takes to
 * reach the same voltage as the send pin. It simply
 * increments one variable at each loop and returns it.
 * So it returns an integer that is proportional to the
 * charge time, but the unit of that integer is
 * completely arbitrary.
 *
 * Video of my implementation: https://www.youtube.com/watch?v=-WlKmOLo8Bo
 *
 * Author: Michele De Stefano
 */
#include <CapacitiveSensor.h>

static uint8_t const  SEND_PIN = 3;
static uint8_t const  RECV_PIN = 4;
static uint8_t const  LED_PIN = 10;
static uint8_t const  NUM_SAMPLES = 50;
// Change this threshold according to your needs
static long const     THRESHOLD = 50;

static CapacitiveSensor sensor(SEND_PIN, RECV_PIN);

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  long const value = sensor.capacitiveSensor(NUM_SAMPLES);
  Serial.print("value:");
  Serial.println(value);
  uint8_t const led_value = (value > THRESHOLD) ? HIGH : LOW;
  digitalWrite(LED_PIN, led_value);
}

/**
 * This is project n. 6 into the Arduino starter kit manual.
 *
 * Author: Michele De Stefano
 */

static long const CALIBRATION_DELAY_MILLIS = 5000;
static uint8_t const SENSOR = A0;
static uint8_t const PIEZO = 8;
static unsigned int const MIN_FREQ_HZ = 50;
static unsigned int const MAX_FREQ_HZ = 1000;
static unsigned long const TONE_DURATION_MS = 20;


class Theremin {
public:

  void init() {
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(PIEZO, OUTPUT);
    // NOTE: Use the builtin led (connected to pin 13) to signal start/end of calibration
    // Move your hand over the fototransistor while claibration is ongoing
    digitalWrite(LED_BUILTIN, HIGH);
    while (millis() < CALIBRATION_DELAY_MILLIS) {
      int sensor_val = analogRead(SENSOR);
      m_sensor_min = min(m_sensor_min, sensor_val);
      m_sensor_max = max(m_sensor_max, sensor_val);
    }
    digitalWrite(LED_BUILTIN, LOW);
  }

  void play() {
    int const sensor_val = analogRead(SENSOR);
    unsigned int const freq = static_cast<unsigned int>(map(sensor_val, m_sensor_min, m_sensor_max, MIN_FREQ_HZ, MAX_FREQ_HZ));
    tone(PIEZO, freq, TONE_DURATION_MS);
  }

private:

  int m_sensor_min = 1023;
  int m_sensor_max = 0;
};

static Theremin theremin;

void setup() {
  // put your setup code here, to run once:
  theremin.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  theremin.play();
}

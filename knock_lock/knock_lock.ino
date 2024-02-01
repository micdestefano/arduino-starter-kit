/**
 * This is a variation on project n. 12 into the starter kit manual.
 *
 * I am not using a motor. I simply used a red and a green led.
 * The green led is switched on only if the correct sequence, with
 * the correct timing, is knocked. This is a lot more complicated
 * than simply checking the total number of knocks in a certain
 * period. The code is quite complex, because it must check also
 * that the knocs are released with the correct rhythm. The expected
 * duration of all the knocks is discovered after the second knock
 * is released. So you can release the knocks slowly or quickly,
 * but you have to knock with the right rhythm!!! Don't release
 * knocks too quickly, because the piezo could miss some of them.
 *
 * I am using again the observer design pattern.
 *
 * NOTE: I notiched that the piezo is not so sensitive to light knocks.
 * I had to knock on the breadboard or on the piezo itself. It is better
 * not to knock on the piezo, for not risking to damage it.
 *
 * Author: Michele De Stefano
 */

static uint8_t const  PIEZO_INPUT_PIN = A0;
static uint8_t const  RED_LED_PIN = 5;
static uint8_t const  GREEN_LED_PIN = 8;
static uint8_t const  PUSHBUTTON_PIN = 11;
static unsigned long const BUTTON_DELAY_MILLIS = 250;

class LedsController {
public:

  void init() {
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    reset();
  }

  void reset() {
    m_red_state = HIGH;
    switch_leds();
  }

  void switch_state() {
    m_red_state = !m_red_state;
    switch_leds();
  }

  void step() {}

private:

  void switch_leds() {
    digitalWrite(RED_LED_PIN, m_red_state);
    digitalWrite(GREEN_LED_PIN, !m_red_state);
  }

private:

  uint8_t m_red_state = LOW;
};


class SequenceDetector {

  typedef enum {
    HALF = 0,     // Half pause
    FULL = 1,     // Full pause
    DOUBLE = 2,   // Double pause
    INF = 3
  } PauseType;

public:

  SequenceDetector(LedsController* const leds_controller) : m_leds_controller(leds_controller) {
    m_pause_len[INF] = INT32_MAX;
  }

  void init() {
    reset();
  }

  void reset() {
    m_t0 = 0;
    m_dt_min = 0;
    m_dt_max = INT32_MAX;
    m_knock_index = 0;
    m_leds_controller->reset();
    Serial.println("Ready to receive the sequence");
  }

  void step() {
    int knock_volume = analogRead(PIEZO_INPUT_PIN);
    long t = millis();
    long delta_t = t - m_t0;
    if (delta_t > 200 && m_knock_index < k_sequence_length && knock_volume > k_knock_volume_threshold) {
      // A sufficiently loud knock has been detected
      ++m_knock_index;
      Serial.print("Knock volume: ");
      Serial.println(knock_volume);
      
      Serial.print("Knock n. ");
      Serial.println(m_knock_index);
      Serial.print("delta_t: ");
      Serial.println(delta_t);
      Serial.print("m_dt_min: "); Serial.println(m_dt_min);
      Serial.print("m_dt_max: "); Serial.println(m_dt_max);
      if (delta_t < m_dt_min || delta_t > m_dt_max) {
        // The detected knock was out of time
        Serial.println("Last knock was out of time. Resetting.");
        reset();
        return;
      }
      // The detected knock was at the right time
      if (m_knock_index == 2) init_pause_lengths(delta_t);

      if (m_knock_index >= 2) {
        // Prepare parameters for detecting if the next knock is at the right time
        PauseType expected_pause = k_expected_pauses[m_knock_index];
        long expected_pause_len = m_pause_len[expected_pause];
        m_dt_min = expected_pause_len - m_pause_tol;
        m_dt_max = expected_pause_len + m_pause_tol;
      }
      m_t0 = t;
      if (m_knock_index == k_sequence_length) {
        // The last detected valid knock in the sequence unlocks the leds
        notify_led_controller();
        Serial.print("Unlocked!!!");
      }
    }
  }

private:

  void init_pause_lengths(long delta_t) {
    // This inits the expected durations on the basis of the delay between the first two knocks
    long const full_millis = delta_to_full(delta_t, k_expected_pauses[1]);
    m_pause_len[FULL]   = full_millis;
    m_pause_len[HALF]   = full_millis >> 1;
    m_pause_len[DOUBLE] = full_millis << 1;
    m_pause_tol = round(full_millis * 0.25);
    Serial.print("HALF (ms): "); Serial.println(m_pause_len[HALF]);
    Serial.print("FULL (ms): "); Serial.println(m_pause_len[FULL]);
    Serial.print("DOUBLE (ms): "); Serial.println(m_pause_len[DOUBLE]);
    Serial.print("TOL +/- (ms): "); Serial.println(m_pause_tol);
  }

  long delta_to_full(long delta_t, PauseType pause) {
    if (pause == FULL) return delta_t;
    if (pause == HALF) return delta_t >> 1;
    return delta_t << 1;
  }

  void notify_led_controller() {
    m_leds_controller->switch_state();
  }

private:

  static PauseType const  k_expected_pauses[];

  static uint8_t const    k_sequence_length;

  static int const        k_knock_volume_threshold = 150;

  LedsController* const   m_leds_controller;

  long m_pause_len[4];

  long m_pause_tol = 0;

  long m_t0;

  long m_dt_min;

  long m_dt_max;

  uint8_t m_knock_index;
};

/*
 * Here below you define your "secret knock password". The notation is analogous to the one used in music for pauses.
 * You have to imagine to knock BETWEEN the pauses, i.e. where you see the commas AND after the last pause.
 */
SequenceDetector::PauseType const SequenceDetector::k_expected_pauses[] = {INF, FULL, DOUBLE, HALF};

uint8_t const   SequenceDetector::k_sequence_length = sizeof(SequenceDetector::k_expected_pauses) / sizeof(SequenceDetector::k_expected_pauses[0]);


class PushbuttonObserver {
public:

  PushbuttonObserver(SequenceDetector* const sequence_detector) : m_sequence_detector(sequence_detector) {}

  void init() {
    pinMode(PUSHBUTTON_PIN, INPUT);
  }

  void step() {
    if (!m_was_pushed && digitalRead(PUSHBUTTON_PIN) == HIGH) {
      m_was_pushed = true;
      m_push_t = millis();
    } else if (m_was_pushed && digitalRead(PUSHBUTTON_PIN) == LOW && millis() - m_push_t >= BUTTON_DELAY_MILLIS) {
      Serial.println("Push button pressed: resetting.");
      m_sequence_detector->reset();
      m_was_pushed = false;
      m_push_t = 0;
    }
  }

private:

  SequenceDetector* const m_sequence_detector;

  bool m_was_pushed = false;

  unsigned long m_push_t = 0;
};


static LedsController     leds_controller;
static SequenceDetector   sequence_detector(&leds_controller);
static PushbuttonObserver button_observer(&sequence_detector);

void setup() {
  Serial.begin(9600);
  leds_controller.init();
  sequence_detector.init();
  button_observer.init();
}

void loop() {
  leds_controller.step();
  sequence_detector.step();
  button_observer.step();
}

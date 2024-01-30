/**
 * This is a variation on project n. 12 into the starter kit manual.
 *
 * I am not using a motor. I simply used a red and a green led.
 * The green led is switched on only if the correct sequence, with
 * the correct timing, is knocked. This is slightly more complicated
 * than simply checking the total number of knocks in a certain
 * period.
 *
 * Author: Michele De Stefano
 */

static uint8_t const  PIEZO_INPUT_PIN = A0;
static uint8_t const  RED_LED_PIN = 5;
static uint8_t const  GREEN_LED_PIN = 8;
static uint8_t const  PUSHBUTTON_PIN = 3;
static unsigned long const BUTTON_DELAY_MILLIS = 500;

class LedsController {
public:

  void init() {
    pinMode(RED_LED_PIN, OUTPUT);
    pinMode(GREEN_LED_PIN, OUTPUT);
    reset();
  }

  void reset() {
    m_red_state = LOW;
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
    m_pause_len[INF] = 0xffff;
  }

  void init() {
    reset();
  }

  void reset() {
    m_t0 = 0;
    m_dt_min = 0;
    m_dt_max = 0xffff;
    m_knock_index = 0;
    m_leds_controller->reset();
  }

  void step() {
    if (m_knock_index < k_sequence_length && analogRead(PIEZO_INPUT_PIN) > k_min_knock_volume) {
      // A sufficiently loud knock has been detected
      if (++m_knock_index == k_sequence_length) {
        // The last detected valid knock in the sequence unlocks the leds
        notify_led_controller();
        return;
      }
      
      long t = millis();
      long delta_t = t - m_t0;
      if (delta_t < m_dt_min || delta_t > m_dt_max) {
        // The detected knock was out of time
        reset();
        return;
      }
      // The detected knock was at the right time
      if (m_knock_index == 2) init_pause_lengths(delta_t);
      m_t0 = t;
      // Prepare parameters for detecting if the next knock is at the right time
      PauseType expected_pause = k_expected_sequence[m_knock_index];
      long expected_pause_len = m_pause_len[expected_pause];
      m_dt_min = expected_pause_len - m_pause_tol;
      m_dt_max = expected_pause_len + m_pause_tol;
    }
  }

private:

  void init_pause_lengths(long delta_t) {
    // This inits the expected durations on the basis of the delay between the first two knocks
    long const full_millis = delta_to_full(delta_t, k_expected_sequence[0]);
    m_pause_len[FULL]   = full_millis;
    m_pause_len[HALF]   = full_millis >> 1;
    m_pause_len[DOUBLE] = full_millis << 1;
    m_pause_tol = round(full_millis * 0.1);
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

  static long const       k_min_knock_volume = 100;

  static PauseType const  k_expected_sequence[];

  static uint8_t const    k_sequence_length;

  LedsController* const   m_leds_controller;

  long m_pause_len[4];

  long m_pause_tol = 0;

  long m_t0;

  long m_dt_min;

  long m_dt_max;

  uint8_t m_knock_index;
};

// NOTE: Here below you define your "secret knock password". The notation is analogous to the one used in music
SequenceDetector::PauseType const SequenceDetector::k_expected_sequence[] = {INF, FULL, HALF, HALF, FULL, DOUBLE, FULL, FULL};

uint8_t const   SequenceDetector::k_sequence_length = sizeof(SequenceDetector::k_expected_sequence) / sizeof(SequenceDetector::k_expected_sequence[0]);


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
    } else if (m_was_pushed && m_push_t - millis() >= BUTTON_DELAY_MILLIS) {
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
  leds_controller.init();
  sequence_detector.init();
  button_observer.init();
}

void loop() {
  leds_controller.step();
  sequence_detector.step();
  button_observer.step();
}

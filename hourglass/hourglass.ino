/**
 * This is an enhanced version of project n. 8 of the manual.
 * When the hourglass completes, all the leds will start blinking.
 *
 * I implemented the behavior like a state machine with two states:
 *   - hourglass behavior
 *   - blink behavior.
 *
 * The code should be quite self explanatory, but I also inserted
 * some comments.
 *
 * Author: Michele De Stefano
 */

// The following global constants tune the behavior of the hourglass
static uint8_t const NUM_LEDS = 6;
static uint8_t const MAX_LED_INDEX = NUM_LEDS - 1;
static const uint8_t LED_PINS[NUM_LEDS] = {2, 3, 4, 5, 6, 7};
static uint8_t const SWITCH_PIN = 10;

static unsigned long const LED_INTERVAL_MILLIS = 5000;
static unsigned long const LED_BLINK_INTERVAL_MILLIS = 1000;

class Behavior {
public:

  virtual void reset() = 0;

  virtual bool step() = 0; // Returns true if complete

protected:

  void set_all_leds_to(uint8_t level) {
    for (uint8_t i = 0; i < NUM_LEDS; digitalWrite(LED_PINS[i++], level));
  }
};

class Hourglass : public Behavior {
public:

  void reset() override {
    set_all_leds_to(LOW);
    m_led_idx = -1;
    m_last_time_record = millis();
  }

  bool step() override {
    unsigned long const cur_time = millis();
    if (cur_time - m_last_time_record > LED_INTERVAL_MILLIS) {
      // NOTE: The following max is a safety measure for not incurring in segmentation faults
      m_led_idx = max(m_led_idx + 1, MAX_LED_INDEX);
      digitalWrite(LED_PINS[m_led_idx], HIGH);
      m_last_time_record = cur_time;
    }
    return m_led_idx == MAX_LED_INDEX;
  }

private:

  unsigned long m_last_time_record;

  uint8_t m_led_idx;
};


class Blink : public Behavior {
public:
  
  void reset() override {
    m_initial_switch_state = static_cast<uint8_t>(digitalRead(SWITCH_PIN));
    m_led_state = HIGH;
    set_all_leds_to(m_led_state);
    m_last_time_record = millis();
  }
  
  bool step() override {
    unsigned long const cur_time = millis();
    if (cur_time - m_last_time_record > LED_BLINK_INTERVAL_MILLIS) {
      m_led_state = !m_led_state;
      set_all_leds_to(m_led_state);
      m_last_time_record = cur_time;
    }
    return static_cast<uint8_t>(digitalRead(SWITCH_PIN)) != m_initial_switch_state;
  }

private:

  unsigned long m_last_time_record;

  uint8_t m_initial_switch_state;

  uint8_t m_led_state;
};

class StateMachine : public Behavior {
public:

  StateMachine() : m_behavior(nullptr) {}

  void setup() {
    pinMode(SWITCH_PIN, INPUT);
    for (uint8_t i = 0; i < NUM_LEDS; pinMode(LED_PINS[i++], OUTPUT));
    reset();
  }

  bool step() override {
    if (m_behavior->step()) {
      // When we enter here, the current behavior is complete and we switch behavior
      Behavior *aux_behavior = m_behavior;
      m_behavior = m_next_behavior;
      m_next_behavior = aux_behavior;
      m_behavior->reset();
    }
    return false;
  }

private:

  void reset() override {
    m_behavior = &m_blink;
    m_next_behavior = &m_hourglass;
    m_behavior->reset();
  }

private:

  Hourglass m_hourglass;

  Blink     m_blink;

  Behavior *m_behavior;

  Behavior *m_next_behavior;
};

static StateMachine state_machine;

void setup() {
  // put your setup code here, to run once:
  state_machine.setup();
}

void loop() {
  // put your main code here, to run repeatedly:
  state_machine.step();
}

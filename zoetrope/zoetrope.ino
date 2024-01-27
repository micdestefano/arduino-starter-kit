/**
 * This is a variation on project n. 10 into the starter kit manual.
 *
 * In the software implementation I am using the "Observer design
 * pattern": I have one observer for each pushbutton, while the
 * observed object is the motor controller. This code is simply
 * a slight modification with respect to that of project n. 9,
 * thanks to the use of the observer pattern and of member function
 * pointers.
 *
 * It is highly advisable to read the datasheet of the H-Bridge.
 * The one in my kit is a Texas Instruments L239D.
 *
 * Author: Michele De Stefano
 */

static uint8_t const ON_OFF_PIN = 12;
static uint8_t const DIRECTION_TOGGLE_PIN = 11;
static uint8_t const MOTOR_PWM_PIN = 5;
static const uint8_t MOTOR_OUT_PINS[2] = {2, 3};
static const uint8_t MOTOR_SPEED_PIN = A0;

class MotorController {
public:

  void init() {
    pinMode(MOTOR_PWM_PIN, OUTPUT);
    for (int i = 0; i < 2; ++i) {
      pinMode(MOTOR_OUT_PINS[i], OUTPUT);
      digitalWrite(MOTOR_OUT_PINS[i], m_motor_out_values[i]);
    }
  }

  void toggle_on_off() {
    m_is_on = !m_is_on;
    Serial.print("Motor is on: ");
    Serial.println(m_is_on);
  }

  void invert_direction() {
    for (int i = 0; i < 2; ++i) {
      m_motor_out_values[i] = !m_motor_out_values[i];
      digitalWrite(MOTOR_OUT_PINS[i], m_motor_out_values[i]);
    }
    Serial.println("Direction inverted");
  }

  void step() {
    if (m_is_on) {
      int const speed = static_cast<int>(map(analogRead(MOTOR_SPEED_PIN), 0, 1023, 0, 255));
      Serial.print("Motor speed: ");
      Serial.println(speed);
      analogWrite(MOTOR_PWM_PIN, speed);
    } else {
      analogWrite(MOTOR_PWM_PIN, 0);
    }
  }

private:

  bool m_is_on = false;
  uint8_t m_motor_out_values[2] = {LOW, HIGH};
};

class PushbuttonObserver {
public:

  typedef void (MotorController::* motor_action_type)();

  PushbuttonObserver(
    uint8_t input_pin,
    motor_action_type motor_action,
    char* const toggle_msg
  ) :
  m_input_pin(input_pin),
  m_motor_action(motor_action),
  m_toggle_msg(toggle_msg)
  {}

  void init(MotorController* motor_controller) {
    m_motor_controller = motor_controller;
    pinMode(m_input_pin, INPUT);
  }

  void step() {
    if (digitalRead(m_input_pin) == HIGH) {
      m_was_pushed = true;
    } else if (m_was_pushed) {
      m_was_pushed = false;
      (m_motor_controller->*m_motor_action)();
      Serial.println(m_toggle_msg);
    }
  }

private:

  uint8_t const m_input_pin;

  motor_action_type const m_motor_action;

  char* const m_toggle_msg;

  MotorController* m_motor_controller = nullptr;

  bool m_was_pushed = false;
};


static const char*        on_off_msg = "Toggle on/off";

static const char*        toggle_direction_msg = "Toggle direction";

static MotorController    motor_controller;

static PushbuttonObserver on_off_observer(
                            ON_OFF_PIN,
                            &MotorController::toggle_on_off,
                            on_off_msg
                          );

static PushbuttonObserver direction_observer(
                            DIRECTION_TOGGLE_PIN,
                            &MotorController::invert_direction,
                            toggle_direction_msg
                          );

void setup() {
  Serial.begin(9600);
  motor_controller.init();
  on_off_observer.init(&motor_controller);
  direction_observer.init(&motor_controller);
  Serial.println("Setup finished");
}

void loop() {
  on_off_observer.step();
  direction_observer.step();
  motor_controller.step();
}

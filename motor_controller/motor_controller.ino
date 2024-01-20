/**
 * This is a variation on project n. 9 into the starter kit manual.
 * Here I am controlling the motor speed through a potentiometer.
 * With respect to the circuit shown into the manual, I've added
 * an analog input from the potentiometer.
 *
 * In the software implementation I am using the "Observer design
 * pattern": the observer is observing the switch, while the
 * observed object is the motor controller. Each press to the
 * switch toggles the motor on and off. The potentiometer controls
 * the speed.
 *
 * Author: Michele De Stefano
 */

static uint8_t const TRIMMER_PIN = A0;
static uint8_t const SWITCH_PIN = 12;
static uint8_t const MOTOR_PIN = 9;


class MotorController {
public:

  void init() {
    pinMode(MOTOR_PIN, OUTPUT);
  }

  void toggle() {
    m_is_on = !m_is_on;
    Serial.print("Motor is on: ");
    Serial.println(m_is_on);
  }

  void step() {
    if (m_is_on) {
      int const speed = static_cast<int>(map(analogRead(TRIMMER_PIN), 0, 1023, 0, 255));
      Serial.print("Motor speed: ");
      Serial.println(speed);
      analogWrite(MOTOR_PIN, speed);
    } else {
      analogWrite(MOTOR_PIN, 0);
    }
  }

private:

  bool m_is_on = false;
};


class SwitchObserver {
public:

  void init(MotorController* motor_controller) {
    m_motor_controller = motor_controller;
    pinMode(SWITCH_PIN, INPUT);
  }

  void step() {
    if (digitalRead(SWITCH_PIN) == HIGH) {
      m_was_pushed = true;
    } else if (m_was_pushed) {
      m_was_pushed = false;
      m_motor_controller->toggle();
      Serial.println("Toggle motor");
    }
  }

private:

  MotorController* m_motor_controller = nullptr;

  bool m_was_pushed = false;
};

static MotorController  motor_controller;

static SwitchObserver   switch_observer;

void setup() {
  Serial.begin(9600);
  motor_controller.init();
  switch_observer.init(&motor_controller);
  Serial.println("Setup finished");
}

void loop() {
  switch_observer.step();
  motor_controller.step();
}

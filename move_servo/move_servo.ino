/**
 * This is project n. 5 into the Arduino starter kit manual.
 *
 * Author: Michele De Stefano
 */
#include <Servo.h>

static uint8_t const HANDLE = A0;
static uint8_t const SERVO = 3;
static bool const VERBOSE = true;

class ServoController {
public:
  
  ServoController(bool verbose) :
    k_verbose(verbose) {}

  void init() {
    pinMode(SERVO, OUTPUT);
    m_servo.attach(SERVO);
    if (k_verbose) {
      Serial.println("handle,clamped_handle,angle");
    }
  }

  void set() {
    int const handle_val = analogRead(HANDLE);
    int const handle_clamped = min(max(handle_val, k_handle_min), k_handle_max);
    long const angle = map(handle_clamped, k_handle_min, k_handle_max, k_angle_min, k_angle_max);
    if (k_verbose) {
      // NOTE: The following is a valid output that can be also parsed by the serial plotter. Separate with commas, or, spaces, or tabs.
      // Don't put a space between things (because the space is a possible separator). Use a colon after the label.
      Serial.print("handle:"); Serial.print(handle_val); Serial.print(",clamped_handle:"); Serial.print(handle_clamped); Serial.print(",angle:"); Serial.println(angle);
    }
    m_servo.write(angle);
  }

private:
  bool const k_verbose;
  int const k_handle_min = 0;
  int const k_handle_max = 1023;
  int const k_angle_min = 15;
  int const k_angle_max = 179;

  Servo m_servo;
};

static ServoController controller(VERBOSE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  controller.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  controller.set();
  delay(15);
}

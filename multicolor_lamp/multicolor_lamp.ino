static uint8_t const

  IN_R_PIN = A0
, IN_G_PIN = A1
, IN_B_PIN = A2

, OUT_R_PIN = 9
, OUT_G_PIN = 10
, OUT_B_PIN = 11;


class RGBController {
public:

  void init() {
    for (uint8_t i = 0; i < 3; pinMode(k_out_pins[i++], OUTPUT));
  }

  void set_color() {
    // Read inputs for colors, with a delay to allow AD converter to do its job
    for (uint8_t i = 0; i < 3; ++i) {
      m_colors[i] = analogRead(k_in_pins[i]) / in_to_out;
      delay(5);
    }
    // Set colors on the RGB LED
    for (uint8_t i = 0; i < 3; ++i) {
      analogWrite(k_out_pins[i], m_colors[i]);
    }
  }

private:

  uint8_t const in_to_out = 4;
  uint8_t const k_in_pins[3]  = {IN_R_PIN, IN_G_PIN, IN_B_PIN};
  uint8_t const k_out_pins[3] = {OUT_R_PIN, OUT_G_PIN, OUT_B_PIN};

  uint8_t m_colors[3] = {0};
};

static RGBController rgb_controller;

void setup() {
  // put your setup code here, to run once:
  rgb_controller.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  rgb_controller.set_color();
}

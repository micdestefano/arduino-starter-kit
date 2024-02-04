/**
 * This project is not present into the Arduino starter kit.
 * Basically this is a substitute of proj. n. 14, where you
 * should have setup serial communication with your computer
 * and change a logo. I did not like that project so I went
 * straight on with creating my project exploiting serial
 * communication.
 *
 * Here the Arduino board is connected to an RGB led and
 * is receiving the color values from a Python program
 * running on the computer.
 *
 * Author: Michele De Stefano
 */

struct Color {
  uint8_t R;
  uint8_t G;
  uint8_t B;
};

class LEDController {
public:

  LEDController(uint8_t R_PIN, uint8_t G_PIN, uint8_t B_PIN) :
    k_R_PIN(R_PIN), k_G_PIN(G_PIN), k_B_PIN(B_PIN)
  {}

  void init() {
    pinMode(k_R_PIN, OUTPUT);
    pinMode(k_G_PIN, OUTPUT);
    pinMode(k_B_PIN, OUTPUT);
    reset();
  }

  void reset() {
    analogWrite(k_R_PIN, 0);
    analogWrite(k_G_PIN, 0);
    analogWrite(k_B_PIN, 0);
  }

  void set(Color const& color) {
    analogWrite(k_R_PIN, color.R);
    analogWrite(k_G_PIN, color.G);
    analogWrite(k_B_PIN, color.B);
  }

private:

  uint8_t const k_R_PIN;
  uint8_t const k_G_PIN;
  uint8_t const k_B_PIN;
};

class ColorReceiver {
public:

  Color const& recv_color() {
    char* color_ptr = reinterpret_cast<char*>(&m_color);
    for (uint8_t read_bytes = 0; read_bytes < k_data_size; read_bytes = color_ptr - reinterpret_cast<char*>(&m_color)) {
      color_ptr += Serial.readBytes(color_ptr, k_data_size - read_bytes);
    }
    return m_color;
  }

private:

  static uint8_t const  k_data_size = sizeof(Color);

  Color                 m_color;
};

static LEDController  led_controller(9, 10, 11);
static ColorReceiver  receiver;

void setup() {
  Serial.begin(9600);
  led_controller.init();
}

void loop() {
  Color const& new_color = receiver.recv_color();
  led_controller.set(new_color);
}

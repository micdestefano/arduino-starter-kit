/**
 * This is a variation on project n. 11 into the starter kit manual.
 *
 * In my implementation I am not using a tilt sensor. I am simply
 * using a push button. I rearranged the connections with respect
 * to what it is suggested into the manual, so that the circuit is
 * cleaner on the breadboard (I added a photo of the final circuit).
 * Notice that this project does not require any PWM signal. All
 * the pins used for LCD control are used as digital pins. Notice
 * also that I used a 1k resistor for the "LED+" pin. In the manual
 * it is suggested to use a 220 ohm resistor, but I read the data
 * sheet and there is no suggestion on the resistance. The data
 * sheet simply says not to connect that pin directly to 5V Vcc.
 * So I started with 220 ohm and then tried to raise the resistance.
 *  
 * In the software implementation I am using the "Observer design
 * pattern": I have one observer for the pushbutton, while the
 * observed object is the LCD controller.
 *
 * Final note: the messages of my sphere are in Italian.
 *
 * Author: Michele De Stefano
 */

#include <LiquidCrystal.h>

static uint8_t const RS_PIN(5), ENABLE_PIN(8), D4_PIN(10), D5_PIN(11), D6_PIN(12), D7_PIN(13);
static uint8_t const SWITCH_PIN(9);
static unsigned long const RESET_DELAY_MILLIS = 5000;
static unsigned long const ANSWER_DELAY_MILLIS = 500;

class LiquidCrystalController {
public:

  LiquidCrystalController() :
    m_lcd(RS_PIN, ENABLE_PIN, D4_PIN, D5_PIN, D6_PIN, D7_PIN)
  {}

  void init() {
    m_lcd.begin(16, 2);
    reset();
    Serial.print("Num. possible answers: ");
    Serial.println(k_num_answers);
  }

  void provide_answer() {
    long const answer_index = random(k_num_answers);
    m_lcd.clear();
    m_lcd.print(k_answers[answer_index]);
    m_last_answer_t = millis();
    m_needs_reset = true;
  }

  void step() {
    if (m_needs_reset && millis() - m_last_answer_t >= RESET_DELAY_MILLIS) reset(); 
  }

private:

  void reset() {
    m_lcd.clear();
    m_lcd.print("Interroga");
    m_lcd.setCursor(0, 1);
    m_lcd.print("la sfera!");
    m_needs_reset = false;
    m_last_answer_t = 0;
  }

private:

  static const String k_answers[];

  static const uint8_t k_num_answers;

  LiquidCrystal m_lcd;

  unsigned long m_last_answer_t = 0;

  bool m_needs_reset = false;
};

const String LiquidCrystalController::k_answers[] = {
  "Improbabile", "Si", "No", "Non te lo dico", "E che ne so?", "Tu lo sai", "Forse",
  "Stai scherzando?", "Ho parlato", "Impossibile", "Pplrrr!!!", "Questa e' la via",
  "Usa la Forza!", "Sicuro al 100%", "Certo", "Sicuro al 90%", "Sulla Terra?", "Su Marte?"
};

const uint8_t LiquidCrystalController::k_num_answers =
  sizeof(LiquidCrystalController::k_answers) / sizeof(LiquidCrystalController::k_answers[0]);


class PushbuttonObserver {
public:

  PushbuttonObserver() : m_input_pin(SWITCH_PIN) {}

  void init(LiquidCrystalController* lcd_controller) {
    m_lcd_controller = lcd_controller;
    pinMode(m_input_pin, INPUT);
  }

  void step() {
    if (!m_was_pushed && digitalRead(m_input_pin) == HIGH) {
      m_was_pushed = true;
      m_push_t = millis();
    } else if (m_was_pushed && m_push_t - millis() >= ANSWER_DELAY_MILLIS) {
      m_lcd_controller->provide_answer();
      m_was_pushed = false;
      m_push_t = 0;
    }
  }

private:

  uint8_t const m_input_pin;

  LiquidCrystalController* m_lcd_controller = nullptr;

  bool m_was_pushed = false;

  unsigned long m_push_t = 0;
};


static LiquidCrystalController lcd_controller;

static PushbuttonObserver button_observer;

void setup() {
  Serial.begin(9600);
  lcd_controller.init();
  button_observer.init(&lcd_controller);
  Serial.println("Setup finished");
}

void loop() {
  button_observer.step();
  lcd_controller.step();
}

/**
 * This is an enhanced version of project n. 7 of the manual.
 * The selection of the tones is precise here.
 *
 * The code assumes the following configuration for the input switches:
 *
 *         Vcc = 5V
 *          |
 *          |------- SW_1 ----|
 *          |                 |
 *          |--R2--- SW_2 ----|
 *          |                 |
 *          |--R3--- SW_3 ----|
 *          |                 |
 *          |--R4--- SW_4 ----|---- Vin (to Arduino analog input)
 *                            |
 *                            Rd
 *                            |
 *                           ---
 *                           GND = 0V
 *
 * Where SW_i is the i-th switch, Ri is the i-th resistor and Rd is the
 * pull-down resistor.
 *
 * I recomputed the Ri values so that Vin changes linearly between 0V
 * (the minimum) and 5V (the maximum) depending on which switch is pressed.
 * Ideally, we would like the following situation to happen:
 *
 *      Pressed switch  |   Vin / Vcc
 *      ------------------------------
 *             1        |     1.00
 *             2        |     0.75
 *             3        |     0.50
 *             4        |     0.25
 *          no press    |     0.00
 *
 * Given that Rd = 10k (typical value for the pulldow resistor), we can
 * compute the desired values for Ri. The desired values are:
 *   - R2 = 3.22k
 *   - R3 = 10k
 *   - R4 = 30k
 *
 * With the available resistors in the kit I could get close to these values
 * by doing the following:
 *   - R2: series of 3 1k resistors + 1 220 ohm resistor
 *   - R3: simply use one 10k resistor
 *   - R4: series of 3 10k resistors.
 *
 * Because of the limited space on the breadboard (look also at the figure on
 * the manual), I decided to use a single 4.7k resistor for R2. So we have the
 * following situation:
 *
 *      Pressed switch  |   Vin / Vcc |  encoded_tone = (int)(10 * Vin/Vcc)
 *      -------------------------------------------------------------------
 *             1        |     1.00    |                10
 *             2        |     0.68    |                 6
 *             3        |     0.50    |                 5
 *             4        |     0.25    |                 2
 *          no press    |     0.00    |                 0
 *
 * Now recall that Vin, when read on any of the analog inputs, is quantized
 * between 0 and 1023. If we remap it between 0 and 1 we can detect which button
 * has been pressed (according to the table above). Problem: the remapping returns
 * an integer (long) value. For this reason I decided to remap the values between
 * 0 and 10. The remapped value (encoded_tone) is visible in the third column above.
 *
 * The implementation below is quite precise and conservative. I did not
 * experience any fluctuation on the produced tone.
 *
 * Final note: a trimmer in series with the piezo allows to control the volume.
 *
 * Author: Michele De Stefano
 */

static uint8_t INPUT_PIN = A0;
static uint8_t PIEZO_PIN = 8;

class KeyboardManager {
public:

  void init() {
    pinMode(PIEZO_PIN, OUTPUT);
  }

  unsigned int play() {
    int const quantized_tone = analogRead(INPUT_PIN);
    unsigned int const freq = select_tone(quantized_tone);
    if (freq == NONE) {
      noTone(PIEZO_PIN);
    } else {
      tone(PIEZO_PIN, freq);
    }
    return freq;
  }

private:

  enum {
    NONE = 0,
    C = 262,
    D = 294,
    E = 330,
    F = 349
  };

  static unsigned int const k_mapped_tones[11];

  unsigned int select_tone(int quantized_tone) {
    long const encoded_tone = map(quantized_tone, 0, 1023, 0, 10);
    // === Debug info ===
    Serial.print("encoded_tone:");
    Serial.println(encoded_tone);
    // ==================
    return k_mapped_tones[encoded_tone];
  }
};

/*
 * Trick: the following array allows to use the encoded_tone as the index into
 * the array, for obtaining the corresponding tone frequency. It is quite conservative
 * and safely remaps different codes to the same frequency. Notice that this remapping
 * will work also if we use R2 = 3.22k.
 */
unsigned int const KeyboardManager::k_mapped_tones[11] = {NONE, F, F, F, E, E, D, D, D, C, C};

static KeyboardManager keyboard;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  keyboard.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned int freq = keyboard.play();
  // === Debug info ===
  Serial.print("frequency:");
  Serial.println(freq);
  // ==================
}

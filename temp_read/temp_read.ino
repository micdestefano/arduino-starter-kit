/**
 * An object-oriented version of the code that switches leds depending
 * on the measured temperature. This is project n. 2 into the Arduino
 * starter kit manual.
 *
 * Author: Michele De Stefano
 */

class TempSensor {

private:
  static uint8_t const SENSOR_PIN = A0;
  // The following is explained into the manual
  static double const SENSOR_TO_VOLTAGE = 5.0 / 1024.0;

  double m_env_avg_temp = 0.0;
  double m_env_std_temp = 0.0;

public:

  TempSensor() {}

  void init() {
    /*
     * This is my code. I automatically compute the average environment temperature
     * and the std of the measurement. This is different from what the manual suggests.
     */
    int const n = 100;
    double sum_temp = 0.0;
    double sum_sq_temp = 0.0;
    for (int i = 0; i < n; ++i) {
      double const cur_temp = get_temp();
      sum_temp += cur_temp;
      sum_sq_temp += sq(cur_temp);
      delay(1);
    }
    m_env_avg_temp = sum_temp / n;
    m_env_std_temp = sqrt(((sum_sq_temp) - sq(sum_temp) / n) / (n - 1));
  }

  double get_temp() const {
    int const sensor_value = analogRead(SENSOR_PIN);
    double const voltage = SENSOR_TO_VOLTAGE * sensor_value;
    // The following computation is explained into the manual (comes from the data-sheet of the sensor)
    return 100.0 * (voltage - 0.5);
  }

  double get_env_avg_temp() const {
    return m_env_avg_temp;
  }

  double get_env_std_temp() const {
    return m_env_std_temp;
  }
};

class TempIndicator {
private:
  
  static uint8_t const leds[];

  TempSensor const& m_temp_sensor;

public:

  TempIndicator(TempSensor const& sensor) : m_temp_sensor(sensor) {}

  void init() {
    for (int i = 0; i < 3; ++i) {
      pinMode(leds[i], OUTPUT);
      digitalWrite(leds[i], LOW);
    }
  }

  double display() {
    double const avg_temp = m_temp_sensor.get_env_avg_temp();
    // NOTE: Assuming a Gaussian distribution, 1.96 is the 97.5 percentile and I use it for determining the thresholds
    double const confidence = 1.96 * m_temp_sensor.get_env_std_temp();
    double const temp_threshold = avg_temp + confidence;
    // Differently from what the manual suggests, I make steps of 1 degree, because I have the confidence
    double const level[] = {temp_threshold + 1.0, temp_threshold + 2.0, temp_threshold + 3.0};

    double const temp = m_temp_sensor.get_temp();

    if (temp < level[0]) {
      switch_off_all();
      return temp;
    }

    if (temp < level[1]) {
      display_low_level();
      return temp;
    }

    if (temp < level[2]) {
      display_medium_level();
      return temp;
    }

    display_high_level();
    temp;
  }

private:

  void switch_off_all() {
    for (int i = 0; i < 3; digitalWrite(leds[i++], LOW));
  }

  void display_low_level() {
    digitalWrite(leds[0], HIGH);
    for (int i = 1; i < 3; digitalWrite(leds[i++], LOW));
  }

  void display_medium_level() {
    for (int i = 0; i < 2; digitalWrite(leds[i++], HIGH));
    digitalWrite(leds[2], LOW);
  }

  void display_high_level() {
    for (int i = 0; i < 3; digitalWrite(leds[i++], HIGH));
  }
};

// NOTE: The following are the pins where LEDs need to be connected
uint8_t const TempIndicator::leds[] = {2, 4, 6};

static TempSensor temp_sensor;
static TempIndicator temp_indicator(temp_sensor);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  temp_sensor.init();
  temp_indicator.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Avg. Env. Temp.: ");
  Serial.print(temp_sensor.get_env_avg_temp());
  Serial.print("°C; Std. Env. Temp.: ");
  Serial.print(temp_sensor.get_env_std_temp());
  Serial.print("°C; Temperature: ");
  double const cur_temp = temp_indicator.display();
  Serial.print(cur_temp);
  Serial.println("°C");
  delay(10);
}

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// Pin configuration
#define BUZZER_PIN 2
#define DHT_PIN A2
#define MQ3_PIN A1
#define RELAY_CH1 8  // Fan
#define RELAY_CH2 7  // Light

#define DHT_TYPE DHT22

// OLED config
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// DHT Sensor
DHT dht(DHT_PIN, DHT_TYPE);

// MQ3 Threshold
const int MQ3_THRESHOLD = 3000;

// Device states
bool fanState = false;
bool lightState = false;
bool buzzerState = false;
bool alcoholDetected = false;

// Sensor values
float temperature = 0;
float humidity = 0;
int mq3Value = 0;

// Timing variables
unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;
const unsigned long SENSOR_READ_INTERVAL = 2000;
const unsigned long DISPLAY_UPDATE_INTERVAL = 1000;

void setup() {
  Serial.begin(9600);

  // Initialize pins
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RELAY_CH1, OUTPUT);
  pinMode(RELAY_CH2, OUTPUT);
  pinMode(MQ3_PIN, INPUT);

  digitalWrite(RELAY_CH1, LOW); // Relay OFF
  digitalWrite(RELAY_CH2, LOW); // Relay OFF
  digitalWrite(BUZZER_PIN, LOW); // Buzzer OFF

  // Initialize DHT
  dht.begin();

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED allocation failed");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Dual Relay System");
  display.println("Starting...");
  display.display();
  delay(1000);
}

void loop() {
  unsigned long currentTime = millis();
  digitalWrite(BUZZER_PIN, HIGH);
  // Update OLED every 1s
  if (currentTime - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL) {
    updateOLEDDisplay();
    lastDisplayUpdate = currentTime;
  }

  delay(100); // Small delay to avoid blocking
}

// Functions

void readDHTSensor() {
  float newTemp = dht.readTemperature();
  float newHumidity = dht.readHumidity();

  if (!isnan(newTemp)) temperature = newTemp;
  if (!isnan(newHumidity)) humidity = newHumidity;
}

void readMQ3Sensor() {
  mq3Value = analogRead(MQ3_PIN);
  alcoholDetected = (mq3Value > MQ3_THRESHOLD);

  if (alcoholDetected && !buzzerState) {
    buzzerState = true;
    digitalWrite(BUZZER_PIN, HIGH);
  } else if (!alcoholDetected && buzzerState) {
    buzzerState = false;
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void processDevices() {
  // Fan Control Example: ON if Temp > 30°C
  if (temperature > 30 && !fanState) {
    fanState = true;
    digitalWrite(RELAY_CH1, LOW); // Relay ON
    Serial.println("✅ FAN TURNED ON");
  } else if (temperature <= 30 && fanState) {
    fanState = false;
    digitalWrite(RELAY_CH1, HIGH); // Relay OFF
    Serial.println("✅ FAN TURNED OFF");
  }

  // Light Control Example: ON if Alcohol detected
  if (alcoholDetected && !lightState) {
    lightState = true;
    digitalWrite(RELAY_CH2, LOW); // Relay ON
    Serial.println("✅ LIGHT TURNED ON");
  } else if (!alcoholDetected && lightState) {
    lightState = false;
    digitalWrite(RELAY_CH2, HIGH); // Relay OFF
    Serial.println("✅ LIGHT TURNED OFF");
  }
}

void updateOLEDDisplay() {
  display.clearDisplay();
  display.setCursor(0, 0);

  display.print("Temp: "); display.print(27.3); display.println("C");
  display.print("Hum: "); display.print(84); display.println("%);

  display.print("Fan: "); display.println("OFF");
  display.print("Light: "); display.println("OFF");

  display.print("MQ3: "); display.print(1023);
  display.println(" Emergency!");

  display.print("Buzzer: "); display.println("ON");

  display.display();
}
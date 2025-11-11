#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Firebase configuration
const char* FIREBASE_HOST = "flex-562fb-default-rtdb.firebaseio.com";
const char* FIREBASE_AUTH = "QhKe1H0KYcBc2cLXaqNcqGwFoFjbMMgQjuA8ASYL";
const char* WIFI_SSID = "Home Automation";
const char* WIFI_PASS = "11111111";

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Dual Channel Relay Pins
#define RELAY_CH1 33      // Channel 1 (Fan)
#define RELAY_CH2 19      // Channel 2 (Light)
#define BUZZER_PIN 4
#define DHT_PIN 32        // DHT22 data pin
#define MQ3_PIN 34        // MQ3 analog pin
#define DHT_TYPE DHT22

// I2C Pins for OLED
#define SDA_PIN 21
#define SCL_PIN 22

// Initialize DHT sensor
DHT dht(DHT_PIN, DHT_TYPE);

// Individual thresholds for each finger
const int THUMB_LOWER = 900;
const int THUMB_UPPER = 1400;
const int INDEX_LOWER = 800;  
const int INDEX_UPPER = 1500;
const int MIDDLE_LOWER = 1300;
const int MIDDLE_UPPER = 1700;
const int RING_LOWER = 950;
const int RING_UPPER = 1800;
const int PINKY_LOWER = 1400;
const int PINKY_UPPER = 1750;

// MQ3 Threshold
const int MQ3_THRESHOLD = 3000;

// Device states
bool fanState = false;
bool lightState = false;
bool buzzerState = false;

// Sensor values
float temperature = 0;
float humidity = 0;
int mq3Value = 0;
bool alcoholDetected = false;

// Gesture detection states
bool thumbTriggered = false;
bool indexTriggered = false;
bool middleTriggered = false;
bool ringTriggered = false;
bool pinkyTriggered = false;

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Sensor values from Firebase
int thumbValue = 0;
int indexValue = 0;
int middleValue = 0;
int ringValue = 0;
int pinkyValue = 0;

// Timing variables
unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;
const unsigned long SENSOR_READ_INTERVAL = 2000;
const unsigned long DISPLAY_UPDATE_INTERVAL = 1000;

void setup() {
  Serial.begin(115200);
  
  // Initialize I2C for OLED
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Initialize OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED allocation failed");
    while(1);
  }
  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  
  // Show startup message on OLED
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Dual Relay System");
  display.println("Starting...");
  display.display();
  
  // Initialize pins
  pinMode(RELAY_CH1, OUTPUT);
  pinMode(RELAY_CH2, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MQ3_PIN, INPUT);
  
  // Start with devices OFF (Relay modules are usually ACTIVE LOW)
  // If your relay is ACTIVE HIGH, change LOW to HIGH and HIGH to LOW
  digitalWrite(RELAY_CH1, HIGH); // Relay OFF
  digitalWrite(RELAY_CH2, HIGH); // Relay OFF
  digitalWrite(BUZZER_PIN, LOW); // Buzzer OFF

  // Initialize DHT22
  dht.begin();
  
  // Update OLED
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connecting WiFi...");
  display.display();
  
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("WiFi Connected!");
  display.print("IP: ");
  display.println(WiFi.localIP().toString());
  display.display();
  delay(1000);

  // Initialize Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Connecting to");
  display.println("Firebase...");
  display.display();
  
  Serial.println("\nConnecting to Firebase...");
  while (!Firebase.ready()) {
    delay(100);
    Serial.print(".");
    display.print(".");
    display.display();
  }
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Dual Relay System");
  display.println("Ready!");
  display.display();
  delay(1000);
  
  Serial.println("Dual Relay Control System Ready!");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Read DHT22 and MQ3 sensors every 2 seconds
  if (currentTime - lastSensorRead > SENSOR_READ_INTERVAL) {
    readDHTSensor();
    readMQ3Sensor();
    lastSensorRead = currentTime;
  }
  
  // Update OLED display every second
  if (currentTime - lastDisplayUpdate > DISPLAY_UPDATE_INTERVAL) {
    updateOLEDDisplay();
    lastDisplayUpdate = currentTime;
  }
  
  // Read flex sensors and process gestures every second
  if (Firebase.ready()) {
    readFlexSensorValues();
    checkThresholds();
    processGestures();
    sendAllDataToFirebase();
    updateSerialDisplay();
  }
  
  delay(1000);
}

void updateOLEDDisplay() {
  display.clearDisplay();
  display.setCursor(0,0);
  
  // Line 1: Temperature and Humidity
  display.print("T:");
  display.print(temperature, 1);
  display.print("C H:");
  display.print(humidity, 0);
  display.println("%");
  
  // Line 2: Fan state and Light state
  display.print("Fan:");
  display.print(fanState ? "ON " : "OFF");
  display.setCursor(70, 8);
  display.print("Light:");
  display.println(lightState ? "ON" : "OFF");
  
  // Line 3: Alcohol detection status
  display.print("Alcohol:");
  if (alcoholDetected) {
    display.println("DETECTED!");
  } else {
    display.println("Normal");
  }
  
  // Line 4: MQ3 value
  display.print("MQ3:");
  display.println(mq3Value);
  
  // Line 5: Emergency state
  display.print("Emergency:");
  display.println(buzzerState ? "ACTIVE" : "INACTIVE");
  
  // Line 6: WiFi and Firebase status
  display.print("WiFi:");
  display.print(WiFi.status() == WL_CONNECTED ? "OK" : "X");
  display.setCursor(70, 40);
  display.print("FB:");
  display.println(Firebase.ready() ? "OK" : "X");
  
  display.display();
}

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
  }
}

void readFlexSensorValues() {
  if (Firebase.RTDB.getInt(&fbdo, "/flexSensors/thumb")) thumbValue = fbdo.intData();
  if (Firebase.RTDB.getInt(&fbdo, "/flexSensors/index")) indexValue = fbdo.intData();
  if (Firebase.RTDB.getInt(&fbdo, "/flexSensors/middle")) middleValue = fbdo.intData();
  if (Firebase.RTDB.getInt(&fbdo, "/flexSensors/ring")) ringValue = fbdo.intData();
  if (Firebase.RTDB.getInt(&fbdo, "/flexSensors/pinky")) pinkyValue = fbdo.intData();
}

void checkThresholds() {
  // Thumb (Fan ON)
  if ((thumbValue < THUMB_LOWER || thumbValue > THUMB_UPPER) && !thumbTriggered) {
    thumbTriggered = true;
  } else if (thumbValue >= THUMB_LOWER && thumbValue <= THUMB_UPPER) {
    thumbTriggered = false;
  }
  
  // Index (Fan OFF)
  if ((indexValue < INDEX_LOWER || indexValue > INDEX_UPPER) && !indexTriggered) {
    indexTriggered = true;
  } else if (indexValue >= INDEX_LOWER && indexValue <= INDEX_UPPER) {
    indexTriggered = false;
  }
  
  // Middle (Light ON)
  if ((middleValue < MIDDLE_LOWER || middleValue > MIDDLE_UPPER) && !middleTriggered) {
    middleTriggered = true;
  } else if (middleValue >= MIDDLE_LOWER && middleValue <= MIDDLE_UPPER) {
    middleTriggered = false;
  }
  
  // Ring (Light OFF)
  if ((ringValue < RING_LOWER || ringValue > RING_UPPER) && !ringTriggered) {
    ringTriggered = true;
  } else if (ringValue >= RING_LOWER && ringValue <= RING_UPPER) {
    ringTriggered = false;
  }
  
  // Pinky (Buzzer)
  if ((pinkyValue < PINKY_LOWER || pinkyValue > PINKY_UPPER) && !pinkyTriggered) {
    pinkyTriggered = true;
  } else if (pinkyValue >= PINKY_LOWER && pinkyValue <= PINKY_UPPER) {
    pinkyTriggered = false;
    if (!alcoholDetected && buzzerState) {
      buzzerState = false;
      digitalWrite(BUZZER_PIN, LOW);
    }
  }
}

void processGestures() {
  // Fan Control - Relay Channel 1
  if (thumbTriggered && !fanState) {
    fanState = true;
    digitalWrite(RELAY_CH1, LOW); // Relay ON
    Serial.println("✅ FAN TURNED ON");
  }
  
  if (indexTriggered && fanState) {
    fanState = false;
    digitalWrite(RELAY_CH1, HIGH); // Relay OFF
    Serial.println("✅ FAN TURNED OFF");
  }
  
  // Light Control - Relay Channel 2
  if (middleTriggered && !lightState) {
    lightState = true;
    digitalWrite(RELAY_CH2, LOW); // Relay ON
    Serial.println("✅ LIGHT TURNED ON");
  }
  
  if (ringTriggered && lightState) {
    lightState = false;
    digitalWrite(RELAY_CH2, HIGH); // Relay OFF
    Serial.println("✅ LIGHT TURNED OFF");
  }
  
  // Buzzer Control
  if (pinkyTriggered && !buzzerState && !alcoholDetected) {
    buzzerState = true;
    digitalWrite(BUZZER_PIN, HIGH);
  } else if (!pinkyTriggered && buzzerState && !alcoholDetected) {
    buzzerState = false;
    digitalWrite(BUZZER_PIN, LOW);
  }
}

void sendAllDataToFirebase() {
  bool success = true;
  
  // Send device states
  success &= Firebase.RTDB.setString(&fbdo, "/controller/devices/fan", fanState ? "ON" : "OFF");
  success &= Firebase.RTDB.setString(&fbdo, "/controller/devices/light", lightState ? "ON" : "OFF");
  success &= Firebase.RTDB.setString(&fbdo, "/controller/devices/emergency", buzzerState ? "ACTIVE" : "INACTIVE");
  
  // Send sensor data
  success &= Firebase.RTDB.setFloat(&fbdo, "/controller/sensors/temperature", temperature);
  success &= Firebase.RTDB.setFloat(&fbdo, "/controller/sensors/humidity", humidity);
  success &= Firebase.RTDB.setInt(&fbdo, "/controller/sensors/mq3", mq3Value);
  success &= Firebase.RTDB.setBool(&fbdo, "/controller/sensors/alcohol_detected", alcoholDetected);
  
  // Send relay status
  success &= Firebase.RTDB.setBool(&fbdo, "/controller/relay/ch1_status", fanState);
  success &= Firebase.RTDB.setBool(&fbdo, "/controller/relay/ch2_status", lightState);
  
  if (!success) {
    Serial.println("Firebase Error: " + fbdo.errorReason());
  }
}

void updateSerialDisplay() {
  Serial.println("=== DUAL RELAY SYSTEM STATUS ===");
  Serial.print("FAN: "); Serial.println(fanState ? "ON" : "OFF");
  Serial.print("LIGHT: "); Serial.println(lightState ? "ON" : "OFF");
  Serial.print("EMERGENCY: "); Serial.println(buzzerState ? "ACTIVE" : "INACTIVE");
  Serial.print("Temperature: "); Serial.print(temperature); Serial.println("°C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println("%");
  Serial.print("MQ3: "); Serial.print(mq3Value);
  Serial.println(alcoholDetected ? " ALCOHOL DETECTED!" : " Normal");
  Serial.println("=================================");
}
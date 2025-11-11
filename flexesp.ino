#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Firebase configuration
const char* FIREBASE_HOST = "flex-562fb-default-rtdb.firebaseio.com";
const char* FIREBASE_AUTH = "QhKe1H0KYcBc2cLXaqNcqGwFoFjbMMgQjuA8ASYL";
const char* WIFI_SSID = "Home Automation";
const char* WIFI_PASS = "11111111";

// SAFE pins for ESP32 DevKit V1
const int FLEX_PINS[5] = {33, 32, 35, 34, 39};

String fingerNames[5] = {"thumb", "index", "middle", "ring", "pinky"};

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long lastDataSend = 0;
const unsigned long SEND_INTERVAL = 100; // Update every 100ms

void setup() {
  Serial.begin(115200);
  
  // Initialize flex sensor pins
  for(int i = 0; i < 5; i++) {
    pinMode(FLEX_PINS[i], INPUT);
  }
  
  Serial.println("Initializing 5 Flex Sensors with ESP32...");
  Serial.println("Using raw analog readings (no averaging)");
  Serial.println("Updates every 100ms");
  
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected! IP: " + WiFi.localIP().toString());

  // Initialize Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("Connecting to Firebase...");
  while (!Firebase.ready()) {
    delay(100);
    Serial.print(".");
  }
  
  Serial.println("Firebase Ready!");
  Serial.println("System Started - Raw Analog Readings Every 100ms");
  Serial.println("===============================================");
}

void loop() {
  // Send raw analog readings to Firebase every 100ms
  if (Firebase.ready() && (millis() - lastDataSend > SEND_INTERVAL)) {
    readAndSendData();
    lastDataSend = millis();
  }
  
  delay(1); // Small delay to prevent watchdog issues
}

void readAndSendData() {
  bool success = true;
  
  // Read and send raw analog readings for each sensor
  for(int i = 0; i < 5; i++) {
    int rawValue = analogRead(FLEX_PINS[i]);
    String path = "/flexSensors/" + fingerNames[i];
    success &= Firebase.RTDB.setInt(&fbdo, path.c_str(), rawValue);
    
    // Print each sensor value
    Serial.print(fingerNames[i]);
    Serial.print(": ");
    Serial.print(rawValue);
    Serial.print(" | ");
  }
  
  if (success) {
    Serial.println("✅ Sent");
  } else {
    Serial.println("❌ Error: " + fbdo.errorReason());
  }
}
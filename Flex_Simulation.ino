// Arduino Uno 5 Flex Sensor Reader
// Reads 5 flex sensors and prints values every 100ms

const int FLEX_PINS[5] = {A0, A1, A2, A3, A4}; // Use analog pins A0–A4
String fingerNames[5] = {"thumb", "index", "middle", "ring", "pinky"};

unsigned long lastDataSend = 0;
const unsigned long SEND_INTERVAL = 100; // 100 ms

void setup() {
  Serial.begin(9600);

  Serial.println("Initializing 5 Flex Sensors with Arduino Uno...");
  Serial.println("Using raw analog readings (no averaging)");
  Serial.println("Updates every 100ms");

  // Set pins as input
  for (int i = 0; i < 5; i++) {
    pinMode(FLEX_PINS[i], INPUT);
  }

  Serial.println("System Ready!");
  Serial.println("===============================================");
}

void loop() {
  if (millis() - lastDataSend > SEND_INTERVAL) {
    readAndPrintData();
    lastDataSend = millis();
  }
}

void readAndPrintData() {
  for (int i = 0; i < 5; i++) {
    int rawValue = analogRead(FLEX_PINS[i]);

    Serial.print(fingerNames[i]);
    Serial.print(": ");
    Serial.print(rawValue);
    Serial.print(" | ");
  }
  Serial.println();
}
#include <Wire.h>
#include <SPI.h>
#include <Servo.h>
#include <Adafruit_PN532.h>

// NFC (SPI)
#define PN532_SCK  (13)
#define PN532_MOSI (11)
#define PN532_SS   (10) 
#define PN532_MISO (12)
Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

// Servo
Servo lockServo;
#define SERVO_PIN 3

// PIR Sensor
#define PIR_PIN 4

// ESP32-CAM trigger pin
#define ESP_TRIGGER_PIN 5

void setup() {
  Serial.begin(115200);
  while (!Serial); // For some boards

  // NFC setup
  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN53x board");
    while (1);
  }
  nfc.SAMConfig();
  Serial.println("Waiting for an ISO14443A Card ...");

  // Servo setup
  lockServo.attach(SERVO_PIN);
  lockServo.write(0);  // Locked

  // PIR & ESP32 setup
  pinMode(PIR_PIN, INPUT);
  pinMode(ESP_TRIGGER_PIN, OUTPUT);
  digitalWrite(ESP_TRIGGER_PIN, LOW);
}

void loop() {
  // 🟠 Check for NFC card
  uint8_t success;
  uint8_t uid[7];    
  uint8_t uidLength; 

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.print("Card scanned. UID: ");
    for (int i = 0; i < uidLength; i++) {
      Serial.print(uid[i], HEX); Serial.print(" ");
    }
    Serial.println();

    // Always unlock when card is scanned
    Serial.println("🔓 Unlocking door...");
    lockServo.write(90);  // Unlock
    delay(3000);          // Hold open for 3 seconds
    lockServo.write(0);   // Lock again
    delay(1000);          // Debounce
  }

  // 🔵 Check for motion
  if (digitalRead(PIR_PIN) == HIGH) {
    Serial.println("🚶 Motion detected! Triggering ESP32-CAM...");
    digitalWrite(ESP_TRIGGER_PIN, HIGH);
    delay(1000);  // Pulse
    digitalWrite(ESP_TRIGGER_PIN, LOW);
    delay(2000);  // Cooldown
  }

  delay(100);
}

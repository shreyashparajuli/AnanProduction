#include <Wire.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <Servo.h>

// PN532 NFC Setup
PN532_I2C pn532i2c(Wire);
PN532 nfc(pn532i2c);

// Servo Setup
Servo lockServo;
const int servoPin = 6;   // Change if needed
const int openAngle = 90; // Adjust based on your lock design
const int closeAngle = 0;

void setup(void) {
  Serial.begin(9600);
  Serial.println("Looking for PN532 (I2C mode)...");

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.println("Didn't find PN532 module :(");
    while (1);
  }

  Serial.println("Found chip PN532");
  Serial.print("Firmware version: ");
  Serial.print((versiondata >> 16) & 0xFF, DEC);
  Serial.print(".");
  Serial.println((versiondata >> 8) & 0xFF, DEC);

  nfc.SAMConfig();
  Serial.println("Waiting for an NFC card...");

  lockServo.attach(servoPin);
  lockServo.write(closeAngle); // Keep door locked at start
}

// Check UID match
bool compareUID(uint8_t *uid, uint8_t *refUID, uint8_t len) {
  for (uint8_t i = 0; i < len; i++) {
    if (uid[i] != refUID[i]) return false;
  }
  return true;
}

// Known allowed UIDs
uint8_t allowedUID1[] = { 0x94, 0x8F, 0xC4, 0x71 };
uint8_t allowedUID2[] = { 0xA3, 0xFA, 0xD9, 0x13 };

void loop(void) {
  uint8_t success;
  uint8_t uid[7];
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {
    Serial.print("Found a card with UID: ");
    for (uint8_t i = 0; i < uidLength; i++) {
      Serial.print(" 0x"); Serial.print(uid[i], HEX);
    }
    Serial.println();

    if ( (uidLength == 4 && compareUID(uid, allowedUID1, 4)) ||
         (uidLength == 4 && compareUID(uid, allowedUID2, 4)) ) {
      Serial.println("✅ Access granted - unlocking...");
      lockServo.write(openAngle);
      delay(3000); // Keep unlocked for 3 seconds
      lockServo.write(closeAngle);
      Serial.println("🔒 Locked again.");
    } else {
      Serial.println("⛔ Access denied.");
    }

    delay(1000); // Small delay before next scan
  }
}

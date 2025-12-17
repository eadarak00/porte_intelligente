// Arduino Code - RC522 Read RFID Tag UID with LED
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 7
#define LED_PIN 4  // Pin pour la LED

MFRC522 rfid(SS_PIN, RST_PIN); // Instance de la classe
MFRC522::MIFARE_Key key; 

void setup() { 
  Serial.begin(9600);
  
  // Initialiser la LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Éteindre la LED au démarrage
  
  SPI.begin(); // Initialiser le bus SPI
  rfid.PCD_Init(); // Initialiser le RC522 
  
  Serial.println("Système RFID prêt...");
}

void loop() {
  // Réinitialiser la boucle si aucune nouvelle carte n'est présente
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Vérifier si l'UID a été lu
  if (!rfid.PICC_ReadCardSerial())
    return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  // Allumer la LED
  digitalWrite(LED_PIN, HIGH);
  
  // Afficher l'UID
  Serial.print("RFID Tag UID:");
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println("");
  
  // Garder la LED allumée pendant 1 seconde
  delay(1000);
  
  // Éteindre la LED
  digitalWrite(LED_PIN, LOW);

  // Arrêter la communication avec la carte
  rfid.PICC_HaltA();
}

// Routine pour afficher un tableau d'octets en hexadécimal sur le Serial
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

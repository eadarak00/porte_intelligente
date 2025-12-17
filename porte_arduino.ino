// Arduino Code - RC522 Read RFID Tag UID avec contrôle d'accès
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 7
#define LED_VERTE 4     // Pin pour la LED verte (accès autorisé)
#define LED_ROUGE 5     // Pin pour la LED rouge (accès refusé)

MFRC522 rfid(SS_PIN, RST_PIN); // Instance de la classe
MFRC522::MIFARE_Key key; 

// Liste des UID autorisés (remplacez par vos propres UID)
String uidAutorises[] = {
  "12 AB 34 CD",  
  "69 FC B1 B0"
};
int nombreUidAutorises = 2; // Nombre d'UID dans la liste

void setup() { 
  Serial.begin(9600);
  
  // Initialiser les LEDs
  pinMode(LED_VERTE, OUTPUT);
  pinMode(LED_ROUGE, OUTPUT);
  
  // Éteindre les LEDs au démarrage
  digitalWrite(LED_VERTE, LOW);
  digitalWrite(LED_ROUGE, LOW);
  
  SPI.begin(); // Initialiser le bus SPI
  rfid.PCD_Init(); // Initialiser le RC522 
  
  Serial.println("Système de contrôle d'accès RFID prêt...");
  Serial.println("En attente d'un tag RFID...");
}

void loop() {
  // Réinitialiser la boucle si aucune nouvelle carte n'est présente
  if (!rfid.PICC_IsNewCardPresent())
    return;

  // Vérifier si l'UID a été lu
  if (!rfid.PICC_ReadCardSerial())
    return;

  // Lire l'UID du tag
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) {
      uid += "0";
    }
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) {
      uid += " ";
    }
  }
  uid.toUpperCase();
  
  Serial.print("Tag détecté - UID: ");
  Serial.println(uid);
  
  // Vérifier si l'UID est autorisé
  boolean accesAutorise = false;
  for (int i = 0; i < nombreUidAutorises; i++) {
    if (uid == uidAutorises[i]) {
      accesAutorise = true;
      break;
    }
  }
  
  // Gérer l'accès
  if (accesAutorise) {
    Serial.println(">> ACCES AUTORISE <<");
    autoriserAcces();
  } else {
    Serial.println(">> ACCES REFUSE <<");
    refuserAcces();
  }
  
  // Arrêter la communication avec la carte
  rfid.PICC_HaltA();
  
  // Petite pause avant de pouvoir scanner un nouveau tag
  delay(2000);
}

// Fonction pour autoriser l'accès (LED verte)
void autoriserAcces() {
  // Éteindre la LED rouge au cas où elle serait allumée
  digitalWrite(LED_ROUGE, LOW);
  
  // Allumer la LED verte
  digitalWrite(LED_VERTE, HIGH);
  
  // Faire clignoter la LED verte 2 fois rapidement
  for(int i = 0; i < 2; i++) {
    delay(150);
    digitalWrite(LED_VERTE, LOW);
    delay(150);
    digitalWrite(LED_VERTE, HIGH);
  }
  
  // Garder la LED verte allumée pendant 1 seconde
  delay(1000);
  
  // Éteindre la LED verte
  digitalWrite(LED_VERTE, LOW);
}

// Fonction pour refuser l'accès (LED rouge)
void refuserAcces() {
  // Éteindre la LED verte au cas où elle serait allumée
  digitalWrite(LED_VERTE, LOW);
  
  // Allumer la LED rouge
  digitalWrite(LED_ROUGE, HIGH);
  
  // Faire clignoter la LED rouge 3 fois lentement
  for(int i = 0; i < 3; i++) {
    delay(300);
    digitalWrite(LED_ROUGE, LOW);
    delay(300);
    digitalWrite(LED_ROUGE, HIGH);
  }
  
  // Garder la LED rouge allumée pendant 1 seconde
  delay(1000);
  
  // Éteindre la LED rouge
  digitalWrite(LED_ROUGE, LOW);
}

// Routine pour afficher un tableau d'octets en hexadécimal sur le Serial
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

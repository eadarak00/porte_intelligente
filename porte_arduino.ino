#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 7

#define LED_VERTE 4
#define LED_ROUGE 5
#define BUZZER 3

int tentative = 0;

MFRC522 rfid(SS_PIN, RST_PIN);

// ========= UID AUTORISÉS (STRING) =========
String uidAutorises[] = {
  "12 AB 34 CD",
  "69 FC B1 B0"
};

int nombreUidAutorises = 1;

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();

  pinMode(LED_VERTE, OUTPUT);
  pinMode(LED_ROUGE, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  digitalWrite(LED_VERTE, LOW);
  digitalWrite(LED_ROUGE, LOW);
  digitalWrite(BUZZER, LOW);
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent()) return;
  if (!rfid.PICC_ReadCardSerial()) return;

  String uidLu = lireUIDString();

  Serial.print("UID détecté : ");
  Serial.println(uidLu);

  if (uidEstAutorise(uidLu)) {
    accesAutorise();
  } else {
    accesRefuse();
  }

  rfid.PICC_HaltA();
}

// ========= CONVERSION UID → STRING =========
String lireUIDString() {
  String uid = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    if (rfid.uid.uidByte[i] < 0x10) uid += "0";
    uid += String(rfid.uid.uidByte[i], HEX);
    if (i < rfid.uid.size - 1) uid += " ";
  }
  uid.toUpperCase();
  return uid;
}

// ========= VÉRIFICATION UID =========
bool uidEstAutorise(String uidLu) {
  for (int i = 0; i < nombreUidAutorises; i++) {
    if (uidLu == uidAutorises[i]) {
      return true;
    }
  }
  return false;
}

// ========= ACCÈS AUTORISÉ =========
void accesAutorise() {
  Serial.println("✔ ACCÈS AUTORISÉ");
  tentative = 0;

  digitalWrite(LED_VERTE, HIGH);
  digitalWrite(LED_ROUGE, LOW);

  tone(BUZZER, 1000);
  delay(200);
  noTone(BUZZER);

  delay(1000);
  digitalWrite(LED_VERTE, LOW);
}

// ========= ACCÈS REFUSÉ =========
void accesRefuse() {
  Serial.println("❌ ACCÈS REFUSÉ");
  tentative++;

  digitalWrite(LED_ROUGE, HIGH);
  digitalWrite(LED_VERTE, LOW);

  for (int i = 0; i < 3; i++) {
    tone(BUZZER, 500);
    delay(150);
    noTone(BUZZER);
    delay(100);
  }

  if (tentative >= 3) {
    Serial.println("🚨 ALARME !");
    tone(BUZZER, 2000);
    delay(3000);
    noTone(BUZZER);
    tentative = 0;
  }

  digitalWrite(LED_ROUGE, LOW);
}

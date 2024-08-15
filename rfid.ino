#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10 // RX slave select
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

byte card_ID[4]; // Card UID size 4 bytes
byte Name3[4] = {0x2D, 0xC6, 0x5A, 0x16}; // Third UID card
byte Name4[4] = {0xD3, 0xA4, 0x5F, 0x1A}; // Fourth UID card

// If you want the Arduino to detect the cards only once
int NumbCard[2] = {0}; // This array contains the number of cards, initialized to 0
int j = 0; // Counter for the NumbCard array

int const RedLed = 6;
int const GreenLed = 5;
int const Buzzer = A1;

String Name; // User name
long Number; // User number
int n = 0; // The number of cards you want to detect (optional)

// LCD Configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600); // Initialize serial communications with the PC
  SPI.begin();  // Init SPI bus
  mfrc522.PCD_Init(); // Init MFRC522 card

  // LCD initialization
  lcd.init();
  lcd.backlight();

  Serial.println("CLEARSHEET");                 // Clear starting at row 1
  Serial.println("LABEL,Date,Time,Name,Number");// Make four columns (Date,Time,[Name:"user name"],[Number:"user number"])

  pinMode(RedLed, OUTPUT);
  pinMode(GreenLed, OUTPUT);
  pinMode(Buzzer, OUTPUT);
}

void loop() {
  // Print "Welcome to Our Project" on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome to Our");
  lcd.setCursor(0, 1);
  lcd.print("Project");
  delay(2000);

  // Look for new card
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return; // Go to start of loop if there is no card present
  }
  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return; // If read card serial(0) returns 1, the UID struct contains the ID of the read card.
  }
  
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    card_ID[i] = mfrc522.uid.uidByte[i];

    if (card_ID[i] == Name3[i]) {
      Name = "RAM"; // User name
      Number = 1; // User number
      j = 0; // First number in the NumbCard array : NumbCard[j]
    } else if (card_ID[i] == Name4[i]) {
      Name = "KEVIN"; // User name
      Number = 2; // User number
      j = 1; // Second number in the NumbCard array : NumbCard[j]
    } else {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Unauthorized");
      lcd.setCursor(0, 1);
      lcd.print("Access");
      digitalWrite(GreenLed, LOW);
      digitalWrite(RedLed, HIGH);
      digitalWrite(Buzzer, HIGH); // Activate buzzer
      delay(3000);
      digitalWrite(Buzzer, LOW); // Deactivate buzzer
      goto cont; // Go directly to line 114
    }
  }
  
  // Check if the card is already detected
  if (NumbCard[j] == 0) {
    NumbCard[j] = 1; // Set the card state to detected
    n++; // Increment the number of detected cards (optional)

    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(Name);
    lcd.setCursor(0, 1);
    lcd.print("Roll No: ");
    lcd.print(Number);
    delay(2000);

    // Send data to Excel
    Serial.print("DATA,DATE,TIME," + Name); // Send the Name to Excel
    Serial.print(",");
    Serial.println(Number); // Send the Number to Excel
    digitalWrite(GreenLed, HIGH); // Turn on green LED
    digitalWrite(RedLed, LOW); // Turn off red LED
    digitalWrite(Buzzer, HIGH); // Activate buzzer
    delay(30);
    digitalWrite(Buzzer, LOW); // Deactivate buzzer
    Serial.println("SAVEWORKBOOKAS,Names/WorkNames"); // Save workbook
  } else {
    // Display on LCD
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(Name);
    lcd.setCursor(0, 1);
    lcd.print("Already Scanned");
    delay(2000);

    // Send data to Excel
    Serial.print("DATA,DATE,TIME," + Name); // Send the Name to Excel
    Serial.print(",");
    Serial.println(Number); // Send the Number to Excel
  }

  delay(1000);

cont:
  delay(2000);
  digitalWrite(GreenLed, LOW);
  digitalWrite(RedLed, LOW);
}
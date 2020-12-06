//Dashbutton zum Senden von Mail oder Nachrichten mit LORA, GSM oder Wlan

//************************************************************************
//Initalisierung RFID-Kartenleser ESP32
#include <SPI.h>
#include <MFRC522.h>
const int RST_PIN = 22; // Reset pin
const int SS_PIN = 21; // Slave select pin
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance
//************************************************************************

//************************************************************************
//Setup RFID-Kartenleser ESP32
void setup()
{
Serial.begin(9600); // Initialize serial communications with the PC
SPI.begin(); // Init SPI bus
mfrc522.PCD_Init(); // Init MFRC522
mfrc522.PCD_DumpVersionToSerial(); // Show details of PCD - MFRC522 Card Reader details
Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));
}
//************************************************************************ 


void loop() {
//*************************************************************************
//loop RFID-Kartenleser
// Look for new cards
if ( ! mfrc522.PICC_IsNewCardPresent()) {
return;
}
 
// Select one of the cards
if ( ! mfrc522.PICC_ReadCardSerial()) {
return;
}
 
// Dump debug info about the card; PICC_HaltA() is automatically called
mfrc522.PICC_DumpToSerial(&(mfrc522.uid));
}

//****************************************************************************

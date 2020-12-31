#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_SCK (2)
#define PN532_MOSI (3)
#define PN532_SS (4)
#define PN532_MISO (5)
#define ACCESS_DELAY 5000
#define DENIED_DELAY 2000
#define LED_G (7) //Define green LED pin
#define LED_R (6) //Define red LED pin
#define LED_B (9) //Define blue LED pin

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

void setup()
 
{
  Serial.begin(9600); //Initiate a serial connection  

  //Pin setup
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(LED_B, OUTPUT);

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // Halt
  }
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  // configure board to read RFID tags
  nfc.SAMConfig();
  Serial.println();
}

void loop(void) 

{
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 butes depending on card

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);

  if (success) {

    Serial.print("Detected UID:");
    String content= "";
    byte letter;
    for(byte i=0; i < uidLength; i++) 
    {
      Serial.print(uid[i] < 0x10 ? " 0" : " ");
      Serial.print(uid[i], HEX);
      content.concat(String(uid[i] < 0x10 ? " 0" : " "));
      content.concat(String(uid[i], HEX));
    }
    
    Serial.println("");

    content.toUpperCase();
    if (content.substring(1) == "6B CC 08 92")
    {
      Serial.println("Authorized access");
      Serial.println();
      delay(100);
      digitalWrite(LED_G, HIGH);
      digitalWrite(LED_B, HIGH);
      delay(ACCESS_DELAY);
      digitalWrite(LED_G, LOW);
      digitalWrite(LED_B, LOW);
    }
    else
    {
      Serial.println("Access denied");
      Serial.println("");
      digitalWrite(LED_R, HIGH);
      delay(DENIED_DELAY);
      digitalWrite(LED_R, LOW);
    }
  }

  delay(4000);
}

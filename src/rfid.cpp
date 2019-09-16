#include "rfid.h"

#include "MFRC522/src/MFRC522.h"

/* depends from harcoded length */
#define CACHE_START_INDEX 2

String keys[KEYS_CACHE_SIZE];
//  = {
//     /* remember to adjust  CACHE_START_INDEX !!!!!!!!! */
//     "iddb3e253c",
//     /* remember to adjust  CACHE_START_INDEX !!!!!!!!! */
// };
uint8_t nextEmptyKey = CACHE_START_INDEX;

MFRC522 mfrc522(SS_PIN, RST_PIN);
unsigned long uidDec;
uint64_t uidDecTemp;

String prevCard = "";


void rfid_init(void)
{
    keys[0] = "2a8452f"; /* alyona */
    keys[1] = "db3e253c"; /* tolik */
    keys[2] = "8f91c149"; /* Nick */

    SPI.begin();         // Init SPI bus.
    SPI.setFrequency(500000L);
    mfrc522.PCD_Init();  // Init MFRC522 card.
}

int8_t rfid_scan(String * id)
{
    // Поиск новой метки
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
       return 1;
    }
    // Выбор метки
    if ( ! mfrc522.PICC_ReadCardSerial()) {
       return 1;
    }
    
    String checkingId;
	for (byte i = 0; i < mfrc522.uid.size; i++)
	{
		checkingId += String(mfrc522.uid.uidByte[i], HEX);
	}

    Serial.print("checkingId "); // Выводим UID метки в консоль.
    Serial.print(checkingId); // Выводим UID метки в консоль.
    Serial.println(" "); // Выводим UID метки в консоль.

    *id = checkingId;

    /* legacy code */
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
    mfrc522.PCD_Init();    // Init MFRC522

    return 0;
}

int8_t rfid_validate_id(String id, int8_t * validation_status)
{
    uint8_t is_exist_in_cache = 0; // 0 - is false
    int i;
    for (i = 0; i < KEYS_CACHE_SIZE; i++) {
        if (id.equals(keys[i])){
            is_exist_in_cache = 1;
            break;
        }
    }

    /* if exist */
    if (is_exist_in_cache != 0) {
        *validation_status = 1;
    } else {
        *validation_status = 0;
    }

    return 0;
}

void rfid_cache_id(String id) 
{
    keys[nextEmptyKey] = id;
    nextEmptyKey++;
    if (nextEmptyKey >= KEYS_CACHE_SIZE) {
        nextEmptyKey = CACHE_START_INDEX;
    }

    /* log cache */
    // Serial.println("cached keys");
    // for (uint8_t i = 0; i < KEYS_CACHE_SIZE; i++) {
        
    //     Serial.print(keys[i]);
    //     Serial.println(" ");
    // }
}
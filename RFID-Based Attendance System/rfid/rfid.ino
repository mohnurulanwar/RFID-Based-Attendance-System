#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>

const char* ssid = "OPPO F9";
const char* password = "123456789";
const char* server = "link your server";
const int httpPort = 80; 

const String secretKey = "your secretKey";
const String deviceId = "your deviceId";

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(2, 0);
WiFiClient client;
HTTPClient http;

const unsigned int buzzer = 15;
const int pushButton = 16;

void wifiConnection() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected!");
}

void setLcd() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(2, 0);
  lcd.print("APLIKASI WEB");
  lcd.setCursor(1, 1);
  lcd.print("PRESENSI RFID.");
  delay(2000);
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("SILAHKAN TEMPEL");
  lcd.setCursor(0, 1);
  lcd.print("   KARTU ANDA.");
}

void setup() {
  wifiConnection();
  setLcd();
  SPI.begin();
  rfid.PCD_Init();
  pinMode(pushButton, INPUT);
  pinMode(buzzer, OUTPUT);
}

void loop() {
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  String idTag = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    idTag += String(rfid.uid.uidByte[i], HEX);
  }
  Serial.println("RFID Detected: " + idTag);

  sendRFID(idTag);
  delay(2000);
}

void toneSuccess() {
  tone(buzzer, 2000);
  delay(500);
  noTone(buzzer);
}

void toneFailed() {
  tone(buzzer, 2000);
  delay(100);
  tone(buzzer, 1000);
  delay(100);
  tone(buzzer, 2000);
  delay(200);
  noTone(buzzer);
}

void buttonConnect()
{
    if(digitalRead(pushButton) == 0) {  //ditekan
    while(digitalRead(pushButton) == 0); //menahan proses sampai tombol dilepas
    if(!client.connect(server, httpPort)) {
      toneFailed();
      lcd.clear();
      lcd.setCursor(3, 0);
      lcd.print("CONNECTION");
      lcd.setCursor(5, 1);
      lcd.print("FAILED");
      Serial.println("connection failed");
      return;
    }

  }
}

void sendRFID(String rfid) {
  if (WiFi.status() == WL_CONNECTED) {
    String url = String(server) + "?secret_key=" + secretKey + "&device_id=" + deviceId + "&rfid=" + rfid;
    Serial.println("Requesting: " + url);

    http.begin(client, url);
    int httpCode = http.GET();
    String payload = http.getString();

    Serial.print("HTTP Response Code: ");
    Serial.println(httpCode);
    Serial.print("Server Response: ");
    Serial.println(payload);

      lcd.clear();
      if (payload == "RFID_REGISTERED") {
        lcd.setCursor(0, 0);
        lcd.print("KODE RFID SUKSES");
        lcd.setCursor(2, 1);
        lcd.print("DI DAFTARKAN");
        toneSuccess();
      } else if (payload == "PRESENCE_CLOCK_IN_SAVED") {
        lcd.setCursor(0, 0);
        lcd.print("CLOCK IN BERHASIL");
        lcd.setCursor(1, 1);
        lcd.print("ANDA MASUK.");
        toneSuccess();
      } else if (payload == "PRESENCE_CLOCK_OUT_SAVED") {
        lcd.setCursor(0, 0);
        lcd.print("CLOCK OUT BERHASIL");
        lcd.setCursor(1, 1);
        lcd.print("ANDA KELUAR.");
        toneSuccess();
      } else if(payload == "SECRET_KEY_NOT_FOUND") {
        lcd.setCursor(3, 0);
        lcd.print("SECRET-KEY");
        lcd.setCursor(2, 1);
        lcd.print("TIDAK SESUAI");
        toneFailed();
      } else if(payload == "DEVICE_NOT_FOUND") {
        lcd.setCursor(3, 0);
        lcd.print("DEVICE-ID");
        lcd.setCursor(2, 1);
        lcd.print("TIDAK SESUAI");
        toneFailed();
      } else if(payload == "RFID_NOT_FOUND") {
        lcd.setCursor(3, 0);
        lcd.print("RFID ANDA");
        lcd.setCursor(0, 1);
        lcd.print("TIDAK TERDAFTAR.");
        toneFailed();
      } else {
        lcd.setCursor(0, 0);
        lcd.print(" SELAMAT ABSENSI  ");
        lcd.setCursor(0, 1);
        lcd.print("    BERHASIL");
        toneSuccess();
      }

    delay(2000); // Tunggu 2 detik sebelum kembali ke tampilan awal
    setLcd();    // Kembali menampilkan "Silakan Tempel Kartu RFID Anda"

    http.end();
  } else {
    Serial.println("WiFi not connected!");
    toneFailed();
  }
}



#include <Arduino.h>
#include <WiFi.h>
#include <TeleBot.h>

// Wifi network station credentials
#define WIFI_SSID "realme C25s"
#define WIFI_PASSWORD "code787898"

void setup() {
  Serial.begin(115200);
  Serial.println();
  pinMode(19, OUTPUT);
  digitalWrite(19, LOW);

  // attempt to connect to Wifi network:
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  secured_client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Retrieving time: ");
  configTime(0, 0, "pool.ntp.org");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);
  pinMode(echo, INPUT);
  pinMode(trigh, OUTPUT);
  SetupServo();
}

void loop() {
  //Get_status();
  HandleBot();
  delay(100);
}
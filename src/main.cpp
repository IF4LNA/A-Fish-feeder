#include <Arduino.h>
#include <WiFi.h>
#include <TeleBot.h>

// Wifi network station credentials
#define WIFI_SSID "IFALNA"
#define WIFI_PASSWORD "Beruang123"

void handleNewMessages(int numNewMessages){
  for (int i = 0; i < numNewMessages; i++){
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;

    String from_name = bot.messages[i].from_name;
    if (from_name == "")
      from_name = "Guest";

    if ((chat_id == CHATID) or (chat_id == CHATID2)){
      if (text == "/makan"){
        bot.sendChatAction(chat_id, "typing");
        bot.sendMessage(chat_id, "Memberi Makan");
        Makan();
        bot.sendMessage(chat_id, "Sudah Diberi makan");
      }

      if (text == "/menu"){
        String welcome = "Selamat datang Di Fish Feeder Automatic, " + from_name + ".\n";
        welcome += "/makan : Untuk memberi makan\n";
        welcome += "/cek : Untuk memberi makan\n";
        bot.sendMessage(chat_id, welcome);
      }
      
      if (text == "/cek"){
        bot.sendMessage(chat_id, "Pakan anda tersisa : " + String(int(persentase)) + "%");
      }
  }
  }
}

void SetupServo(){
	// Setup Servo
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(50);
	myservo.attach(servoPin, 0, 4000);
  myservo.write(0);
}
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
  Get_status();
   if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
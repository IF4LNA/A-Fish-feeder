#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ESP32Servo.h>
#include <WaktuMakan.h>
#include <cstdint>
#include <string>

Servo myservo;
#define servoPin 18
int8_t last[2];

#define echo 14
#define trigh 12

// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "6525571173:AAF3ZNtvxFwuo293fzKryjCEZXV6mmOnm7c"
#define CHATID "6762920775"//"1348547429"
#define CHATID2 "6762920775"//"6909375322"

const unsigned long BOT_MTBS = 100; // mean time between scan messages

WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);
unsigned long bot_lasttime;
bool Start = false;

long timer;
float persentase;
// last notif berfungsi untuk metimpan waktu terakhir notif sisa makanan
int jarak, last_notif = 0;
#define max 13 //max tinggi tanki

void Get_status(){
  time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

  digitalWrite(trigh, LOW);                   
  delayMicroseconds(2);
  digitalWrite(trigh, HIGH);                  
  delayMicroseconds(10);
  digitalWrite(trigh, LOW);                   

  timer = pulseIn(echo, HIGH);
  jarak = timer/58;
  persentase = (max-jarak);
  persentase /= max;
  persentase *= 100;
  delay(100);
  Serial.print("Jarak = ");
  Serial.print(jarak);
  Serial.print(" cm : ");
  Serial.print(persentase);
  Serial.print("%\n");
  if(persentase <= 20){
    if(last_notif == timeinfo.tm_min){
      bot.sendMessage(CHATID, "❗Pakan anda tersisa : " + String(int(persentase)) + "%");
      //bot.sendMessage(CHATID2, "❗Pakan anda tersisa : " + String(int(persentase)) + "%");
      digitalWrite(19, HIGH);
    }
    if (last_notif == 59){
      last_notif = 1;
    }else{
      last_notif = timeinfo.tm_min + 1;
    }
  }else{
    digitalWrite(19, LOW);
  }
}

void SetupServo(){
  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(3);
	myservo.setPeriodHertz(60);    // standard 60 hz servo
	myservo.attach(18, 500, 2400);
  myservo.write(0);
}

void Makan(){
  myservo.write(180);
  delay(2000);
  myservo.write(0);
}

void infoJadwal(String ID){
  bot.sendMessage(ID,
    "Jadwal Makan:\nJadwal 1 : " + JadwalMakan[0] + "\n" +
    "Jadwal 2 : " + JadwalMakan[1]
  );
}
void handleNewMessages(int numNewMessages){
  for (int i = 0; i < numNewMessages; i++){
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
	  String from_name = bot.messages[i].from_name;

	if((chat_id == CHATID) or (chat_id == CHATID2)){
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
      /*
      Pakan tersisa
      Jadwal
      */
        bot.sendMessage(chat_id, "Pakan anda tersisa : " + String(int(persentase)) + "%");
        infoJadwal(chat_id);
      }

    if(text.substring(0,2) == "J1"){
      text += ":00";
      if(UpdateWaktuMakan(0, text.substring(2).c_str())){
        bot.sendMessage(chat_id, "Waktu 1 berhasil di Atur");
        infoJadwal(chat_id);
      }else{
        bot.sendMessage(chat_id, "Format waktu salah");
      }
    }

    if(text.substring(0,2) == "J2"){
      text += ":00";
      if(UpdateWaktuMakan(1, text.substring(2).c_str())){
        bot.sendMessage(chat_id, "Waktu 2 berhasil di Atur");
        infoJadwal(chat_id);
      }else{
        bot.sendMessage(chat_id, "Format waktu salah");
      }
    }
    
    }
  }
}

String timeOn, timeSec;
void CekWaktuMakan(){
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    timeOn = (
            String((timeinfo.tm_hour < 10 ? "0" : "")) + String(timeinfo.tm_hour) + ":"
          + String((timeinfo.tm_min < 10 ? "0" : "")) + String(timeinfo.tm_min) + ":"
          + String((timeinfo.tm_sec < 10 ? "0" : "")) + String(timeinfo.tm_sec)
          );
    now += 5;
    localtime_r(&now, &timeinfo);
    timeSec = (
            String((timeinfo.tm_hour < 10 ? "0" : "")) + String(timeinfo.tm_hour) + ":"
          + String((timeinfo.tm_min < 10 ? "0" : "")) + String(timeinfo.tm_min) + ":"
          + String((timeinfo.tm_sec < 10 ? "0" : "")) + String(timeinfo.tm_sec)
          );

    delay(500);

    if(
      timeOn < JadwalMakan[0]&&
      JadwalMakan[0] < timeSec
    ){
        bot.sendMessage(CHATID, "Memberi Makan");
        Makan();
        bot.sendMessage(CHATID, "Sudah Diberi makan");
    }

    if(
      timeOn < JadwalMakan[1]&&
      JadwalMakan[1] < timeSec
    ){
        bot.sendMessage(CHATID, "Memberi Makan");
        Makan();
        bot.sendMessage(CHATID, "Sudah Diberi makan");
    }
    
}

void HandleBot(){
  if (millis() - bot_lasttime > BOT_MTBS){
      int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      while (numNewMessages){
          Serial.println("got response");
          handleNewMessages(numNewMessages);
          numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      }

      bot_lasttime = millis();
  }
  CekWaktuMakan();
  Get_status();
}
#include <time.h>

String JadwalMakan[2] = {"00:00:00", "00:00:00"};

void SetupMakan(){
    configTime(7 * 3600, 0, "id.pool.ntp.org");
    while (!time(nullptr)) {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("Waktu berhasil diatur!");
}

bool UpdateWaktuMakan(int8_t idx, const char * waktu){
    struct tm targetTime;
    memset(&targetTime, 0, sizeof(struct tm)); // Inisialisasi struktur tm dengan 0
    char* res = strptime(waktu, "%H:%M:%S", &targetTime);

    if (res == nullptr || *res != '\0') {
        return false;
    } else {
        JadwalMakan[idx] = 
            String((targetTime.tm_hour < 10 ? "0" : "")) + String(targetTime.tm_hour) + ":"
            + String((targetTime.tm_min < 10 ? "0" : "")) + String(targetTime.tm_min) + ":"
            + String((targetTime.tm_sec < 10 ? "0" : "")) + String(targetTime.tm_sec);
        return true;
    }
}
//code du receiver: // =================== RÉCEPTEUR ESP8266 ===================
// Reçoit la trame ESP-NOW et la transmet sur la liaison série (TX → Mega).

#include <ESP8266WiFi.h>
#include <espnow.h>

typedef struct {
  int  yAxis;
  bool turnLeft;
  bool turnRight;
} Message;

Message data;

void onDataReceive(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
  if (len == sizeof(data)) {
    memcpy(&data, incomingData, sizeof(data));
    // Format envoyé à la Mega : "Y:xxx,L:0,R:1"
    Serial.print("Y:");
    Serial.print(data.yAxis);
    Serial.print(",L:");
    Serial.print(data.turnLeft ? 1 : 0);
    Serial.print(",R:");
    Serial.println(data.turnRight ? 1 : 0);
  }
}

void setup() {
  Serial.begin(9600);   // communication avec la Mega (broche TX de l'ESP)
  WiFi.mode(WIFI_STA);

  // Affiche l'adresse MAC pour la copier dans l'émetteur
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != 0) {
    Serial.println("Erreur ESP-NOW");
    return;;,;,  
  }
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(onDataReceive);
}

void loop() {
  delay(10);
}

//code du sender: // =================== ÉMETTEUR ESP8266 ===================
// Lit l'axe Y du joystick sur A0 et envoie la valeur via ESP-NOW.

#include <ESP8266WiFi.h>
#include <espnow.h>

// Adresse MAC du récepteur (à remplacer par la vôtre)
uint8_t receiverMAC[] = {0x08, 0x3A, 0x8D, 0xEF, 0x09, 0x5C};

#define LED_PIN      D0
#define BTN_LEFT_PIN D1   // bouton gauche (optionnel)
#define BTN_RIGHT_PIN D2  // bouton droit (optionnel)

typedef struct {
  int  yAxis;      // 0-1023, direction
  bool turnLeft;   // true si bouton gauche pressé
  bool turnRight;  // true si bouton droit pressé
} Message;

Message data;

void onDataSent(uint8_t *mac, uint8_t status) {
  digitalWrite(LED_PIN, status == 0 ? HIGH : LOW);
}

void setup()  {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BTN_LEFT_PIN, INPUT_PULLUP);
  pinMode(BTN_RIGHT_PIN, INPUT_PULLUP);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  if (esp_now_init() != 0) {
    Serial.println("Erreur init ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_add_peer(receiverMAC, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  esp_now_register_send_cb(onDataSent);
}

void loop() {
  data.yAxis = analogRead(A0);
  data.turnLeft  = (digitalRead(BTN_LEFT_PIN)  == LOW);
  data.turnRight = (digitalRead(BTN_RIGHT_PIN) == LOW);

  esp_now_send(receiverMAC, (uint8_t *) &data, sizeof(data));
  delay(50);
}

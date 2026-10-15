/*
  ARDUINO MEGA – PILOTAGE COMPLET AVEC PWM
  Rotations fonctionnelles (chaque côté tourne en bloc).
*/

// ==================== BROCHES L298N ====================
// Côté gauche
const int IN1_GAUCHE = 22;   // moteur avant gauche
const int IN2_GAUCHE = 23;
const int IN3_GAUCHE = 24;   // moteur arrière gauche
const int IN4_GAUCHE = 25;
// Côté droit
const int IN1_DROIT  = 26;   // moteur avant droit
const int IN2_DROIT  = 27;
const int IN3_DROIT  = 28;   // moteur arrière droit
const int IN4_DROIT  = 29;

// PWM Enable
const int ENA_GAUCHE = 10;   // pour IN1/IN2 gauche
const int ENB_GAUCHE = 11;   // pour IN3/IN4 gauche
const int ENA_DROIT  = 12;   // pour IN1/IN2 droit
const int ENB_DROIT  = 13;   // pour IN3/IN4 droit

// ==================== SEUILS JOYSTICK ====================
const int Y_CENTRE      = 512;
const int Y_ZONE_MORTE  = 60;
const int Y_SEUIL_AVANT   = Y_CENTRE + Y_ZONE_MORTE;
const int Y_SEUIL_ARRIERE = Y_CENTRE - Y_ZONE_MORTE;

// ==================== VITESSE ====================
const int VITESSE_MIN = 110;   // à ajuster selon vos moteurs
const int VITESSE_MAX = 155;
const int VITESSE_ROTATION = 180;

// ==================== FONCTIONS MOTEURS ====================
void setVitesse(int pwm) {
  pwm = constrain(pwm, 0, 255);
  analogWrite(ENA_GAUCHE, pwm);
  analogWrite(ENB_GAUCHE, pwm);
  analogWrite(ENA_DROIT,  pwm);
  analogWrite(ENB_DROIT,  pwm);
}

// --- Avant / Arrière ---
void forward() {
  // Avant gauche
  digitalWrite(IN1_GAUCHE, HIGH); digitalWrite(IN2_GAUCHE, LOW);
  // Arrière gauche
  digitalWrite(IN3_GAUCHE, HIGH); digitalWrite(IN4_GAUCHE, LOW);
  // Avant droit
  digitalWrite(IN1_DROIT,  HIGH); digitalWrite(IN2_DROIT,  LOW);
  // Arrière droit
  digitalWrite(IN3_DROIT,  HIGH); digitalWrite(IN4_DROIT,  LOW);
  Serial.println("🟢 FORWARD");
}

void backward() {
  digitalWrite(IN1_GAUCHE, LOW);  digitalWrite(IN2_GAUCHE, HIGH);
  digitalWrite(IN3_GAUCHE, LOW);  digitalWrite(IN4_GAUCHE, HIGH);
  digitalWrite(IN1_DROIT,  LOW);  digitalWrite(IN2_DROIT,  HIGH);
  digitalWrite(IN3_DROIT,  LOW);  digitalWrite(IN4_DROIT,  HIGH);
  Serial.println("🔴 BACKWARD");
}

// --- Rotations (les deux moteurs du même côté tournent dans le même sens) ---
void rotateLeft() {
  // Gauche recule
  digitalWrite(IN1_GAUCHE, LOW);  digitalWrite(IN2_GAUCHE, HIGH);
  digitalWrite(IN3_GAUCHE, LOW);  digitalWrite(IN4_GAUCHE, HIGH);
  // Droite avance
  digitalWrite(IN1_DROIT,  HIGH); digitalWrite(IN2_DROIT,  LOW);
  digitalWrite(IN3_DROIT,  HIGH); digitalWrite(IN4_DROIT,  LOW);
  Serial.println("🔄 ROTATE LEFT");
}

void rotateRight() {
  // Gauche avance
  digitalWrite(IN1_GAUCHE, HIGH); digitalWrite(IN2_GAUCHE, LOW);
  digitalWrite(IN3_GAUCHE, HIGH); digitalWrite(IN4_GAUCHE, LOW);
  // Droite recule
  digitalWrite(IN1_DROIT,  LOW);  digitalWrite(IN2_DROIT,  HIGH);
  digitalWrite(IN3_DROIT,  LOW);  digitalWrite(IN4_DROIT,  HIGH);
  Serial.println("🔄 ROTATE RIGHT");
}

void stopMotors() {
  digitalWrite(IN1_GAUCHE, LOW); digitalWrite(IN2_GAUCHE, LOW);
  digitalWrite(IN3_GAUCHE, LOW); digitalWrite(IN4_GAUCHE, LOW);
  digitalWrite(IN1_DROIT,  LOW); digitalWrite(IN2_DROIT,  LOW);
  digitalWrite(IN3_DROIT,  LOW); digitalWrite(IN4_DROIT,  LOW);
  setVitesse(0);
  Serial.println("⏹️ STOP");
}

// ==================== VARIABLES ET SETUP ====================
const unsigned long TIMEOUT_MS = 500;
unsigned long dernierReception = 0;
bool trameRecue = false;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(IN1_GAUCHE, OUTPUT); pinMode(IN2_GAUCHE, OUTPUT);
  pinMode(IN3_GAUCHE, OUTPUT); pinMode(IN4_GAUCHE, OUTPUT);
  pinMode(IN1_DROIT,  OUTPUT); pinMode(IN2_DROIT,  OUTPUT);
  pinMode(IN3_DROIT,  OUTPUT); pinMode(IN4_DROIT,  OUTPUT);
  pinMode(ENA_GAUCHE, OUTPUT); pinMode(ENB_GAUCHE, OUTPUT);
  pinMode(ENA_DROIT,  OUTPUT); pinMode(ENB_DROIT,  OUTPUT);

  stopMotors();
  Serial.println("Mega prêt - attente des commandes ESP...");
}

// ==================== LOOP ====================
void loop() {
  if (Serial1.available()) {
    String ligne = Serial1.readStringUntil('\n');
    ligne.trim();
    Serial.print("📨 Reçu : ");
    Serial.println(ligne);

    int idxY = ligne.indexOf("Y:");
    int idxL = ligne.indexOf(",L:");
    int idxR = ligne.indexOf(",R:");
    if (idxY != -1 && idxL != -1 && idxR != -1) {
      int y = ligne.substring(idxY + 2, idxL).toInt();
      int l = ligne.substring(idxL + 3, idxR).toInt();
      int r = ligne.substring(idxR + 3).toInt();

      Serial.print("   Y="); Serial.print(y);
      Serial.print(" L="); Serial.print(l);
      Serial.print(" R="); Serial.println(r);

      dernierReception = millis();
      trameRecue = true;

      if (l == 1) {
        setVitesse(VITESSE_ROTATION);
        rotateLeft();
      } else if (r == 1) {
        setVitesse(VITESSE_ROTATION);
        rotateRight();
      } else if (y > Y_SEUIL_AVANT) {
        int pwm = map(y, Y_SEUIL_AVANT, 1023, VITESSE_MIN, VITESSE_MAX);
        setVitesse(pwm);
        forward();
      } else if (y < Y_SEUIL_ARRIERE) {
        int pwm = map(y, Y_SEUIL_ARRIERE, 0, VITESSE_MIN, VITESSE_MAX);
        setVitesse(pwm);
        backward();
      } else {
        stopMotors();
      }
    }
  }

  if (trameRecue && (millis() - dernierReception > TIMEOUT_MS)) {
    stopMotors();
    trameRecue = false;
    Serial.println("⚠️ TIMEOUT - arrêt sécurité");
  }
  delay(10);
}

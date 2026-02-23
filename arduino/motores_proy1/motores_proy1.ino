int IN1 = 8;
int IN2 = 9;
int ENA = 10;
int IN3 = 6;
int IN4 = 7;
int ENB = 5;

// ====== ULTRASONICO ======
const int TRIG_PIN = 3;
const int ECHO_PIN = 2;
const int DISTANCIA_MINIMA = 20;  // cm — ajusta según necesites
bool obstaculoDetectado = false;

int velMotor = 150;
String inputBuffer = "";

long medirDistancia() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duracion = pulseIn(ECHO_PIN, HIGH, 30000); // timeout 30ms (~5m)
  if (duracion == 0) return 999; // sin eco = camino libre
  return duracion * 0.0343 / 2;
}

void chequearObstaculo() {
  long distancia = medirDistancia();

  if (distancia <= DISTANCIA_MINIMA) {
    if (!obstaculoDetectado) {
      obstaculoDetectado = true;
      stopMotors();
    }
  } else {
    if (obstaculoDetectado) {
      obstaculoDetectado = false;
    }
  }
}

// ====== MOVIMIENTOS ======
void driveForward() {
  if (obstaculoDetectado) return;  // <-- override
  Serial.println("WHITE -> FORWARD");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, velMotor);
  analogWrite(ENB, velMotor);
}
void driveBackward() {
  Serial.println("RED -> BACKWARD");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, velMotor);
  analogWrite(ENB, velMotor);
}
void turnRight() {
  if (obstaculoDetectado) return;  // <-- override
  Serial.println("GREEN -> RIGHT");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, velMotor);
  analogWrite(ENB, velMotor);
}
void turnLeft() {
  if (obstaculoDetectado) return;  // <-- override
  Serial.println("BLUE -> LEFT");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, velMotor);
  analogWrite(ENB, velMotor);
}
void stopMotors() {
  Serial.println("STOP");
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ====== COMANDOS ======
void handleLine(String line) {
  if (obstaculoDetectado) {
    return;
  }
  line.trim();
  line.toUpperCase();
  if (line.length() == 0) return;
  if      (line == "RED") driveForward();
  else if (line == "WHITE")   driveBackward();
  else if (line == "GREEN") turnRight();
  else if (line == "BLUE")  turnLeft();
  else if (line == "STOP")  stopMotors();
  else                      stopMotors();
}

void readSerialLines() {
  while (Serial.available() > 0) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (inputBuffer.length() > 0) {
        handleLine(inputBuffer);
        inputBuffer = "";
      }
    } else {
      inputBuffer += c;
      if (inputBuffer.length() > 64) inputBuffer = "";
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT); pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT); pinMode(ENB, OUTPUT);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  stopMotors();
}

void loop() {
  chequearObstaculo();  // siempre primero
  readSerialLines();
}

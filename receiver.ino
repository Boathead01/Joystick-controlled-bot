#include <esp_now.h>
#include <WiFi.h>

// Right motor
int enableRightMotor = 22; 
int rightMotorPin1 = 27;
int rightMotorPin2 = 14;
// Left motor
int enableLeftMotor = 23;
int leftMotorPin1 = 18;
int leftMotorPin2 = 19;

#define MAX_MOTOR_SPEED 250
const int PWMFreq = 1000; 
const int PWMResolution = 8;

#define SIGNAL_TIMEOUT 1000  
unsigned long lastRecvTime = 0;

struct PacketData {
  byte xAxisValue;
  byte yAxisValue;
  byte switchPressed;
};
PacketData receiverData;

bool throttleAndSteeringMode = false;

// Updated callback for Library 3.3.8 with Serial Feedback
void OnDataRecv(const esp_now_recv_info *info, const uint8_t *incomingData, int len) {
  if (len == 0) return;
  
  memcpy(&receiverData, incomingData, sizeof(receiverData));

  // --- SERIAL MONITOR FEEDBACK ---
  Serial.print("Received from: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", info->src_addr[i]);
    if (i < 5) Serial.print(":");
  }
  
  Serial.print(" | X: "); Serial.print(receiverData.xAxisValue);
  Serial.print(" | Y: "); Serial.print(receiverData.yAxisValue);
  Serial.print(" | Button: "); Serial.println(receiverData.switchPressed ? "ON" : "OFF");
  // -------------------------------

  if (receiverData.switchPressed == true) {
    throttleAndSteeringMode = !throttleAndSteeringMode;
  }

  if (throttleAndSteeringMode) throttleAndSteeringMovements();
  else simpleMovements();
  
  lastRecvTime = millis();   
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  if (rightMotorSpeed < 0) {
    digitalWrite(rightMotorPin1, LOW); digitalWrite(rightMotorPin2, HIGH);    
  } else if (rightMotorSpeed > 0) {
    digitalWrite(rightMotorPin1, HIGH); digitalWrite(rightMotorPin2, LOW);      
  } else {
    digitalWrite(rightMotorPin1, LOW); digitalWrite(rightMotorPin2, LOW);      
  }
  
  if (leftMotorSpeed < 0) {
    digitalWrite(leftMotorPin1, LOW); digitalWrite(leftMotorPin2, HIGH);    
  } else if (leftMotorSpeed > 0) {
    digitalWrite(leftMotorPin1, HIGH); digitalWrite(leftMotorPin2, LOW);      
  } else {
    digitalWrite(leftMotorPin1, LOW); digitalWrite(leftMotorPin2, LOW);      
  } 

  ledcWrite(enableRightMotor, abs(rightMotorSpeed));
  ledcWrite(enableLeftMotor, abs(leftMotorSpeed));    
}

void simpleMovements() {
  if (receiverData.yAxisValue <= 75) rotateMotor(MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
  else if (receiverData.yAxisValue >= 175) rotateMotor(-MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED);
  else if (receiverData.xAxisValue >= 175) rotateMotor(-MAX_MOTOR_SPEED, MAX_MOTOR_SPEED);
  else if (receiverData.xAxisValue <= 75) rotateMotor(MAX_MOTOR_SPEED, -MAX_MOTOR_SPEED);
  else rotateMotor(0, 0);
}

void throttleAndSteeringMovements() {
  int throttle = map(receiverData.yAxisValue, 254, 0, -255, 255);
  int steering = map(receiverData.xAxisValue, 0, 254, -255, 255);  
  int motorDirection = (throttle < 0) ? -1 : 1;
  
  int rightMotorSpeed = constrain(abs(throttle) - steering, 0, 255);
  int leftMotorSpeed = constrain(abs(throttle) + steering, 0, 255);

  rotateMotor(rightMotorSpeed * motorDirection, leftMotorSpeed * motorDirection);
}

void setUpPinModes() {
  pinMode(enableRightMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  // New PWM setup for Library 3.x
  ledcAttach(enableRightMotor, PWMFreq, PWMResolution);
  ledcAttach(enableLeftMotor, PWMFreq, PWMResolution);
  
  rotateMotor(0, 0);
}

void setup() {
  setUpPinModes();
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  Serial.println("Receiver Ready. Waiting for data...");
}
 
void loop() {
  unsigned long now = millis();
  if (now - lastRecvTime > SIGNAL_TIMEOUT) {
    rotateMotor(0, 0); // Safety stop if signal lost
  }
}

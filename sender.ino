#include <esp_now.h>
#include <WiFi.h>

#define X_AXIS_PIN 33
#define Y_AXIS_PIN 32
#define SWITCH_PIN 25

// receiver's MAC Address
uint8_t broadcastAddress[] = {0x1C, 0xC3, 0xAB, 0xA0, 0x56, 0xF4}; 

struct PacketData{
  byte xAxisValue;
  byte yAxisValue;
  byte switchPressed;
};
PacketData data;

int mapAndAdjustJoystickDeadBandValues(int value, bool reverse){
  if (value >= 2200){
    value = map(value, 2200, 4095, 127, 254);
  }
  else if (value <= 1800){
    value = map(value, 1800, 0, 127, 0);  
  }
  else{
    value = 127;
  }
  if (reverse){
    value = 254 - value;
  }
  return value;
}

// Fixed for library 3.3.8
void OnDataSent(const wifi_tx_info_t *tx_info, esp_now_send_status_t status){
  Serial.print("\r\nLast Packet Send Status:\t ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Message sent" : "Message failed");
}

void setup(){
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK){
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  else{
    Serial.println("Succes: Initialized ESP-NOW");
  }

  esp_now_register_send_cb(OnDataSent);
  
  esp_now_peer_info_t peerInfo = {}; // Added {} for safety
  memcpy(peerInfo.peer_addr, broadcastAddress, 6); // Changed to broadcastAddress
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
  else{
    Serial.println("Succes: Added peer");
  } 

  pinMode(SWITCH_PIN, INPUT_PULLUP);   
}
 
void loop(){
  data.xAxisValue = mapAndAdjustJoystickDeadBandValues(analogRead(X_AXIS_PIN), false);
  data.yAxisValue = mapAndAdjustJoystickDeadBandValues(analogRead(Y_AXIS_PIN), false);  
  data.switchPressed = false; 

  if (digitalRead(SWITCH_PIN) == LOW){
    data.switchPressed = true;
  }
  
  // Changed to broadcastAddress
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &data, sizeof(data));
  
  if (result == ESP_OK){
    Serial.println("Sent with success");
  }
  else{
    Serial.println("Error sending the data");
  }    
  
  if (data.switchPressed == true){
    delay(500);
  }
  else{
    delay(50);
  }
}

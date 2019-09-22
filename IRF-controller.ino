#include <WiFiNINA.h>
#include <IRLibAll.h>
#include <WiFiUdp.h>
#include "secrets.h"
#include <RCSwitch.h>

#define PORT 835
#define MAX_MSG 26
#define BCAST_PORT 836
#define BCAST_SIZE 30

char ssid[] = WIFI_NAME;
char passwd[] = WIFI_PASS;
char dev[] = DEVICE_NAME;

bool conected;

WiFiServer server(PORT);
IRsend sender;
WiFiUDP bcast;
RCSwitch rf = RCSwitch();

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  //while(!Serial);
  Serial.println("<<<POWERUP INITIATED>>>");
  if(WiFi.status() == WL_NO_MODULE){
    Serial.println("No WiFi card found");
    while(true);
  }
  digitalWrite(LED_BUILTIN, HIGH);
  WiFi.begin(ssid, passwd);
  delay(10000);
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Can't connect to WiFi. Check name and password");
    while(true);
  }
  Serial.println("WiFi connected");
  Serial.print("Listening to ");
  Serial.println(WiFi.localIP());
  server.begin();
  conected = false;
  bcast.begin(BCAST_PORT);
  rf.enableTransmit(4);
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() {
  WiFiClient client = server.available();
  uint8_t vers = 0x00, op_code = 0x00, current = 0x00;
  uint8_t message[MAX_MSG];
  char bcast_msg[BCAST_SIZE];
  int index = 0, operation = 0;
  bool done = false, prev_done = false;
  uint32_t nec_code = 0x00000000;
  bzero(message, sizeof(message));
  if(client){
    if(!conected) {
      Serial.println("Client connected");
      conected = true;
    }
    while(client.connected()){
      if(client.available()){
        current = client.read();
        Serial.print(current);
        if(!done)
        if(current == 0x26){
          operation++;
        } else {
          if(operation == 0) vers = current;
          if(operation == 1) op_code = current;
          if(operation == 2 && index < MAX_MSG) {
            message[index] = current;
            if(message[index] == 0x00) {
              if(prev_done) done = true;
              prev_done = true;
            }
            index++;
          }
        }
      }
      if(done){
        Serial.println("\nI'm done");
        if(vers > 0x31) {
          client.println("505&Version not supported");
        } else {
          if(vers == 0x00 || op_code == 0x00){
            client.println("400&Bad request");
          } else {
            if(op_code == 0x31){
              char result[40];
              sprintf(result, "%x", (uint32_t)(message[0]<<24|message[1]<<16|message[2]<<8|message[3]));
              Serial.println(result);
              sender.send(NEC, (uint32_t)(message[0]<<24|message[1]<<16|message[2]<<8|message[3]), 32);
              client.println("200&Seems legit");
            } else {
              if(op_code == 0x01){
                char result[24];
                memset(result, 0x00, sizeof(result));
                for(int i = 0; i < 24; i++){
                  result[i] = (char)message[i];
                }
                Serial.println(result);
                rf.send(result);
                /*delay(100);
                rf.send(result);
                delay(100);
                rf.send(result);*/
                client.println("200&Seems legit");
              }
            }
          }
        }
        done = false;
        client.stop();
      }
      if(!client.available() && !done){
        client.println("400&Bad request");
        client.stop();
      }
    }
  }
  bzero(bcast_msg, sizeof(bcast_msg));
  int bcast_len = 0;
  bcast_len = bcast.parsePacket();
  if(bcast_len){
    Serial.println("Broadcast received");
    bcast.read(bcast_msg, BCAST_SIZE);
    char bcast_name[14];
    bzero(bcast_name, sizeof(bcast_name));
    if(bcast_msg[0] == 0x31 && bcast_msg[2] == 0xFF){
      Serial.println("Protocol accepted");
      for(int i = 0; i < 14; i++) bcast_name[i] = (char)bcast_msg[i+4];
      Serial.println(bcast_name);
      Serial.println(strcmp(bcast_name, dev));
      if(strcmp(bcast_name, dev) == 0){
        delay(1000);
        Serial.println("Sending message");
        bcast.beginPacket(bcast.remoteIP(), BCAST_PORT);
        bcast.write(bcast_msg);
        bcast.endPacket();
      }
    }
  }
}

/*void readSerial(char *result){
  char current;
  while(!Serial.available());
  for(int i = 0; i < MAX_WIFI && Serial.available(); i++){
    current = Serial.read();
    result[i] = current;
    if(current == '\n'){
      result[i] = '\0';
      break;
    }
  }
}*/

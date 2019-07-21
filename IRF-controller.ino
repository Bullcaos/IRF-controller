#include <WiFiNINA.h>
#include <IRLibAll.h>

#define PORT 835
#define MAX_WIFI 20
#define MAX_MSG 6

char ssid[MAX_WIFI];
char passwd[MAX_WIFI];

bool conected;

WiFiServer server(PORT);
IRsend sender;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  Serial.println("<<<POWERUP INITIATED>>>");
  if(WiFi.status() == WL_NO_MODULE){
    Serial.println("No WiFi card found");
    while(true);
  }
  while(WiFi.status() != WL_CONNECTED){
    Serial.print("Enter SSID: ");
    readSerial(ssid);
    Serial.print("\n");
    Serial.print("Enter password: ");
    readSerial(passwd);
    Serial.print("\n");
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, passwd);
    delay(10000);
  }
  Serial.print("WiFi connected");
  Serial.print("Listening to ");
  Serial.println(WiFi.localIP());
  server.begin();
  conected = false;
}

void loop() {
  WiFiClient client = server.available();
  uint8_t vers = 0x00, op_code = 0x00, current;
  uint8_t message[MAX_MSG];
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
              Serial.println(result);c
              sender.send(NEC, (uint32_t)(message[0]<<24|message[1]<<16|message[2]<<8|message[3]), 32);
              client.println("200&Seems legit");
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
}

void readSerial(char *result){
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
}

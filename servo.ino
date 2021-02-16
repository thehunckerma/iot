#include <Servo_ESP32.h>
#include <WiFi.h>
#include <PubSubClient.h>

static const int servoPin = 14; //printed G14 on the board
const char* ssid = "bounouna";
const char* password = "bbbbbbbb";
const char* mqtt_server = "192.168.43.38";

WiFiClient espClient;
PubSubClient client(espClient);
Servo_ESP32 servo1;
int state=1;


int angle =0;
int angleStep = 5;

int angleMin =0;
int angleMax = 180;
int angleStart = (angleMin+angleMax)/2;

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void setup() {
    Serial.begin(115200);
    servo1.attach(servoPin);
    servo1.write(angleStart);
    setup_wifi();
    client.setServer(mqtt_server, 8883);
    client.setCallback(callback);
}


void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  if (String(topic) == "motorState") {
    state=messageTemp.toInt();
    if(state==0){
     servo1.write(angleMin);
    }else if(state==1){
     servo1.write(angleStart);
    }
  }
  if (String(topic) == "motorInput" && state) {
    angle = angle+ messageTemp.toInt();
    angle = (angle>angleMax?angleMax:(angle<angleMin?angleMin:angle));
    servo1.write(angle);
    char charAngle[16];
    itoa(angle, charAngle, 10);
    client.publish("motor",charAngle);
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("motorState");
      client.subscribe("motorInput");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
    if (!client.connected()) {
    reconnect();
    }
    client.loop();
}

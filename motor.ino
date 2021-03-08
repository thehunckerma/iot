#include <ArduinoWebsockets.h>
#include <WiFi.h>
#include <Servo_ESP32.h>

const char *ssid = "bounouna";
const char *password = "bbbbbbbb";
const char *websockets_server = "ws://192.168.43.38:8080/ws/motor";

// int angle = 0;
int prevAngle = 0;
int angleMin = 2;
int angleMax = 178;
int angleStart = (angleMin + angleMax) / 2;
int angle = angleStart;
static const int servoPin = 14; // G14 on the board

Servo_ESP32 servo1;

using namespace websockets;

WebsocketsClient client;

void onMessageCallback(WebsocketsMessage message)
{
    prevAngle = angle;
    String messageTemp = message.data();
    Serial.print("Instruction : ");
    Serial.print(messageTemp);

    Serial.println();

    angle = angle + messageTemp.toInt();
    angle = (angle > angleMax ? angleMax : (angle < angleMin ? angleMin : angle));
    if (prevAngle == angle)
    {
        Serial.println("Stopped");
    }
    else
    {
        servo1.write(angle);
    }
    Serial.print("Angle : ");
    Serial.println(angle);

    // char charAngle[16];
    // itoa(angle, charAngle, 10);
    // client.publish("motor", charAngle);
}

void onEventsCallback(WebsocketsEvent event, String data)
{
    if (event == WebsocketsEvent::ConnectionOpened)
    {
        Serial.println("Connnection Opened");
    }
    else if (event == WebsocketsEvent::ConnectionClosed)
    {
        Serial.println("Connnection Closed");
    }
    else if (event == WebsocketsEvent::GotPing)
    {
        Serial.println("Got a Ping!");
    }
    else if (event == WebsocketsEvent::GotPong)
    {
        Serial.println("Got a Pong!");
    }
}

void setup()
{
    Serial.begin(115200);
    servo1.attach(servoPin);
    servo1.write(angleStart);

    // Connect to wifi
    WiFi.begin(ssid, password);
    delay(1000);
    WiFi.disconnect();
    delay(1000);
    WiFi.begin(ssid, password);
    delay(1000);
    // Wait some time to connect to wifi
    Serial.println("Waiting for Wifi");
    for (int i = 0; i < 10 && WiFi.status() != WL_CONNECTED; i++)
    {
        Serial.print(".");
        delay(1000);
    }

    Serial.println("Connected to Wifi");
}

void loop()
{
    if (!client.available())
    {
        // Setup Callbacks
        client.onMessage(onMessageCallback);
        client.onEvent(onEventsCallback);

        // Connect to server
        client.connect(websockets_server);

        // Send a message
        Serial.println("Sending hi..");
        client.send("Hi Server!");
        // Send a ping
        client.ping();
    }
    else
    {
        client.poll();
    }
}
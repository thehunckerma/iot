/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp32-cam-post-image-photo-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <Arduino.h>
#include <WiFi.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_camera.h"
#include <ArduinoWebsockets.h>

const char *ssid = "bounouna";
const char *password = "bbbbbbbb";
const char *websockets_server = "ws://192.168.43.38:8080/ws/camera";

using namespace websockets;

WebsocketsClient client;

// CAMERA_MODEL_AI_THINKER
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27

#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

const int timerInterval = 3000;   // time between each HTTP POST image
unsigned long previousMillis = 0; // last time image was sent

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    Serial.begin(115200);

    WiFi.mode(WIFI_STA);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("ESP32-CAM IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println(1);

    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    Serial.println(2);
    // init with high specs to pre-allocate larger buffers
    if (psramFound())
    {
        Serial.println(3);
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 10; //0-63 lower number means higher quality
        config.fb_count = 2;
    }
    else
    {
        Serial.println(4);
        config.frame_size = FRAMESIZE_CIF;
        config.jpeg_quality = 12; //0-63 lower number means higher quality
        config.fb_count = 1;
    }
    Serial.println(5);
    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        delay(1000);
        ESP.restart();
    }
    Serial.println(6);
    // Connect to server
    client.connect(websockets_server);
    Serial.println(7);
}

void loop()
{
    sendPhoto();
}

String sendPhoto()
{
    Serial.println(8);
    camera_fb_t *fb = NULL;
    fb = esp_camera_fb_get();
    if (!fb)
    {
        Serial.println("Camera capture failed");
        delay(1000);
        ESP.restart();
    }
    Serial.println(9);
    char *fbBuf = (char *)fb->buf;
    // uint8_t *fbBuf = fb->buf;
    size_t fbLen = fb->len;
    Serial.println(11);

    for (size_t n = 0; n < fbLen; n = n + 1024)
    {
        if (n + 1024 < fbLen)
        {
            client.sendBinary(fbBuf, 1024);
            fbBuf += 1024;
        }
        else if (fbLen % 1024 > 0)
        {
            size_t remainder = fbLen % 1024;
            client.sendBinary(fbBuf, remainder);
        }
    }

    esp_camera_fb_return(fb);
    Serial.println(12);
}
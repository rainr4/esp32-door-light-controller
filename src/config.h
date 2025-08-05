#ifndef CONFIG_H
#define CONFIG_H

// WiFi Configuration - UPDATE THESE!
#define WIFI_SSID "YOUR_WIFI_NETWORK_NAME"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// OTA Configuration - UPDATE THESE!
#define OTA_PASSWORD "YOUR_OTA_UPDATE_PASSWORD" 
#define OTA_HOSTNAME "esp32-door"

// Hardware Pin Configuration (adjust if needed)
#define LED_PIN 27
#define DEFAULT_NUM_LEDS 40
#define TRIG_PIN 12
#define ECHO_PIN 14

// Default Settings (adjust as needed)
#define DEFAULT_THRESHOLD_INCHES 2.0
#define DEFAULT_R 0
#define DEFAULT_G 0
#define DEFAULT_B 0
#define DEFAULT_W 255

#endif

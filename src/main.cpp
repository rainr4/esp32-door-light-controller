#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoOTA.h>
#include "config.h"

#define LED_TYPE NEO_RGBW + NEO_KHZ800

Adafruit_NeoPixel strip(DEFAULT_NUM_LEDS, LED_PIN, LED_TYPE);
AsyncWebServer server(80);

// WiFi credentials from config
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

uint16_t numLeds = DEFAULT_NUM_LEDS;
uint8_t r = DEFAULT_R, g = DEFAULT_G, b = DEFAULT_B, w = DEFAULT_W;
float openThresholdInches = DEFAULT_THRESHOLD_INCHES;

void setAllLeds(uint8_t r, uint8_t g, uint8_t b, uint8_t w) {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    // Swap R and G for your SK6812 strip - send G,R,B,W instead of R,G,B,W
    strip.setPixelColor(i, strip.Color(g, r, b, w));
  }
  strip.show();
}

void turnOffLeds() {
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(0, 0, 0, 0));
  }
  strip.show();
}

float getDistanceInches() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // timeout after 30ms
  float distance_cm = duration * 0.0343 / 2;
  float distance_in = distance_cm * 0.3937;
  return distance_in;
}

void setup() {
  Serial.begin(115200);
  strip.begin();
  strip.show();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  ArduinoOTA.setHostname(OTA_HOSTNAME);
  ArduinoOTA.setPassword(OTA_PASSWORD);
  ArduinoOTA
    .onStart([]() {
      Serial.println("OTA start");
      turnOffLeds(); // Optionally turn off LEDs during OTA
    })
    .onEnd([]() {
      Serial.println("\nOTA end");
    })
    .onProgress([](unsigned int p, unsigned int t) {
      Serial.printf("OTA progress: %u%%\r", (p * 100) / t);
    })
    .onError([](ota_error_t err) {
      Serial.printf("OTA Error[%u]: ", err);
      if      (err == OTA_AUTH_ERROR)   Serial.println("Auth Failed");
      else if (err == OTA_BEGIN_ERROR)  Serial.println("Begin Failed");
      else if (err == OTA_CONNECT_ERROR)Serial.println("Connect Failed");
      else if (err == OTA_RECEIVE_ERROR)Serial.println("Receive Failed");
      else if (err == OTA_END_ERROR)    Serial.println("End Failed");
    });
  ArduinoOTA.begin();

  // Serve the web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<html><head><title>RGB Door Control</title>"
      "<meta name='viewport' content='width=device-width,initial-scale=1'>"
      "<style>body{font-family:sans-serif;background:#181c24;color:#eee;margin:0;padding:0;}"
      ".container{max-width:420px;margin:40px auto;background:#23283a;padding:2em 2em 1em 2em;border-radius:16px;box-shadow:0 2px 16px #0006;}"
      "h2{margin-top:0;}"
      ".status{font-size:1.2em;font-weight:bold;margin:1em 0;}"
      ".open{color:#fff;background:#27ae60;padding:0.5em 1em;border-radius:8px;display:inline-block;}"
      ".closed{color:#fff;background:#e74c3c;padding:0.5em 1em;border-radius:8px;display:inline-block;}"
      ".slider-label{display:inline-block;width:2em;}"
      ".slider{width:200px;}"
      "form input[type='number']{width:60px;padding:0.2em;margin:0.2em 0.5em 0.2em 0;background:#23283a;color:#eee;border:1px solid #444;border-radius:6px;}"
      "form input[type='submit']{background:#2980b9;color:#fff;border:none;padding:0.5em 1.5em;border-radius:8px;font-size:1em;cursor:pointer;}"
      "form input[type='submit']:hover{background:#3498db;}"
      ".slider-value{display:inline-block;width:2.5em;text-align:right;margin-left:0.5em;}"
      "</style>"
      "</head><body><div class='container'>";
    html += "<h2>Door Sensor Control</h2>";
    html += "<div class='status'>Door Status: <span id='door'></span></div>";
    html += "<div>Current Distance: <b id='dist'></b></div>";
    html += "<form action='/set' method='get'>";
    html += "<div>Num LEDs: <input type='number' name='num' min='1' max='300' value='" + String(numLeds) + "'></div>";
    // Fixed slider assignments - R slider controls 'r', G slider controls 'g'
    html += "<div><span class='slider-label'>R</span><input class='slider' type='range' name='r' min='0' max='255' value='" + String(r) + "' oninput=\"syncSlider('r',this.value)\"><span class='slider-value' id='rVal'>"+String(r)+"</span></div>";
    html += "<div><span class='slider-label'>G</span><input class='slider' type='range' name='g' min='0' max='255' value='" + String(g) + "' oninput=\"syncSlider('g',this.value)\"><span class='slider-value' id='gVal'>"+String(g)+"</span></div>";
    html += "<div><span class='slider-label'>B</span><input class='slider' type='range' name='b' min='0' max='255' value='" + String(b) + "' oninput=\"syncSlider('b',this.value)\"><span class='slider-value' id='bVal'>"+String(b)+"</span></div>";
    html += "<div><span class='slider-label'>W</span><input class='slider' type='range' name='w' min='0' max='255' value='" + String(w) + "' oninput=\"syncSlider('w',this.value)\"><span class='slider-value' id='wVal'>"+String(w)+"</span></div>";
    html += "<div>Open Threshold (inches): <input type='number' step='0.1' name='thresh' min='1' max='40' value='" + String(openThresholdInches) + "'></div>";
    html += "<div><input type='submit' value='Set'></div>";
    html += "</form>";
    html += "<script defer>\n"
      "function updateStatus() {\n"
      "  console.log('→ fetching /status…');\n"
      "  fetch('/status')\n"
      "    .then(r => {\n"
      "      console.log('  response:', r.status);\n"
      "      return r.json();\n"
      "    })\n"
      "    .then(d => {\n"
      "      console.log('  payload:', d);\n"
      "      const doorElem = document.getElementById('door');\n"
      "      if (!doorElem) console.error('#door element not found!');\n"
      "      else doorElem.innerHTML = d.doorOpen\n"
      "        ? '<span class=\"open\">OPEN</span>'\n"
      "        : '<span class=\"closed\">CLOSED</span>';\n"
      "      document.getElementById('dist').innerText = d.distance.toFixed(2) + ' in';\n"
      "    })\n"
      "    .catch(err => console.error('updateStatus error:', err));\n"
      "}\n"
      "updateStatus();\n"
      "setInterval(updateStatus, 1000);\n"
      "function syncSlider(id,val){document.getElementById(id+'Val').innerText=val;}\n"
      "</script>";
    html += "</div></body></html>";
    request->send(200, "text/html", html);
  });

  server.on("/status", HTTP_GET, [](AsyncWebServerRequest *request){
    float currentDist = getDistanceInches();
    bool doorOpen = (currentDist > openThresholdInches) || (currentDist == 0.0f);
    String json = "{";
    json += "\"doorOpen\":" + String(doorOpen ? "true" : "false");
    json += ",\"distance\":" + String(currentDist, 2);
    json += ",\"r\":" + String(r);
    json += ",\"g\":" + String(g);
    json += ",\"b\":" + String(b);
    json += ",\"w\":" + String(w);
    json += ",\"numLeds\":" + String(numLeds);
    json += ",\"thresh\":" + String(openThresholdInches, 2);
    json += "}";
    request->send(200, "application/json", json);
  });

  // Fix the /set endpoint - remove the swapped assignments
  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request){
    if (request->hasParam("num")) numLeds = request->getParam("num")->value().toInt();
    if (request->hasParam("r")) r = request->getParam("r")->value().toInt();  // Fixed: r controls r
    if (request->hasParam("g")) g = request->getParam("g")->value().toInt();  // Fixed: g controls g
    if (request->hasParam("b")) b = request->getParam("b")->value().toInt();
    if (request->hasParam("w")) w = request->getParam("w")->value().toInt();
    if (request->hasParam("thresh")) openThresholdInches = request->getParam("thresh")->value().toFloat();
    strip.updateLength(numLeds);
    request->redirect("/");
  });

  server.begin();
  turnOffLeds();
}

void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Attempting to reconnect...");
    
    // Flash red while disconnected
    for (int i = 0; i < 3; i++) {
      setAllLeds(255, 0, 0, 0); // Now this will show actual red
      delay(200);
      turnOffLeds();
      delay(200);
    }
    
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
      // Continue flashing red during reconnection attempts
      setAllLeds(255, 0, 0, 0); // Red
      delay(250);
      turnOffLeds();
      delay(250);
      Serial.print(".");
      attempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi reconnected!");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      
      // Flash green to indicate successful reconnection
      for (int i = 0; i < 2; i++) {
        setAllLeds(0, 255, 0, 0); // Green
        delay(300);
        turnOffLeds();
        delay(300);
      }
    } else {
      Serial.println("\nFailed to reconnect. Will try again later.");
      
      // Flash red rapidly to indicate failure
      for (int i = 0; i < 5; i++) {
        setAllLeds(255, 0, 0, 0); // Red
        delay(100);
        turnOffLeds();
        delay(100);
      }
      
      // Optional: Reset ESP32 after multiple failed attempts
      Serial.println("Restarting ESP32...");
      ESP.restart();
    }
  }
}

void loop() {
  ArduinoOTA.handle();

  // Check WiFi connection every 15 seconds
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 15000) {
    checkWiFiConnection();  
    lastWiFiCheck = millis();
  }

  // Rolling buffer for door state
  const int windowSize = 5;
  static bool stateBuffer[windowSize] = {false};
  static int bufferIndex = 0;
  static bool lightsOn = false;

  float dist = getDistanceInches();
  bool doorOpen = (dist > openThresholdInches) || (dist == 0.0f);

  // Update buffer
  stateBuffer[bufferIndex] = doorOpen;
  bufferIndex = (bufferIndex + 1) % windowSize;

  // Count how many "open" in buffer
  int openCount = 0;
  for (int i = 0; i < windowSize; i++) {
    if (stateBuffer[i]) openCount++;
  }

  // Only control door lights if WiFi is connected
  if (WiFi.status() == WL_CONNECTED) {
    if (openCount >= (windowSize / 2 + 1)) { // Majority open
      if (!lightsOn) {
        Serial.println("Lights ON (door open, filtered)");
        setAllLeds(r, g, b, w);
        lightsOn = true;
      }
    } else {
      if (lightsOn) {
        Serial.println("Lights OFF (door closed, filtered)");
        turnOffLeds();
        lightsOn = false;
      }
    }
  }

  delay(75); // Faster response, but still filtered
}

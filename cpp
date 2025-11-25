#include <WiFi.h>
#include <HTTPClient.h>

// TODO: change these
const char* WIFI_SSID     = "YOUR_WIFI_SSID";
const char* WIFI_PASSWORD = "YOUR_WIFI_PASSWORD";

// Example: "http://192.168.1.100:8000/telemetry"
const char* SERVER_URL = "http://YOUR_BACKEND_IP:8000/telemetry";

const char* DEVICE_ID = "ECOBOT_01";

unsigned long lastSendTime = 0;
const unsigned long SEND_INTERVAL_MS = 10000; // 10 seconds

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected! IP: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  connectToWiFi();
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    connectToWiFi();
  }

  unsigned long now = millis();
  if (now - lastSendTime >= SEND_INTERVAL_MS) {
    lastSendTime = now;

    // 🔧 TODO: replace with real sensor readings
    int dustLevel       = random(40, 120);  // PM2.5
    int vocLevel        = random(10, 80);   // VOC
    int batteryLevel    = random(30, 100);  // % battery
    float energyUsed    = random(5, 20) + 0.23; // Wh
    int recoveredBatteries = random(0, 3);  // count
    String batteryCondition = "Recycle";

    String status = "cleaning";    // or "idle", "charging"
    String sessionId = "S123";     // you can generate dynamic IDs

    // Build JSON payload
    String jsonPayload = "{";
    jsonPayload += "\"device_id\":\"" + String(DEVICE_ID) + "\",";
    jsonPayload += "\"status\":\"" + status + "\",";
    jsonPayload += "\"dust_level\":" + String(dustLevel) + ",";
    jsonPayload += "\"voc_level\":" + String(vocLevel) + ",";
    jsonPayload += "\"battery_level\":" + String(batteryLevel) + ",";
    jsonPayload += "\"energy_used\":" + String(energyUsed, 2) + ",";
    jsonPayload += "\"session_id\":\"" + sessionId + "\",";
    jsonPayload += "\"recovered_batteries\":" + String(recoveredBatteries) + ",";
    jsonPayload += "\"battery_condition\":\"" + batteryCondition + "\"";
    jsonPayload += "}";

    Serial.println("Sending JSON:");
    Serial.println(jsonPayload);

    HTTPClient http;
    http.begin(SERVER_URL);
    http.addHeader("Content-Type", "application/json");

    int httpCode = http.POST(jsonPayload);

    if (httpCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpCode);
      String response = http.getString();
      Serial.println(response);
    } else {
      Serial.printf("Error sending POST: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
}


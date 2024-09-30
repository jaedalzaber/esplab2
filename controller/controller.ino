#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include <ArduinoJson.h>

#define LED 2

using namespace websockets;

int motor1Pin1 = 25;
int motor1Pin2 = 26;

int motor2Pin1 = 27;
int motor2Pin2 = 9;

int enable1Pin = 5;
int enable2Pin = 2;

// int motor1Pin1 = 36;
// int motor1Pin2 = 39;

// int motor2Pin1 = 34;
// int motor2Pin2 = 35;

// int enable1Pin = 18;
// int enable2Pin = 23;

const int freq = 30000;
const int pwmChannel = 0;
const int resolution = 8;
int dutyCycle = 200;

String serverName = "ws://10.10.206.199:3000/";
String movePath = "move";

const char* ssid = "UIU-STUDENT";
// const char* password = "sadiq628";
const char* password = "12345678";

// Set your Static IP address
IPAddress local_IP(10,10,249,250);
// Set your Gateway IP address
IPAddress gateway(192, 168, 0, 1);

IPAddress subnet(255, 255, 0, 0);
IPAddress primaryDNS(8, 8, 8, 8);    // optional
IPAddress secondaryDNS(8, 8, 4, 4);  // optional

unsigned long previousMillis = 0;
unsigned long interval = 30000;

unsigned long http_lastTime = 0;
unsigned long http_timerDelay = 1000;

WebsocketsClient client;


void onMessageCallback(WebsocketsMessage message) {
  Serial.print("Got Message: ");
  Serial.println(message.data());
  //stop
  // calculate new path
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("Connnection Opened");
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("Connnection Closed");
  } else if (event == WebsocketsEvent::GotPing) {
    Serial.println("Got a Ping!");
  } else if (event == WebsocketsEvent::GotPong) {
    Serial.println("Got a Pong!");
  }
}

void initWifi() {
  Serial.begin(115200);
  delay(1000);
  //-----Wifi Initialization-------//
  //Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.mode(WIFI_STA);  //Optional
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());

 // -----Web Socket Initialization-------//
  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);
  client.connect(serverName);
  client.send("message");
  client.ping();
}

void setupPins() {
  // Set pin mode
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  pinMode(enable2Pin, OUTPUT);

  // ledcSetup(pwmChannel, freq, resolution);
  // ledcAttachPin(enable1Pin, pwmChannel);
  // ledcAttachPin(enable2Pin, pwmChannel);
  ledcAttachChannel(enable1Pin, freq, resolution, pwmChannel);
  ledcAttachChannel(enable2Pin, freq, resolution, pwmChannel);
}

void setup() {
  setupPins();

  // initWifi();
}

void moveController() {
  // Move the DC motor forward at maximum speed
  Serial.println("Moving Forward");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
  delay(2000);

  // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
  delay(1000);

  // Move DC motor backwards at maximum speed
  Serial.println("Moving Backwards");
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH);
  delay(2000);

  // Stop the DC motor
  Serial.println("Motor stopped");
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
  delay(1000);

  // Move DC motor forward with increasing speed
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);


  while (dutyCycle <= 155) {
    ledcWrite(enable1Pin, dutyCycle);
    ledcWrite(enable2Pin, dutyCycle);

    Serial.println(dutyCycle);
    dutyCycle = dutyCycle + 5;
    delay(500);
  }
  dutyCycle = 100;
}

void wifiReconnect() {
  unsigned long currentMillis = millis();
  // if WiFi is down, try reconnecting
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)) {
    Serial.print(millis());
    Serial.println("Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    previousMillis = currentMillis;
  }
}

void handleHttpRequest() {
  if ((millis() - http_lastTime) > http_timerDelay) {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      String serverPath = serverName + movePath;

      // Your Domain name with URL path or IP address with path
      http.begin(serverPath.c_str());

      // If you need Node-RED/server authentication, insert user and password below
      //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

      // Send HTTP GET request
      int httpResponseCode = http.GET();

      if (httpResponseCode > 0) {
        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);
        String payload = http.getString();
        JsonDocument doc;
        deserializeJson(doc, payload.c_str());
        Serial.println(payload);
        Serial.println(doc["direction"].as<const char*>());
      } else {
        Serial.print("Error code: ");
        Serial.println(httpResponseCode);
      }
      // Free resources
      http.end();
    } else {
      Serial.println("WiFi Disconnected");
    }
    http_lastTime = millis();
  }
}

void handleSocket() {
  client.poll();
}

void loop() {
  moveController();

  // handleSocket();
  // wifiReconnect();
}
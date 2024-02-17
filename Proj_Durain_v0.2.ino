#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include "FS.h"

SoftwareSerial RFID(D5, D6);  // RX and TX

const char* id = "000";
const char* ssid = "esp8266_WiFi";
const char* password = "12345678";
const char* mqtt_server = "mqtt.fakepng.dev";
const int mqtt_port = 39736;
const char* mqtt_Client = "";
const char* mqtt_username = "fakepng";
char* mqtt_password = "u2e7F8qA4aACjBvD";

WiFiClient espClient;
PubSubClient client(espClient);

ESP8266WebServer server(80);

char wifi_ssid[20];
char wifi_password[20];

int i = 0;
int statusCode;
String st;
String content;

long lastMsg = 0;
int value = 0;
char msg[100];
String DataString;
String text;
char c;

bool loadConfig() {
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    Serial.println("Failed to open config file:");
    return false;
  }
  size_t size = configFile.size();
  Serial.println("size ");
  Serial.println(size);
  if (size > 256) {
    Serial.println("Config file size is to large");
    return false;
  }
  //Allocate a buffer to store contents of the file.
  std::unique_ptr<char[]> buf(new char[size]);
  configFile.readBytes(buf.get(), size);
  Serial.println(buf.get());

  JsonDocument json;
  deserializeJson(json, buf.get());
  DeserializationError error = deserializeJson(json, buf.get());
  if (error) {
    Serial.println("Failed to parse config file");
    return false;
  }

  const char* json_ssid = json["json_ssid"];
  strncpy(wifi_ssid, json_ssid, 20);
  Serial.print("ssid Loaded: ");
  Serial.println(wifi_ssid);

  const char* json_paswd = json["json_paswd"];
  strncpy(wifi_password, json_paswd, 20);
  Serial.print("paswd Loaded: ");
  Serial.println(wifi_password);
  return true;
}

bool saveConfig() {
  JsonDocument json;

  json["json_ssid"] = wifi_ssid;
  json["json_paswd"] = wifi_password;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Failed to open config file for writing");
    return false;
  }
  serializeJson(json, configFile);
  return true;
}

void launchWeb() {
  Serial.println("");
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  createWebServer();
  server.begin();
  Serial.println("Server started");
}

void setupAP(void) {
  WiFi.mode(WIFI_STA);
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.print("\n");
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
  st = "<ol>";
  for (int i = 0; i < n; ++i) {
    // Print SSID and RSSI for each network found
    st += "<li>";
    st += WiFi.SSID(i);
    st += " (";
    st += WiFi.RSSI(i);

    st += ")";
    st += (WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*";
    st += "</li>";
  }
  st += "</ol>";
  delay(100);
  WiFi.softAP(ssid, password);
  launchWeb();
}

void createWebServer() {
  {
    server.on("/", []() {
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>ESP8266 Web Server";
      content += "<form action=\"/scan\" method=\"POST\"><input type=\"submit\" value=\"scan\"></form>";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID : </label><input name='ssid' length=20><label><br>PASSWORD : </label><input name='pass' length=20><input type='submit'></form>";
      content += "</html>";
      server.send(200, "text/html", content);
    });

    server.on("/scan", []() {
      //setupAP();
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);

      content = "<!DOCTYPE HTML>\r\n<html>go back";
      server.send(200, "text/html", content);
    });

    server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      int sid_str_len = qsid.length() + 1;
      char webssid[sid_str_len];
      qsid.toCharArray(webssid, sid_str_len);
      int pass_str_len = qpass.length() + 1;
      char webpass[pass_str_len];
      qpass.toCharArray(webpass, pass_str_len);

      if ((qsid.length() > 0) && (qpass.length() >= 8) && (qpass.length() <= 20)) {
        strncpy(wifi_ssid, webssid, 20);
        Serial.print("writing eeprom ssid:");
        Serial.println(wifi_ssid);

        strncpy(wifi_password, webpass, 20);
        Serial.print("writing eeprom pass:");
        Serial.println(wifi_password);
        Serial.print("\n");

        saveConfig();

        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
        ESP.restart();
      } else {
        content = "{\"Error\":\"404 not found\"}";
        content += "{\"password length must be of 8-15\"}";
        statusCode = 404;
        Serial.println("Sending 404");
        Serial.println("password length must be of 8-15");
      }
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(statusCode, "application/json", content);
    });
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("durian_1/post");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
      digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String message;
  for (int i = 0; i < length; i++) { 
    Serial.println(char(payload[i]));
  }
  if (String(topic) == "durian_1/get") {
    if (char(payload[0]) == '1') {
      digitalWrite(BUILTIN_LED, HIGH);
      Serial.println("LED ON");
    } else if(char(payload[0] == '0')){
      digitalWrite(BUILTIN_LED, LOW);
      Serial.println("LED OFF");
    }
  } 
}


void setup() {
  Serial.begin(9600);
  RFID.begin(9600);
  Serial.println();
  
  Serial.println("Mounting FS...");  
  if(!SPIFFS.begin()){
    Serial.println("Failed to mount file system");
    return;
  }  
  loadConfig();
  saveConfig();
    
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.print("\n"); 
  Serial.println("Turning the HotSpot On");
  launchWeb();
  setupAP();
 
  Serial.println();
  Serial.println("Waiting...");
  
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password); 
  while ((WiFi.status() != WL_CONNECTED))
  {
    delay(500);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    server.handleClient();
  }
  digitalWrite(LED_BUILTIN, 0);
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);
  client.subscribe("durian_1/post");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now;
    ++value;

    while (RFID.available() > 0) {
      delay(5);
      c = RFID.read();
      text += c;
      digitalWrite(LED_BUILTIN, 0);
    }
    digitalWrite(LED_BUILTIN, 1);
    
    if (text.length() > 20) {
      text = text.substring(1, 11);
      Serial.println("Card ID : " + text);
      //Format Json : {"data":{"id":"000","rfid":"0000000000"}}
      DataString = "{\"data\":{\"id\":\"" + (String)id + "\",\"rfid\":\"" + (String)text + "\"}}";
      DataString.toCharArray(msg, 100);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("durian_1/post", msg); 
      text = "";
    }
  }
}

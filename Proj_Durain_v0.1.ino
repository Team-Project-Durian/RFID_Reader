#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

SoftwareSerial RFID(D5, D6); // RX and TX

const char* ssid = "MIRABILIS";
const char* password = "Z{c30948";
const char* mqtt_server = "mqtt.fakepng.dev";
const int mqtt_port = 39736;
const char* mqtt_Client = "";
const char* mqtt_username = "fakepng"; 
char* mqtt_password = "u2e7F8qA4aACjBvD";  
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;
char msg[100];
String DataString;
String text;
char c;

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(mqtt_Client, mqtt_username, mqtt_password)) {  //เชื่อมต่อกับ MQTT BROKER
      Serial.println("connected");
      client.subscribe("durian_1/post");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("try again in 5 seconds");
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
  Serial.begin(115200);
  RFID.begin(9600);
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);  //เชื่อมต่อกับ WIFI
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());            //เชื่อมต่อกับ WIFI สำเร็จ แสดง IP
  client.setServer(mqtt_server, mqtt_port);  //กำหนด MQTT BROKER, PORT ที่ใช้
  client.setCallback(callback);              //ตั้งค่าฟังก์ชันที่จะทำงานเมื่อมีข้อมูลเข้ามาผ่านการ Subscribe
  client.subscribe("durian_1/post");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  long now = millis();
  if (now - lastMsg > 10000) {  //จับเวลาส่งข้อมูลทุก ๆ 5 วินาที
    lastMsg = now;
    ++value;

    while (RFID.available() > 0) {
      delay(5);
      c = RFID.read();
      text += c;
    }
    
    if (text.length() > 20) {
      text = text.substring(1, 11);
      Serial.println("Card ID : " + text);
      DataString = "{\"id\":" + (String)text + "}";
      DataString.toCharArray(msg, 100);
      Serial.print("Publish message: ");
      Serial.println(msg);
      client.publish("durian_1/post", msg); 
      text = "";
    }
  }
}
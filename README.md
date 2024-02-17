# RFID_Reader

## v.0.0
<p>funtion : Only read RFID.</p>

## v.0.1
<p>
function : <br>
- read RFID. <br>
- connect wifi. <br>
- sent data to node-red. <br>
</p>

## v.0.2
<p>
function : <br>
- read RFID. <br>
- connect wifi. <br>
- sent data to node-red. <br>
- can change connect wifi. <br>
</p>

## How to use (v.0.2).
<p>
Equipment
  - ESP8266
  - RDM6300
Library
  - ESP8266WiFi.h
  - ESP8266HTTPClient.h
  - ESP8266WebServer.h
  - PubSubClient.h
  - SoftwareSerial.h
  - ArduinoJson.h
Pin
  ESP8266 --> RDM6300
  Vin --> +5V(DC)
  GND --> GND
  D5(RX) --> TX
  D6(TX) --> RX
How to use
  Connect Wifi or change Wifi to connect to.
    1. กดปุ่ม Reset บน ESP8266
    2. เชื่อมต่อไวไฟ esp8266_wifi (password : 12345678)
    3. เปิดบราวเซอร์ใส่ ip address 192.168.4.1 ที่ช่องค้นหา
    4. ใส่ชื่อและรหัสไวไฟที่ต้องการเชื่อมต่อแล้วกดส่ง
  
</p>

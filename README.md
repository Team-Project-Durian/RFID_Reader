# RFID_Reader

## v.0.0
<p>funtion : Only read RFID.</p>

## v.0.1
<p>
*function : <br>
  * read RFID. <br>
  * connect wifi. <br>
  * sent data to node-red. <br>
</p>

## v.0.2
<p>
* function : <br>
  * read RFID. <br>
  *  connect wifi. <br>
  * sent data to node-red. <br>
  * can change connect wifi. <br>
</p>

## How to use (v.0.2).
### Equipment
<p>
  * ESP8266 <br>
  * RDM6300 <br>
</p>
### Library
<p>
  * ESP8266WiFi.h <br>
  * ESP8266HTTPClient.h <br>
  * ESP8266WebServer.h <br>
  * PubSubClient.h <br>
  * SoftwareSerial.h <br>
  * ArduinoJson.h <br> 
</p>
### Pin
<p>
  ESP8266 --> RDM6300 <br>
  Vin --> +5V(DC) <br>
  GND --> GND <br>
  D5(RX) --> TX <br>
  D6(TX) --> RX <br>
</p>
### How to use
<p>
 * **Connect Wifi or change Wifi to connect to.** <br>
    1. กดปุ่ม **Reset** บน **ESP8266** <br>
    2. เชื่อมต่อไวไฟ **esp8266_wifi** (password : **12345678**) <br>
    3. เปิดบราวเซอร์ใส่ ip address : **192.168.4.1** ที่ช่องค้นหา <br>
    4. ใส่ชื่อและรหัสไวไฟที่ต้องการเชื่อมต่อแล้วกดส่ง <br>
</p>

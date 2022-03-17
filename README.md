# ESP_8266 Temperature, Pressure & Humidity project
<p>
<img alt="GitHub" src="https://img.shields.io/github/license/birrozza/esp_8266_T-H">
<img alt="GitHub all releases" src="https://img.shields.io/github/downloads/birrozza/esp_8266_T-H/total">
<img alt="AUR last modified" src="https://img.shields.io/aur/last-modified/esp_8266_T-H">
</p>

![](/images/login_page.png)

<p>The BME280 module, thanks to its sensors, can detect the temperature, pressure and humidity of the surrounding environment, and send this data to the Wemos board which, thanks to its wifi module, can transmit them to a PC or smartphone.</p>

### Feautures
Thanks to code inside (sketch), the data is sent to a web platform (www.thingspeak.com) and displayed through graphics. Moreover, these and other data can be viewed on all network devices because the Wemos card works as a <strong>web-server </strong>([see below](#software-istruction)). It is possible to activate a **bot** on a **Telegram** with which to remotely interrogate the sensor ([see below](#Enable-a-Telegram-bot-to-be-able-to-interrogate-the-sensor)).


[This is my chanel were data are sent ](https://thingspeak.com/channels/867219).  The data are in real-time

### Dependency:
| Library | Source |
| ------ | ------ |
| ESP8266WiFi.h | ESP8266 Arduino Core (ver 3.0.2) |
| Ticker.h | ESP8266 Arduino Core (ver 3.0.2) |
| ESP8266mDNS.h | ESP8266 Arduino Core (ver 3.0.2) |
| FS.h| ESP8266 Arduino Core (ver 3.0.2) |
| ESPAsyncTCP.h | https://github.com/me-no-dev/ESPAsyncTCP |
| ESPAsyncWebServer.h | https://github.com/me-no-dev/ESPAsyncWebServer |
| ESPAsyncWiFiManager.h | WifiManager 2.0.3 alpha |
| AsyncTelegram.h | Ver. 1.1.3 |
| ArduinoJson.h | Ver 6.15.1 |
| ThingSpeak.h | Ver 2.0.1 |
| TimeLib.h | Ver 1.6.1 |
| ArduinoOTA.h | ....... |

<h3>Necessary material:</h3>

<blockquote>
<ul>
<li><p> <a href="https://www.banggood.com/Geekcreit-D1-mini-V2_2_0-WIFI-Internet-Development-Board-Based-ESP8266-4MB-FLASH-ESP-12S-Chip-p-1143874.html?p=MX1504307245201310VT&custlinkid=673886">Wemos D1 mini clone based on ESP8266 chip </a></p></li>
<li><p> <a href="https://it.aliexpress.com/item/1005002847432886.html?gatewayAdapt=glo2ita&spm=a2g0o.order_list.0.0.21ef3696OlhuTX">BME280 5v sensor (temperature, pressure & humidity)</a></p></li>
<li><p> <a href="https://www.thingiverse.com/thing:3965015">My printable case project on Thingiverse.com (optional)</a></p></li>
</ul>
</blockquote>

<h3>Software istruction</h3>

<p>First you need to register the service (free but with some limitations) here:  <a href="https://thingspeak.com">www.thingspeak.com</a>. 
<br> 
 So,  download the code for Arduino from this repository.
<br>
With the Arduino IDE, you must enter in the <b><em>secret.h</em></b> file and update the <em><strong>Channel ID</em></strong> and the <em><strong>Channel Write Key</strong></em> values whith obtained when the service was activated on Thingspeak.</p>
<b>Very important note</b>: to have the Arduino IDE compile the <b><em>secret.h</em></b> file you need to modify the sketch following the instructions in the comments.

![](/images/comment.png)

In this project, in addition to managing the BMP280 sensor, the Wemos card also acts as a <strong>web-server</strong>. For this purpose, <strong>web pages</strong> have been created which function as a user interface. These must be loaded into the SPIFFS of the ESP8266. Here the plug-in about it [ESP8266 Sketch Data Upload](https://github.com/esp8266/arduino-esp8266fs-plugin).

When your ESP starts up, it sets it up in Station mode and tries to connect to a previously saved Access Point.
If this is unsuccessful (or no previous network saved) it moves the ESP into Access Point mode and spins up a  WebServer (default ip <strong>192.168.4.1</strong>)
Using any wifi enabled device with a browser (computer, phone, tablet) connect to the newly created Access Point (<strong>ESP8266 AP Wifi</strong>).
Because of the Captive Portal and the DNS server you will either get a 'Join to network' type of popup or get any domain you try to access redirected to the configuration portal.
Choose one of the access points scanned, enter password, click save.
ESP will try to connect. If successful, it relinquishes control back to your app. If not, reconnect to AP and reconfigure.
[Here this tutorial](https://github.com/alanswx/ESPAsyncWiFiManager).

Once the wifi connection is made, the card is now reachable on our network. It is sufficient to type in the address bar of the browser the url <b>http://myesp.local</b> to be able to navigate between the sensor pages. The host name is the default but can be changed in the _config.json_ file ([see below](#configuration)).  

In the <b>landing</b> page the access data (<b>user</b> & <b>password</b>) are available as tip by passing the pointer over the fields. They are editable via the _config.json_ file.

On the <strong>setting page</strong> you can find different values, such as the <strong>IP address</strong> of the card, its <strong>ID name</strong>, the name of the <strong>wi-fi</strong> network and its power (<strong>RSSI</strong>), and other specific card data.
Below, in the <strong>File Manager</strong> section, you have the image of the <strong>SPIFFS</strong> and further down, you have the possibility to update the Html files as well as upload new ones.

<h3>Configuration</h3>

Through the <b>_config.json_</b> file it is possible to configure the program at run-time.
In fact, in the file you find the following fields and modifiable sub fields:

*  <b>"_board_"</b>:

    *  <b>"_board_id_"</b>: the identification name of the card;
    *  <b>"_type_"</b>: it must remain unchanged;
    *  <b>"_local_host_name_"</b>: the host name of the card to connect to with your browser;
    *  <b>"_rateo_"</b>: no longer in use.

*  <b>"_login_"</b>:    
    *  <b>"_user_"</b>: user;
    *  <b>"_password_"</b>: password.
    

*  <b>"_location_"</b>:    
    *  <b>"_city_"</b>: the name of the city where the sensor is installed;
    *  <b>"_country_"</b>: the name of the country where the sensor is installed.
    

*  <b>"_field_1_"</b> & <b>"_field_2_"</b>:    
    *  <b>"_name_"</b>: it must remain unchanged;
    *  <b>"_url_"</b>: Thingspeak channel fields URL.

The other fields are not currently used.

After modifying the file, you can upload it using the "_upload file_" function on the setting page. The changes will take effect immediately.


### Firmware OTA update (Over The Air)
You can now also update the firmware in the card via the **OTA (Over The Air)** feature. In fact, it is no longer necessary to connect it to the PC but through the Arduino IDE it can be updated remotely. Just select the network port in the menu: *tools -> port -> Network Ports*.

<h2>Enable a Telegram bot to be able to interrogate the sensor</h2>

![](/images/telegram.jpg)

First of all you need to create a bot on the Telegram with <b>BothFather</b> function. The procedure 
can be found at this <a href="https://core.telegram.org/bots#6-botfather">link</a>.

Obtained the token of our new bot, it must be inserted in the <b>_secret.h_</b> file.

Through the bot, you can interrogate the sensor with commands:
<ul>
    <li><b>"<em>read</em>"</b>:  returns the latest temperature and humidity reading;</li>
    <li><b>"<em>wifi</em>"</b>:  returns the parameters of the wifi connection;</li>
    <li><b>"<em>stato</em>"</b>: returns the last status of the data transmission to ThingSpeak;</li>
    <li><b>"<em>reset</em>"</b>: reboot the card.</li>
</ul> 
<br>

### Project version for DTH22 sensor
Of this project there is also the version for DHT22 sensors. However, only the temperature and humidity can be read with these sensors. you can find it in the repository: https://github.com/birrozza/esp_8266_T-H


<strong>Stay tuned!!! (last update March 2022)</strong>

If you want to support me, you can always donate to my paypal account https://www.paypal.me/birrozza. 

**Thanks**
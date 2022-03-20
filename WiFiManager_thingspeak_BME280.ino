/* ------------- MEMO -----------------
  - per ESP8266 clone WeMos D1 R1 usare LOLIN(WeMos) D1 R1
  - esp core 3.0.2 funzionante con telegram https://github.com/esp8266/Arduino
  - driver esp8266 http://www.wch.cn/download/CH341SER_ZIP.html
  - Markdown editor https://pandao.github.io/editor.md/en.html  o  https://stackedit.io/
*/
#include <NTPClient.h>          // https://github.com/arduino-libraries/NTPClient (v 3.2.0)
//#include <WiFiUdp.h>            // library ESP8266 Arduino Core (ver 3.0.2)
#include "ESP8266WiFi.h"        // library ESP8266 Arduino Core (ver 3.0.2)
#include "Ticker.h"             // library ESP8266 Arduino Core (ver 3.0.2)
#include "ESP8266mDNS.h"        // library ESP8266 Arduino Core (ver 3.0.2)
#include <FS.h>                 // library ESP8266 Arduino Core (ver 3.0.2)
#include "ESPAsyncTCP.h"        // https://github.com/me-no-dev/ESPAsyncTCP
#include "ESPAsyncWebServer.h"  // https://github.com/me-no-dev/ESPAsyncWebServer
#include "ESPAsyncWiFiManager.h"// WifiManager 2.0.3 alpha
#include "AsyncTelegram.h"      // ver 1.1.3
#include <ArduinoJson.h>        // ver 6.15.1
#include "ThingSpeak.h"         // ver 2.0.1
#include <TimeLib.h>            // ver 1.6.1
#include <ArduinoOTA.h>   
#include <math.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C       
#include "utility.h"
#include "C:\Users\computer\Desktop\esp8266 sketch\secret.h"   // always comment on this line (for development only) 
//#include "secret.h" // uncomment if it's commented out

#define GOOGLE_SHEET_ENABLED //video 529
#define TELEGRAM_BOT_ENABLED

//// variable
unsigned long  myChannelNumber = SECRET_CH_ID;        // ┌───────────┐
const char     * myWriteAPIKey = SECRET_WRITE_APIKEY; // │from secret.h file│
const char* token = SECRET_TELEGRAM_BOT_TOKEN;        // └───────────┘
WiFiClient     client;
String         boardName;
String         password;
String         user;
String         hostName="myesp";
AsyncWebServer server(80);
DNSServer      dns;
float lastTemperatureRead = 0.0;
float lastHumidityRead = 0.0;
float lastPressionRead = 0.0;
AsyncTelegram myBot;
String stato="*ESP start"; // stato corrente del sistema
int count=0;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org", 3600);

Ticker ticker; //for LED status

// function prototypes for HTTP handlers
void dirRequest (AsyncWebServerRequest *request);
void handleLogin(AsyncWebServerRequest *request);
void handleIpRequest(AsyncWebServerRequest *request);
void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final); 

//// metodi
void tick() { //toggle state
  int state = digitalRead(BUILTIN_LED);
  digitalWrite(BUILTIN_LED, !state);
}
 
//gets called when WiFiManager enters configuration mode
void configModeCallback (AsyncWiFiManager  *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);
}
 
void setup() {
  timeClient.begin();
  delay(1000);
  
  Serial.begin(57600);
  SPIFFS.begin();   // Start the SPI Flash Files System
  
  File txtFile = SPIFFS.open(F("/config.json"),"r"); // apri il file di configurazione
  String configuration = txtFile.readString();
  txtFile.close(); // chiudi il file
  Serial.println("\nConfiguration file uploaded!!!\n");
  
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, configuration);

  boardName = String(doc["board"]["board_id"]);
  hostName = String(doc["board"]["local_host_name"]);
  user = String(doc["login"]["user"]);
  password = String(doc["login"]["password"]);
    
  //myChannelNumber = (doc["thingspeak"]["id"]);
  //myWriteAPIKey = (doc["thingspeak"]["api_key"]);
  //const char* token = (doc["Telegram_bot"]["token"]);
    
  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  ticker.attach(0.6, tick); // start ticker with 0.5 because we start in AP mode and try to connect
 
  //WiFiManager
  //Local intialization. Once its business is done,
  //there is no need to keep it around
  WiFi.persistent(true);
  AsyncWiFiManager wifiManager(&server,&dns);
  //reset settings - for testing
  //wifiManager.resetSettings();
 
  //set callback that gets called when connecting to previous WiFi fails
  //and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);
 
  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("ESP8266 AP Wifi")) {// INSERISCI QUI IL NOME DELLA RETE COME ACCES POINT
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");
  
  ticker.detach();
  //keep LED on
  digitalWrite(BUILTIN_LED, LOW);
  Serial.println('\n');
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              // Tell us what network we're connected to
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           // Send the IP address of the ESP8266 to the computer
  //delay(500);
  //secured_client.setInsecure();
  // setup server services
  server.on("/", HTTP_GET,  [](AsyncWebServerRequest *request) {
     Serial.println("Serving index.html");
    request->send(SPIFFS, "/index.html", "text/html");
  });
  server.on("/login",     HTTP_POST, handleLogin); // Call the 'handleLogin' function when a POST request is made to URI "/login" 
  server.on("/login",     HTTP_GET , [](AsyncWebServerRequest *request) {
     Serial.println("serving login.html");
    request->send(SPIFFS, "/login.html", "text/html");
  });
  /*
  server.on("/iprequest", HTTP_POST, [](AsyncWebServerRequest *request) {
    String ip="IP "+WiFi.localIP().toString();
    Serial.println(ip);
    request->send(200, "text/html", ip);
  });
  */
  server.on("/iprequest", HTTP_GET, handleIpRequest);
  server.on("/setpage", HTTP_GET, [](AsyncWebServerRequest *request) {
    Serial.println("Serving setpage.html");
    request->send(SPIFFS, "/setting.html","text/html");   
  });
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) { // chiedi il reset
    request->send(SPIFFS, "/index.html", "text/html");  
    ESP.restart(); // avvio completo della scheda
  });
  server.on("/directory", HTTP_GET,  dirRequest);
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request){
    request->send(200);
  },handleFileUpload);
  
  server.onNotFound([](AsyncWebServerRequest *request){
    Serial.println("in onNotFound");
    request->send(SPIFFS, "/sitonontrovato.html", "text/html");
    //request->send(404, "text/html", sitoNonTrovato);  
  });
   
  server.begin();                           // Actually start the server
  Serial.println("HTTP server started");

  // modify TTL associated  with the domain name (in seconds)
  // default is 60 seconds
  //dnsServer.setTTL(300);
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is DNSReplyCode::NonExistentDomain
  //dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
/*
  // start DNS server for a specific domain name
  if(!dnsServer.start(DNS_PORT, "angelo", WiFi.localIP())){
    Serial.println("dnsserver fallito");
  }
    Serial.println("dnsserver partito");
*/  

  if (!MDNS.begin(hostName, WiFi.localIP(),3600)) {             // Start the mDNS responder for myesp.local
        Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");
  // Add service to MDNS-SD
  MDNS.addService("http", "tcp", 80);
  WiFi.mode(WIFI_STA);

  ThingSpeak.begin(client);
  
  
  // Set the Telegram bot properies
  myBot.setClock("CET-1CEST-2,M3.5.0/02:00:00,M10.5.0/03:00:00");  //CET-1CEST,M3.5.0,M10.5.0/3
  myBot.setUpdateTime(2000);
  myBot.setTelegramToken(token);
    
  // Check if all things are ok
  Serial.print("\nTest Telegram connection... ");
  if (myBot.begin()){
    Serial.println("Telegram OK");
    
  } else { 
    Serial.println("Telegram NO OK");
  }

  // Prepare OTA handler
  ArduinoOTA.onStart([]() {
    Serial.println("Start OTA");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd OTA");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });

  ArduinoOTA.setHostname((const char *)hostName.c_str());
  ArduinoOTA.begin();

  // default settings
  //unsigned status = bme.begin();  
    // You can also pass in a Wire library object like &Wire2
    // status = bme.begin(0x76, &Wire2)
  //Wire.setSpeed(CLOCK_SPEED_400KHZ);
  if (!bme.begin(0x76)) {
        Serial.println("Could not find a valid BME280 sensor, check wiring, address, sensor ID!");
        Serial.print("SensorID was: 0x"); Serial.println(bme.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
  } 
  Serial.println("BME280 Sensor ok...");
    // weather monitoring
  Serial.println("-- Weather Station Scenario --");
  Serial.println("forced mode, 1x temperature / 1x humidity / 1x pressure oversampling,");
  Serial.println("filter off");
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                    Adafruit_BME280::SAMPLING_X1, // temperature
                    Adafruit_BME280::SAMPLING_X1, // pressure
                    Adafruit_BME280::SAMPLING_X1, // humidity
                    Adafruit_BME280::FILTER_OFF,   
                    Adafruit_BME280::STANDBY_MS_1000);  
  timeClient.update();
  delay(1000);

} // end setup

int seTimeOk = 0;

unsigned long t1;
ADC_MODE(ADC_VCC);
void loop() {
 /*
  if ((millis()-t1)>2000){
    Serial.println("-controllo messaggi: "+String(millis()-t1));
    Serial.println(ESP.getVcc()/1000.00);
    t1=millis();
  }
 */ 
  MDNS.update();
  ArduinoOTA.handle();
  
  TBMessage msg;
  if (timeStatus()!=timeSet) { //set time only first time
    setTime(timeClient.getEpochTime());
    Serial.println("set time for first time!!!");
    seTimeOk++;
  }
    
  if (WiFi.status() != WL_CONNECTED) ESP.reset(); // verifica lo stato della  connessione
  
  if((((minute()-1) % 2 == 0)) && (second()==5)){ // ogni 2 min leggo i dati dal sensore 
    // Only needed in forced mode! In normal mode, you can remove the next line.
    bme.takeForcedMeasurement(); // has no effect in normal mode  
    delay(50);
    lastTemperatureRead=bme.readTemperature();
    delay(50);
    lastPressionRead=bme.readPressure() / 100.0F;
    delay(50);
    lastHumidityRead=abs(bme.readHumidity()*100)/100; 
    Serial.println("Temp= "+String(lastTemperatureRead)+"°C,  Hum= "+String(lastHumidityRead)+"%, Press= "+String(lastPressionRead)+"hPa");
    delay(1000);
  }    
  if(((minute()==0) || (minute()== 30)) && (second()==0)){ // dopo altri due minuti invia la lettura
    // se l'invio è prima di una lettura allora imposta 15.99 di default la temperatura
    ThingSpeak.setField(1, (lastTemperatureRead == 0.0? float(15.99) : lastTemperatureRead));
    // se l'invio è prima di una lettura allora imposta 50.99 di default l'umidità
    ThingSpeak.setField(2, (lastHumidityRead == 0.0? float(50.99) : lastHumidityRead));
    ThingSpeak.setField(3, (lastPressionRead == 0.0? float(SEALEVELPRESSURE_HPA) : lastPressionRead));
    ThingSpeak.setStatus(stato);
    int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
    if(x == 200){
      Serial.println("Channel update successful.");
      stato="[Data updated ("+String(x)+")]"; // azzera lo stato
      delay(1100); //attendo un secondo per uscire da questa condizione
    } else {
      Serial.println("Problem updating channel. HTTP error  " + msgFeedBack(x)); 
      stato=stato+"* Problem updating. Err. Code " + msgFeedBack(x)+" *"; 
      setTime(now()-240); // se ci sono problemi riporta indietro il tempo di 4 min e ricomincia daccapo
    }
  }
  if (minute()>=30) {
    count=60-minute();
  }else count=30-minute() ;
  
  //telegram step
  if (myBot.getNewMessage(msg)) { //se è presente un messaggio
    String replay="";
    if (msg.text.equalsIgnoreCase("read")) {
      replay = "Ciao " + String(msg.sender.firstName) + "!!!\nThe last data read:";
      replay += "\nLast temperature: " + String(lastTemperatureRead) + "°C";
      replay += "\nLast humidity: " + String(lastHumidityRead) + "%";
      replay += "\nLast pressure: " + String(lastPressionRead) + "hPa";
      double s = lastTemperatureRead - 37.25*(2 - log10(double(lastHumidityRead))); // punto di rugiada
      replay += "\nDev point: " + String(s); 
      //myBot.sendMessage(msg, replay );
    } // read
    else if (msg.text.equalsIgnoreCase("wifi")){
      replay = "SSID: "+WiFi.SSID()+"\nRSSI: " +WiFi.RSSI()+"\nIP: " + WiFi.localIP().toString()+"\nLocal host: "+hostName+"\nBoard: "+boardName;
      //myBot.sendMessage(msg, replay);
    } // wifi
    else if (msg.text.equalsIgnoreCase("stato")){
      replay = "Stato: " + stato;
      replay += "\nCount: -"+String(count)+" min";
      //myBot.sendMessage(msg, replay);
    } // stato
    else if (msg.text.equalsIgnoreCase("reset")){
      replay = "Tra 5 secondi la scheda si riavvia" ;
      myBot.sendMessage(msg, replay);
      delay(5000);
      ESP.reset();
    } // reset
    else {
      replay = "Ciao!!! \nChoices available:\n- read -> temp & Humidity;\n- wifi -> wifi status;\n- stato -> stato;\n- reset -> to reset card.";
      //myBot.sendMessage(msg, replay);
    }
    myBot.sendMessage(msg, replay);
  } // end telegram bot
  
} // end loop

///////// WEB SERVER metod ///////////////////

void handleIpRequest(AsyncWebServerRequest *request){ // invia json con ip, city, country e url_fields
  
  StaticJsonDocument <1024> rispostaJson; //creazione del json di risposta
  DynamicJsonDocument doc(1024); // creazione del contenitore per il json di configurazione
  String json;
  // carica il file config.json
  File txtFile = SPIFFS.open(F("/config.json"),"r"); // apri il file config.json dove c'è la configurazione
  String configuration = txtFile.readString(); //metti nella stringa tutto il testo scaricato
  txtFile.close(); // chiudi il file
  
  deserializeJson(doc, configuration); // da stringa scaricata a json
  
  rispostaJson["ip"] = WiFi.localIP().toString();
  rispostaJson["city"] = doc["location"]["city"];
  rispostaJson["country"] = doc["location"]["country"];
  rispostaJson["temp"] = doc["thingspeak"]["field_1"]["url"];
  rispostaJson["humi"] = doc["thingspeak"]["field_2"]["url"];
  
  serializeJson(rispostaJson, json); // da json a stringa per l'invio
  //Serial.print('json: ' + json);
  request->send(200, "text/json", json); //invia json
}

void handleLogin(AsyncWebServerRequest *request) {                         // If a POST request is made to URI /login
  Serial.println("in handlelogin");
  if( !request->hasArg("username") || !request->hasArg("password") // verifica se li argomenti esistono
      || request->arg("username") == NULL || request->arg("password") == NULL) { // If the POST request doesn't have username and password data
    //request->send(400, "text/html", invalidRequest);         
    request->send(SPIFFS, "/invalidrequest.html", "text/html"); // The request is invalid, so send HTTP status 400 
    return;
  }
  if(request->arg("username") == user && request->arg("password") == password) { // If both the username and the password are correct
      request->send(SPIFFS, "/login.html");
  } else {                                                                              // Username and password don't match
      //request->send(401, "text/html", unauthorized);// unauthorized = stringa in formato HTML (vedi testiHTML.h)
      request->send(SPIFFS, "/nonautorizzato.html", "text/html");
  }
}

void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if(!index){
    if(!filename.startsWith("/")) filename = "/"+filename;
    Serial.println((String)"UploadStart: " + filename);
    // open the file on first call and store the file handle in the request object
    request->_tempFile = SPIFFS.open(filename, "w");
  }
  if(len) {
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data,len);
  }
  if(final){
    Serial.println((String)"UploadEnd: " + filename + "," + index+len);
    // close the file handle as the upload is now done
    request->_tempFile.close();
    // prima di rimandare alla pagina setpage
    Serial.println(filename);
    if(filename=="config.json"){ //se il file caricato è quello di configurazione
        Serial.println("in");
        //** carico in un json il file di configurazione **//
        DynamicJsonDocument fileConfig(1024); // creazione del contenitore per il json di configurazione
        File txtFile = SPIFFS.open(F("/config.json"),"r"); // apri il file config.json dove c'è la configurazione
        String configuration = txtFile.readString(); //metti nella stringa tutto il testo scaricato
        txtFile.close(); // chiudi il file
        deserializeJson(fileConfig, configuration); // da stringa scaricata a json
        //aggiorno i parametri sensibili
        boardName = String(fileConfig["board"]["board_id"]);
        String newHostName = String(fileConfig["board"]["local_host_name"]);
        //rateo = float(fileConfig["board"]["rateo"]);
        user = String(fileConfig["login"]["user"]);
        password = String(fileConfig["login"]["password"]);
        if (newHostName!=hostName){
          hostName=newHostName;
          MDNS.end();
          if (!MDNS.begin(hostName, WiFi.localIP(),3600)) {             // Start the mDNS responder for myesp.local
            Serial.println("Error setting up MDNS responder!");
          }  
        }  
    }    
    request->redirect("/setpage");
  } 
}

void dirRequest (AsyncWebServerRequest *request){
    // per la verifica del json>>>>    https://filosophy.org/code/fixing-syntaxerror-unexpected-string-token-in-json-at-position/ 
    // oppure                  >>>>    http://jsonviewer.stack.hu/
    // oppure                  >>>>    https://arduinojson.org/v6/assistant/
    String json;
    
    //creo il json da inviare
    StaticJsonDocument <2048> doc; /// per creare il json di risposta
    JsonArray dir_json = doc.createNestedArray("dir");
    
    Dir dir = SPIFFS.openDir("/");
    int x = 0;
    while (dir.next()) {
        // get filename
        JsonArray dir_file = dir_json[x++].createNestedArray("file");
        dir_file.add(String(dir.fileName()));
        // If element have a size display It else write 0
        if(dir.fileSize()) {
            File f = dir.openFile("r");
            dir_file.add(String(f.size()));
            f.close();
        }else{
            dir_file.add("0");            
        }
    }
    doc["ip"] = WiFi.localIP().toString();
    /// inseriamo nel json i datitecnici della scheda
    FSInfo fs_info;
    SPIFFS.info(fs_info);
    JsonArray infosys = doc.createNestedArray("infosys");
    infosys.add(String("Total Space:     " + String(fs_info.totalBytes)    + " byte"));
    infosys.add(String("Space Used:      " + String(fs_info.usedBytes)     + " byte"));
    //infosys.add(String("Block Size:      " + String(fs_info.blockSize)     + " byte"));
    //infosys.add(String("Page Size:       " + String(fs_info.totalBytes)    + " byte"));
    //infosys.add(String("Max open files:  " + String(fs_info.maxOpenFiles)  + " files"));
    infosys.add(String("Comp. Date:      " + String(__DATE__)));
    infosys.add(String("Comp. Time:      " + String(__TIME__)));
    infosys.add(String("Voltage:         " + String(ESP.getVcc()/1000.00)+"Volt"));
    if (count!=1){  //se non è all'ultimo minuto conta i minuti
      infosys.add(String("Count down:      -" + String(count) + " min"));
    } else{ // se è all'ultimo minuto conta i secondi 
      infosys.add(String("Count down:      -" + String(60-second()) + " sec"));
    }
    infosys.add(String("Last stato:      " + stato + ""));
    doc["RSSI"] = String(WiFi.RSSI());
    doc["SSID"] = String(WiFi.SSID());
    doc["Board_Name"] = boardName;
    doc["Temperature"] = String(lastTemperatureRead);
    doc["Humidity"] = String(lastHumidityRead);
    doc["Pressure"] =String(lastPressionRead);
    doc["Host_name"] = hostName;
    doc["Count"]= count;
    doc["hour"] = String(hour()); 
    doc["minute"] = String(minute());
    doc["flag"] = seTimeOk;
    doc["compDate"]=__DATE__;
    doc["compTime"]=__TIME__;
    doc["Voltage"]=String(ESP.getVcc()/1000.00);
    serializeJson(doc, json);
    request->send(200, "text/json", json);
}

/*
{
  "dir": [
    {
      "file": [
        "/board.txt",
        "10"
      ]
    },
    {
      "file": [
        "/index.html",
        "6933"
      ]
    },
    {
      "file": [
        "/login.html",
        "4468"
      ]
    },
    {
      "file": [
        "/setting.html",
        "7426"
      ]
    }
  ],
  "ip": "192.168.1.71",
  "infosys": [
    "Total Space:     957314 byte",
    "Space Used:      20331 byte",
    "Block Size:      8192 byte",
    "Page Size:       957314 byte",
    "Max open files:  5 files",
    "Max path lenght: 32",
    "Count:           149409355",
    "Stato:            ESP start, "
  ],
  "RSSI": "-83",
  "SSID": "FASTWEB-F31559",
  "Board_Name": "esp_aef385", oppure "esp-bd13a8"
  "Temperature": "28,9",
  "Humidity": "68,9",
  
}*/

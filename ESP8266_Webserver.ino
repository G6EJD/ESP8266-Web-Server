// ESP8266 WebServer - using SDHT11 sensor
// (c) D L Bird 2016
//
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTTYPE DHT11 // or DHT22
#define DHTPIN  2
#define RELAY_Pin 5 //Initialize D1 aka GPIO-5 as an output on the WeMOS board

DHT_Unified dht(DHTPIN, DHTTYPE);

float humidity, temperature;  // Values read from sensor
String webpage = "";

#define temperature_error_offset  0
#define humidity_error_offset    16

const char *ssid     = "SKY6D12C";
const char *password = "WQSUBEPQ";

ESP8266WebServer server(80);

static bool hasSD = false;
//File root;
File uploadFile;
sensor_t sensor;
sensors_event_t event; 
  
void setup(void){
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  dht.begin();
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.print("Connecting to "); Serial.println(ssid);
  // Wait for connection
  int i = 0;
  while (WiFi.status() != WL_CONNECTED && i++ <= 10) {//wait 10 seconds
    delay(1000);
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print("Could not connect to network...");
    while(1) delay(500);
  }
  Serial.print("Connected to IP address: ");
  Serial.println(WiFi.localIP()); // Use the IP address printed here to connect to the server e.g. http://192.168.0.42

  // Use the next line if you want to use your own index page, otherwise the server will use index.htm on the SD-card
  //server.on("/", handle_root);                         // Define what happens when a client requests attention
  server.on("/LED_on",  led_on);                       // Define what happens when a client requests attention
  server.on("/LED_off", led_off);                      // Define what happens when a client requests attention
  server.on("/RELAY_on",  relay_on);                   // Define what happens when a client requests attention
  server.on("/RELAY_off", relay_off);                  // Define what happens when a client requests attention

  server.on("/dir", directory);                        // Define what happens when a client requests attention
  server.on("/DHT_readings", temp_and_humidity);       // Define what happens when a client requests attention
  server.on("/DHT_update", write_TempHumi_to_SD_card); // Define what happens when a client requests attention
    
  server.onNotFound(handleNotFound);
  
  server.begin();                                      // Start the webserver
  if (SD.begin(SS)){
    Serial.println("SD Card initialized.");
    hasSD = true;
  }
}

void loop(){
  server.handleClient();
}

//----------- Subroutines
//
// Use this function to write to the client your own index page, currently it would send the text (No HTML) 'ESP8266 Web Server'
// Ideally construct a web page in HTML like the example in the function temp_and_humidity
void handle_root() {
  server.send(200, "text/plain", "ESP8266 Web Server");
}

// Read DHT for temperature and humidity, create web page (html) and display it
void temp_and_humidity() {
  update_temperature_and_humidity();
  append_webpage_header();
  webpage += "<div id=\"header\"><h1>ESP8266 Web Server using SD Card</h1></div>";
  webpage += "<div id=\"section\"><h2>DHT11 Readings</h2>";
  webpage += "Temperature: " + String((int)temperature) + "&#176;C&nbsp&nbsp&nbsp"; // and some no-break spaces
  webpage += "Humidity: "    + String((int)humidity)    + "%rh";
  webpage += "<p><a href=\"index.htm\">Home Page</a></p>";
  webpage += "</div>";
  webpage += "<div id=\"footer\">Copyright &copy; D L Bird 2016</div>";
  webpage += "<p><a href=\"index.htm\">Home Page</a></p>";
  webpage += "<p><a href=\"DHT_update\">Update readings</a></p></body></html>";
  server.send(200, "text/html", webpage);
}

// Read DHT for temperature and humidity
void update_temperature_and_humidity() {
  dht.temperature().getEvent(&event);
  temperature = event.temperature + temperature_error_offset;
  dht.humidity().getEvent(&event);
  humidity    = event.relative_humidity  + humidity_error_offset; // Read humidity as %rh
  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
}

// Switch on-board LED ON, create web page and display it
void led_on() {
  append_webpage_header();
  webpage += "<div id=\"header\"><h1>ESP8266 DHT11 Web Server LED Control</h1></div>";
  webpage += "<div id=\"section\"><h2>LED Status</h2>";
  webpage += "LED has been switched ON";
  webpage += "</div>";
  webpage += "<div id=\"footer\">Copyright &copy; D L Bird 2016</div>";

  webpage += "<p><a href=\"page3.htm\">LED Controls</a></p>";
  webpage += "<p><a href=\"index.htm\">Home Page</a></p></body></html>";
  server.send(200, "text/html", webpage);
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, LOW);   // Turn the LED on, set pin Low
  delay(1000);
  Serial.println("Switched LED ON");
}

// Switch on-board LED OFF, create web page and display it
void led_off() {
  append_webpage_header();
  webpage += "<div id=\"header\"><h1>ESP8266 DHT11 Web Server LED Control</h1></div>";
  webpage += "<div id=\"section\"><h2>LED Status</h2>";
  webpage += "LED has been switched OFF";
  webpage += "</div>";
  webpage += "<div id=\"footer\">Copyright &copy D L Bird 2016</div>";

  webpage += "<p><a href=\"page3.htm\">LED Controls</a></p>";
  webpage += "<p><a href=\"index.htm\">Home Page</a></p></body></html>";
  server.send(200, "text/html", webpage);
  pinMode(LED_BUILTIN, OUTPUT);     // Initialize the LED_BUILTIN pin as an output
  digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off, set pin High
  delay(1000);
  Serial.println("Switched LED OFF");
}

// Switch RELAY ON, create web page and display it
void relay_on() {
  append_webpage_header();
  webpage += "<div id=\"header\"><h1>ESP8266 Web Server RELAY Control</h1></div>";
  webpage += "<div id=\"section\"><h2>RELAY Status</h2>";
  webpage += "RELAY has been switched ON";
  webpage += "</div>";
  webpage += "<div id=\"footer\">Copyright &copy D L Bird 2016</div>";

  webpage += "<p><a href=\"page4.htm\">Relay Controls</a></p>";
  webpage += "<p><a href=\"index.htm\">Home Page</a></p></body></html>";
  server.send(200, "text/html", webpage);
  pinMode(RELAY_Pin, OUTPUT);     // Initialize the D1 aka GPIO5 as an output
  digitalWrite(RELAY_Pin, HIGH);   // Turn the RELAY on (Note the Relay is active high
  Serial.println("Switched RELAY ON");
}


// Switch RELAY OFF create web page and display it
void relay_off() {
  append_webpage_header();
  webpage += "<div id=\"header\"><h1>ESP8266 Web Server RELAY Control</h1></div>";
  webpage += "<div id=\"section\"><h2>RELAY Status</h2>";
  webpage += "RELAY has been switched OFF";
  webpage += "</div>";
  webpage += "<div id=\"footer\">Copyright &copy D L Bird 2016</div>";

  webpage += "<p><a href=\"page4.htm\">Relay Controls</a></p>";
  webpage += "<p><a href=\"index.htm\">Home Page</a></p></body></html>";
  server.send(200, "text/html", webpage);
  pinMode(RELAY_Pin, OUTPUT);     // Initialize D1 aka GPIO5 as an output
  digitalWrite(RELAY_Pin, LOW);   // Turn the RELAY off (Note that relay is active HIGH, so LOW is off
  Serial.println("Switched RELAY OFF");
}

// Read SD Card contents and display the result by creating a web page and displaying it
void directory() {
  uploadFile = SD.open("/");
  append_webpage_header();
  webpage += "<div id=\"header\"><h1>ESP8266 DHT11 Web Server SD Card Directory</h1></div>";
  webpage += "<div id=\"section\"><h2>Directory Listing</h2><table border=\"1\">";
  uploadFile.rewindDirectory();
  print_SD_Directory(uploadFile); // Second paramater is number of prefix TABS before files are listed
  webpage += "</table></div>";
  webpage += "<div id=\"footer\">Copyright &copy; D L Bird 2016</div></body></html>";
  server.send(200,"text/html",webpage);
}

// Read SD Card contents and display the result by creating a web page and displaying it
void print_SD_Directory(File dir) {
  while (true) {
    File entry =  dir.openNextFile();
    if (! entry) {
      // no more files
      break;
    }
    webpage += "<tr><td>";
    webpage += entry.name();
    webpage += "&nbsp;</td>";
    if (entry.isDirectory()) {
      webpage += "<td>/</td>";
      print_SD_Directory(entry);
    } else {
      // files have sizes, directories do not
      webpage += "<td>";
      webpage += entry.size();
      webpage += "-Bytes&nbsp;</td></tr>";
    }
    entry.close();
  }
}

// Read file from SD Card and display it
bool loadFromSdCard(String path){
  String dataType = "text/plain";
  if(path.endsWith("/")) path += "index.htm";
  if(path.endsWith(".src")) path = path.substring(0, path.lastIndexOf("."));
  else if(path.endsWith(".htm")) dataType = "text/html";
  else if(path.endsWith(".css")) dataType = "text/css";
  else if(path.endsWith(".js"))  dataType = "application/javascript";
  else if(path.endsWith(".png")) dataType = "image/png";
  else if(path.endsWith(".gif")) dataType = "image/gif";
  else if(path.endsWith(".jpg")) dataType = "image/jpeg";
  else if(path.endsWith(".ico")) dataType = "image/x-icon";
  else if(path.endsWith(".xml")) dataType = "text/xml";
  else if(path.endsWith(".pdf")) dataType = "application/pdf";
  else if(path.endsWith(".zip")) dataType = "application/zip";

  File dataFile = SD.open(path.c_str());
  if(dataFile.isDirectory()){
    path += "/index.htm";
    dataType = "text/html";
    dataFile = SD.open(path.c_str());
  }

  if (!dataFile)
    return false;

  if(server.hasArg("download")) dataType = "application/octet-stream";

  server.sendHeader("Content-Length", String(dataFile.size()));
  server.sendHeader("Connection", "close");
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.send(200, dataType.c_str(), "");

  WiFiClient client = server.client();
  size_t totalSize = dataFile.size();
  if (client.write(dataFile, HTTP_DOWNLOAD_UNIT_SIZE) != totalSize) {
    Serial.println("Sent less data than expected!");
  }

  dataFile.close();
  return true;
}

// Handle file not found situation
void handleNotFound(){
  if(hasSD && loadFromSdCard(server.uri())) return;
  String message = "SDCARD Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " NAME:"+server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  Serial.print(message);
}

// Read temperature and humidity, create web page called temphumi.htm and write to SD Card, then display web page
// Demonstartes how derived data can be written to SD Card for transfer to an HTML environment
void write_TempHumi_to_SD_card() { // writes data and a html file to the SD card as an example
  char file_name[] = "TempHumi.htm";
  if (SD.exists(file_name)) {
    SD.remove(file_name);
  }
  File DataFile = SD.open(file_name, FILE_WRITE);
  // if the file opened okay, so write to it:
  if (DataFile) {
     update_temperature_and_humidity();
     Serial.print("Writing to HTM file ...");
     append_webpage_header();
     webpage += "<div id=\"header\"><h1>ESP8266 DHT11 Web Server</h1></div>";
     webpage += "<div id=\"section\"><h2>DHT11 Readings</h2>";
     webpage += "Temperature: " + String((int)temperature) + "&#176;C&nbsp&nbsp&nbsp"; // and some no-break spaces
     webpage += "Humidity: "    + String((int)humidity)    + "%rh";
     webpage += "</div>";
     webpage += "<div id=\"footer\">Copyright &copy; D L Bird 2016</div>";
     
     webpage += "<p><a href=\"DHT_update\">Update readings</a></p>";
     webpage += "<p><a href=\"index.htm\">Home Page</a></p></body></html>";
     DataFile.println(webpage);
     // close the file:
     DataFile.close();
     server.send(200, "text/html", webpage);
     Serial.println("Completed");
   } else {
     // if the file didn't open, print an error:
     Serial.println("Error opening TempHumi.htm");
   }
}

// A short methoid of adding the same web page header to some text
void append_webpage_header() {
  // webpage is a global variable
  webpage = ""; // Clear the variable
  webpage += "<!DOCTYPE html><html lang=\"en\"><head><title>DHT Web Server</title>";
  webpage += "<style>";
  webpage += "#header  {background-color:blue;      font-family:Tahoma,Verdana,Serif,sans-serif; width:1024px; padding:5px; color:white; text-align:center; }";
  webpage += "#section {background-color:#C2DEFF;   font-family:Tahoma,Verdana,Serif,sans-serif; width:1024px; padding:5px; color:blue;  font-size:20px; text-align:center;}";
  webpage += "#footer  {background-color:steelblue; font-family:Tahoma,Verdana,Serif,sans-serif; width:1024px; padding:5px; color:white; font-size:12px; clear:both; text-align:center;}";
  webpage += "</style></head><body>";
}

/*
 * HTML Web Page Template for reference
<!DOCTYPE html>
<html>
<head>
<title>DHT Web Server</title>
<style>
#header {
    background-color:blue;
    color:white;
    text-align:center;
    padding:5px;
}
#nav {
    line-height:30px;
    background-color:#C2DEFF;
    height:300px;
    width:100px;
    float:left;
    padding:5px;        
}
#section {
    background-color:#C2DEFF;
    width:800px 
    float:left;
    padding:10px;    
}
#footer {
    background-color:steelblue;
    color:white;
    clear:both;
    text-align:center;
    padding:5px;     
}
</style>
</head>
<body>

<div id="header">
<h1>ESP8266 DHT11 Web Server</h1>
</div>

<div id="section">
<h2>DHT11 Readings</h2>
<p>Temeperature=</p>
<p>Humidity=</p>
</div>

<div id="footer">
Copyright © D L Bird 2016
</div>

</body>
</html>
 */

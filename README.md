# ESP8266
ESP8266 Web Server

A web server built using the ESP8266, I used a WEMOS D1 Mini to ease the build and assembly, but any ESP8266 would do.

The webserver uses files loaded from an SD Card, in my case the WEMOS SD shield and controls the on-board LED, the WEMOS Relay shield and
displays images loaded from the card. There is a index page that controls the I/O functions.

The WEMOS DHT11 shield is also used to display on a web page the current temperature and humidity. To do this, two examples are provided, one uses the ESP8266 to read the DHT temp and humidity then adds an HTML wrappers and directly serves the HTML to the client, in the other the ESP8266 reads the temperature and humidity then writes this data to a text file on the SD Card, then uses HTML to construct a web page around the values to be displayed and shows how values can be parsed between environments and used for data logging.

With the exception of the ESP8266 .ini file copy all files to an SD Card, insert it to the SD Card reader, compile the code and see the server in action...

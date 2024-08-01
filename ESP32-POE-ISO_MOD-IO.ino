//################# LIBRARIES ################
#include <iostream>
#include <string>
#include <ETH.h>
#include <MOD_IO.h>
#include <time.h>
#include <ESP32WebServer.h>  //https://github.com/Pedroalbuquerque/ESP32WebServer download and place in your Libraries folder

//################ VARIABLES ################
static bool eth_connected = false;

// Control MOD-IO at address 0x2C (the 7-bit address), 
// shifted 1-bit left the address is 0x58.
// Be sure to enter the correct I2C address of your module
MOD_IO modio(0x58);

// the media access control (ethernet hardware) address for the shield:
// "E0:5A:1B:6E:54:57";
byte mac[] = {0xE0, 0x5A, 0x1B, 0x6E, 0x54, 0x57};

// Site details
String siteheading    = "ESP32-PoE with MOD-IO";     // Site's Main Title
String sitetitle      = "ESP32-PoE Webserver";       // Appears on the tabe of a Web Browser
String yourfootnote   = "Olimex demo for ESP32-PoE"; // A foot note e.g. "My Web Site"
String siteversion    = "v1.0";  // Version of your Website

#define sitewidth  1024  // Adjust site page width in pixels as required

String webpage = ""; // General purpose variable to hold HTML code

// Set bind port
ESP32WebServer server(80);

// React to Ethernet events:
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      // This will happen during setup, when the Ethernet service starts
      Serial.println("ETH Started");
      //set eth hostname here
      ETH.setHostname("esp32-ethernet");
      break;

    case ARDUINO_EVENT_ETH_CONNECTED:
      // This will happen when the Ethernet cable is plugged 
      Serial.println("ETH Connected");
      break;

    case ARDUINO_EVENT_ETH_GOT_IP:
    // This will happen when we obtain an IP address through DHCP:
      Serial.print("Got an IP Address for ETH MAC: ");
      Serial.print(ETH.macAddress());
      Serial.print(", IPv4: ");
      Serial.print(ETH.localIP());
      if (ETH.fullDuplex()) {
        Serial.print(", FULL_DUPLEX");
      }
      Serial.print(", ");
      Serial.print(ETH.linkSpeed());
      Serial.println("Mbps");
      eth_connected = true;
      break;

    case ARDUINO_EVENT_ETH_DISCONNECTED:
      // This will happen when the Ethernet cable is unplugged 
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;

    case ARDUINO_EVENT_ETH_STOP:
      // This will happen when the ETH interface is stopped but this never happens
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;

    default:
      break;
  }
}

void setup() {
  //don't remove this delay, it helps fix the ethernet
  delay(250);

  Serial.begin(115200); // initialize serial communications
  modio.begin();
  
  WiFi.onEvent(WiFiEvent);
  ETH.begin();
    
  Serial.println("Connect your I2C sensors to the default SDA, SCL pins for your board shown here:");
  Serial.println("I2C SDA pin = " + String(SDA));
  Serial.println("I2C SCL pin = " + String(SCL)); // Connect I2C sensors to the default SDA and SCL pins! Check Serial port for details
  StartTime();

  Serial.println("Use this URL to connect: http://" + ETH.localIP().toString() + "/"); // Print the IP address

  // Supported routes
  server.on("/", PoE_Demo);
  server.on("/PoE_Demo", PoE_Demo);

  // Handle unsupported routes
  server.onNotFound(handleNotFound);   // If the user types something that is not supported, say so

  // Start server
  server.begin(); Serial.println(F("Webserver started...")); // Start the webserver
}

void loop () {
  server.handleClient();
}

void handleNotFound() {
  String message = "Not Found";
  server.send(404, "text/plain", message);
}

void PoE_Demo () {
  String MOD_IO_CheckBoxChoice = "";
  webpage = ""; // don't delete this command, it ensures the server works reliably!
  append_HTML_header();
  String IPaddress = ETH.localIP().toString();
  webpage += "<form action=\"http://" + IPaddress + "/PoE_Demo\" method=\"POST\">";
  webpage += "<table style='font-family:arial,sans-serif;font-size:16px;border-collapse:collapse;text-align:center;width:90%;margin-left:auto;margin-right:auto;'>";
  webpage += "<tr>";
  webpage += "</tr>";
  webpage += "</table><br><br>";

  // Input Form
  webpage += "Enable Relays: ";
  webpage += "<input type='checkbox' id='Relay1' name='EnableRelay' value='1' /> <label for='Relay1'>Relay 1  </label>";
  webpage += "<input type='checkbox' id='Relay2' name='EnableRelay' value='2' /> <label for='Relay2'>Relay 2  </label>";
  webpage += "<input type='checkbox' id='Relay3' name='EnableRelay' value='3' /> <label for='Relay3'>Relay 3  </label>";
  webpage += "<input type='checkbox' id='Relay4' name='EnableRelay' value='4' /> <label for='Relay4'>Relay 4  </label>";

  webpage += "<br><br>Disable Relays: ";
  webpage += "<input type='checkbox' id='Relay1' name='DisableRelay' value='1' /> <label for='Relay1'>Relay 1  </label>";
  webpage += "<input type='checkbox' id='Relay2' name='DisableRelay' value='2' /> <label for='Relay2'>Relay 2  </label>";
  webpage += "<input type='checkbox' id='Relay3' name='DisableRelay' value='3' /> <label for='Relay3'>Relay 3  </label>";
  webpage += "<input type='checkbox' id='Relay4' name='DisableRelay' value='4' /> <label for='Relay4'>Relay 4  </label>";
  webpage += "<br><br><br><button type='submit'> Submit </button>";

  // And so-on
  webpage += "</form></body>";
  append_HTML_footer();
  server.send(200, "text/html", webpage); // Send a response to the client to enter their inputs, if needed, Enter=defaults

  if (server.args() > 0 ) {
    // Process each input
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      String Argument_Name   = server.argName(i);
      int selected_relay = server.arg(i).toInt();
      
      Serial.println("Command: " + Argument_Name + "\tTarget Relay: " + selected_relay);
      
      // Set desired relay state
      int state = 0;
      if (Argument_Name == "EnableRelay") {
        state = 1;
      }
      if (Argument_Name == "DisableRelay") {
        state = 0;
      }

      switch (selected_relay) {
        case 1:
          modio.setRelay(RELAY1, state);
          break;
        case 2:
          modio.setRelay(RELAY2, state);
          break;
        case 3:
          modio.setRelay(RELAY3, state);
          break;
        case 4:
          modio.setRelay(RELAY4, state);
          break;
      }
    }
  }
}

void StartTime() {
  configTime(0, 0, "pool.ntp.org", "10.1.0.1");
  setenv("TZ", "AEST-10", 1); // Set for your locale
  delay(200);
  GetTime();
}

String GetTime() {
  struct tm timeinfo;
  while (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    StartTime();
  }
  //See http://www.cplusplus.com/reference/ctime/strftime/
  Serial.println(&timeinfo, "%a %b %d %Y   %H:%M:%S"); // Displays: Saturday, June 24 2017 14:05:49
  char output[50];
  strftime(output, 50, "%d/%m/%y %H:%M:%S", &timeinfo); // Format needed for Google Charts is "11/12/17 22:01:00"; //dd/mm/yy hh:hh:ss
  return output;
}

void append_HTML_header() {
  webpage  = "";
  webpage += "<!DOCTYPE html><html><head>";
  webpage += "<meta http-equiv='refresh' content='300'>"; // 5-min refresh time, test needed to prevent auto updates repeating some commands
  webpage += "<style>";
  webpage += "body {width:" + String(sitewidth) + "px;margin:0 auto;font-family:arial;font-size:14px;text-align:center;color:blue;background-color:#F7F2Fd;}";
  webpage += "h1 {background-color:#ffc66e;margin:16px 30px;}"; // Orange background
  webpage += "h3 {color:#9370DB;font-size:24px;width:auto;}";
  webpage += ".navbar{overflow:hidden;background-color:#558ED5;position:fixed;top:0;width:" + String(sitewidth) + "px;margin-left:30px;}";
  webpage += ".navbar a {float:left;display:block;color:yellow;text-align:center;padding:10px 12px;text-decoration: none;font-size:17px;}";
  webpage += ".main{padding:0px;margin:16px 30px;height:1000px;width:" + String(sitewidth) + "px;}";
  webpage += ".style1{text-align:center;font-size:16px;background-color:#FFE4B5;}";
  webpage += ".style2{text-align:left;font-size:16px;background-color:#F7F2Fd;width:auto;margin:0 auto;}";
  // Note: You cannot include (table, tr, td, or th) styles if you want Google Charts to work!
  webpage += "</style>";
  webpage += "</head><body>";
  webpage += "<div class='navbar'>";
  // For each new page you add or remove, make sure there is a menu item to call it or remove it when not used
  //webpage += " <a href='/homepage'>Home</a>";
  //webpage += " <a href='/PoE_Demo'>PoE Demo</a>";// SPP
  webpage += "</div>";
  webpage += "<br><title>" + sitetitle + "</title><br>";
  webpage += "<div class='main'><h1>" + siteheading + " " + siteversion + "</h1>";
}

void append_HTML_footer() {
  webpage += "<footer><p>" + yourfootnote + "<br>";
  webpage += "&copy; Project made by Carlos Aya. You can find it here:<br>https://github.com/G6EJD/ESP32-General-Purpose-Webserver</p></footer>";
  webpage += "</div></body></html>";
}

#include <NativeEthernet.h>
#include <Wire.h>
#include "MS5837.h"
#include "SdFat.h"

// assign a MAC address for the Ethernet controller.
// fill in your address here:
byte mac[] = {
  0x04, 0xE9, 0xE5, 0x14, 0xDA, 0xE8
};
// assign an IP address for the controller:
IPAddress ip(192, 168, 11, 99);
//IPAddress ip(169, 254, 125, 165);
//IPAddress ip(192, 168, 20, 188); //IP need to be in the same ip subnet and a free IP. Use the address in previous line if not connected to internet 
//byte serv[] = {192, 168, 1, 127};

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(80);

long Pressure;
long Temperature;
long Depth;
long Altitude;

MS5837 sensor;

SdFs sd;
FsFile file;

// Use FIFO SDIO.
#define SD_CONFIG SdioConfig(FIFO_SDIO)

// Log file base name.  Must be six characters or less.
#define FILE_BASE_NAME "Data"
const uint8_t BASE_NAME_SIZE = sizeof(FILE_BASE_NAME) - 1;
char fileName[13] = FILE_BASE_NAME "00.csv";

//--------------------------------------------------------------------------------------------------
void listenForEthernetClients() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Got a client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println ("Refresh: 5");
          client.println();
          // print the current readings, in HTML format:
          client.println ("<!DOCTYPE HTML>");
          client.println ("<html>");
          client.print ("<Title>Teensy Reading </Title>");
          client.println ("<h1>MS5837 Pressure Sensor Reading </h1>");
          client.print("Pressure (mb): ");
          client.print(Pressure);
          client.print("<br>Temperature (C): ");
          client.print(Temperature);
          client.print("<br>Depth (m): ");
          client.print(Depth);
          client.print("<br>Altitude (m): ");
          client.print(Altitude);
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
}
//--------------------------------------------------------------------------------------------------

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);

    
    Serial.begin(9600);

    Serial.println("Begin setup");

    // start the Ethernet connection
    Ethernet.begin(mac, ip);
  
    //Sensor being
    Wire.begin();
/*
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }*/
  while (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
    delay(500);
  }

  while (!sensor.init(Wire)) {
    Serial.println("Sensor init failed!");
    Serial.println("\n\n\n");
    delay(5000);
  }
  sensor.setModel(MS5837::MS5837_30BA);
  sensor.setFluidDensity(1029); // kg/m^3 (997 freshwater, 1029 for seawater)

  // start listening for clients
  server.begin();

  sd.begin(SD_CONFIG);

  while (sd.exists(fileName)) {
    if (fileName[BASE_NAME_SIZE + 1] != '9') {
    fileName[BASE_NAME_SIZE + 1]++;
  } else if (fileName[BASE_NAME_SIZE] != '9') {
    fileName[BASE_NAME_SIZE + 1] = '0';
    fileName[BASE_NAME_SIZE]++;
  } 
  }
  file.open(fileName, O_WRONLY | O_CREAT | O_EXCL);

  Serial.println("File opened");
  file.println("Pressure(mbar),Temp(deg),Depth(m),Altitude(m),Time(ms)");
  Serial.println("End setup");
}


void loop() {

    // Single reads
    sensor.read();
    Pressure = sensor.pressure();
    Temperature = sensor.temperature();
    Depth = sensor.depth();
    Altitude = sensor.altitude();

    // listen for incoming Ethernet connections:
    listenForEthernetClients();
 
    file.print(Pressure);
    file.print(",");
    file.print(Temperature);
    file.print(",");
    file.print(Depth);
    file.print(",");
    file.print(Altitude);
    file.print(",");
    file.println(millis());

    file.flush();

    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    delay(100000);

}

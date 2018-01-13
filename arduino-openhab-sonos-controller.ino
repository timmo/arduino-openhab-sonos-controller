/*
   UIPEthernet TcpClient example.

   UIPEthernet is a TCP/IP stack that can be used with a enc28j60 based
   Ethernet-shield.

   UIPEthernet uses the fine uIP stack by Adam Dunkels <adam@sics.se>

        -----------------

   This TcpClient example gets its local ip-address via dhcp and sets
   up a tcp socket-connection to 192.168.0.1 port 5000 every 5 Seconds.
   After sending a message it waits for a response. After receiving the
   response the client disconnects and tries to reconnect after 5 seconds.

   Copyright (C) 2013 by Norbert Truchsess <norbert.truchsess@t-online.de>
*/

#include <UIPEthernet.h>

EthernetClient client;
signed long tcpTimeout;

uint8_t mac[6] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05};

// Destination host and port
const char dsthost[] = "10.49.8.193";
IPAddress dstip = IPAddress(10, 49, 8, 193);
const uint16_t dstport = 8080;

String presets;


void setup() {

  Serial.begin(9600);

  Serial.println("Welcome");

  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(5000);
  }
  Serial.println("Ethnert initialized");

  Serial.print("localIP: ");
  Serial.println(Ethernet.localIP());
  Serial.print("subnetMask: ");
  Serial.println(Ethernet.subnetMask());
  Serial.print("gatewayIP: ");
  Serial.println(Ethernet.gatewayIP());
  Serial.print("dnsServerIP: ");
  Serial.println(Ethernet.dnsServerIP());

  tcpTimeout = 0;

  loadPresets();

}

void loop() {

  delay(5000);
}

void loadPresets() {
  tcpTimeout = millis() + 5000;

  Serial.println("Loading Presets ....");
  if (client.connect(dstip, dstport))
  {
    Serial.print("Connected to ");
    Serial.println(dsthost);

    // send request

    char getbuf[96] = {0};
    char hostbuf[32] = {0};
    sprintf(getbuf, "GET /rest/items/SonosPresets/state");
    sprintf(hostbuf, "\n Host: %s\n\n", dsthost);
    client.print(getbuf);
    client.print(hostbuf);
    Serial.println("Request Sent");

    // response handling

//    client.println("DATA from Client: ");
    while (client.available() == 0)
    {
      if (tcpTimeout - millis() < 0)
        goto stop;
    }
    Serial.println("Response received");
    int size;
    while ((size = client.available()) > 0)
    {
      char* data = (char*)malloc(size + 1);
      size = client.read(data, size);
      data[size] = '\0';
      presets = String(data);
      free(data);
    }
stop:
    client.stop();
    Serial.print("Presets loaded: ");
    Serial.println(presets);
  }
  else {
    Serial.println("Connection failed!");
  }

}


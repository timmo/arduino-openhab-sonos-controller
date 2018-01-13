#include <UIPEthernet.h>
#include <HttpClient.h>

EthernetClient c;
HttpClient http(c);
byte mac[6] = {0x00, 0x01, 0x02, 0x03, 0x03, 0x07};
const char kHostname[] = "Timmos-MacBook.de.meteogroup.net";
const char kPath[] = "/rest/items/SonosPresets/state";
const int kNetworkTimeout = 5 * 1000;
const int kNetworkDelay = 1000;

// #include <SPI.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>
// #define OLED_RESET 4
// Adafruit_SSD1306 display(OLED_RESET);
// #if (SSD1306_LCDHEIGHT != 64)
// #error("Height incorrect, please fix Adafruit_SSD1306.h!");
// #endif

#define BUF_SIZE 100
char presets[BUF_SIZE];
int presetsLength = 0;
int numPresets = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("Welcome"));

  // initializeDisplay();
  initializeEthernet();
  // displayIpAddress();

  loadPresets();
  tokenizePresets();
  printoutPresets();

  // displayPresets();
}

void initializeEthernet()
{
  delay(1000);
  while (Ethernet.begin(mac) != 1)
  {
    Serial.println(F("Error getting IP address via DHCP, trying again..."));
    delay(3000);
  }
  Serial.print(F("localIP: "));
  Serial.println(Ethernet.localIP());
  // Serial.print("subnetMask: ");
  // Serial.println(Ethernet.subnetMask());
  // Serial.print("gatewayIP: ");
  // Serial.println(Ethernet.gatewayIP());
  // Serial.print("dnsServerIP: ");
  // Serial.println(Ethernet.dnsServerIP());
}

void loadPresets()
{
  int err = 0;
  err = http.get(kHostname, 8080, kPath);
  if (err != 0)
  {
    Serial.print(F("Cannot connect to "));
    Serial.println(err);
  }
  else
  {
    Serial.println(F("startedRequest ok"));

    int statusCode = http.responseStatusCode();
    if (statusCode < 0)
    {
      Serial.print(F("Getting response failed: "));
      Serial.println(err);
    }
    else
    {
      Serial.print(F("Got status code: "));
      Serial.println(statusCode);

      err = http.skipResponseHeaders();
      if (err >= 0)
      {
        unsigned long timeoutStart = millis();
        char c;
        presetsLength = 0;
        // Whilst we haven't timed out & haven't reached the end of the body
        while ((http.connected() || http.available()) &&
               ((millis() - timeoutStart) < kNetworkTimeout))
        {
          if (http.available())
          {
            presets[presetsLength++] = http.read();
            // We read something, reset the timeout counter
            timeoutStart = millis();
          }
          else
          {
            // We haven't got any data, so let's pause to allow some to
            // arrive
            delay(kNetworkDelay);
          }
        }
        presets[presetsLength] = 0;
      }
      else
      {
        Serial.print(F("Failed to skip response headers: "));
        Serial.println(err);
      }
    }
  }
  http.stop();
}

void tokenizePresets()
{
  // Serial.println("Tokenize Presets:");
  if (presets > 0)
    numPresets = 1;
  else
    numPresets = 0;
  for (int i = 0; i < presetsLength; i++)
  {
    if (presets[i] == ',')
    {
      presets[i] = 0;
      numPresets++;
    }
  }
}

char *getPreset(int p)
{
  int token = 0;
  for (int i = 0; i < presetsLength; i++)
  {
    if (token == p)
      return &presets[i];
    if (presets[i] == 0)
      token++;
  }
  return nullptr;
}

void printoutPresets()
{
  Serial.println("Presets:");
  for (int i = 0; i < numPresets; i++)
  {
    Serial.print(i);
    Serial.print(" ");
    Serial.println(getPreset(i));
  }
}

// void initializeDisplay()
// {
//   // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
//   display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)
//   display.clearDisplay();
// }

// void displayIpAddress(){

//   display.clearDisplay();
//   display.setTextSize(2);
//   display.print(F("local IP: ")); display.println(Ethernet.localIP());

//   // Serial.print("subnetMask: ");
//   // Serial.println(Ethernet.subnetMask());
//   // Serial.print("gatewayIP: ");
//   // Serial.println(Ethernet.gatewayIP());
//   // Serial.print("dnsServerIP: ");
//   // Serial.println(Ethernet.dnsServerIP());
// }

// void displayPresets()
// {
//   display.setTextSize(1);
//   display.setTextColor(WHITE);
//   display.setCursor(0,0);
//   display.println("Hello, world!");
//   display.setTextColor(BLACK, WHITE); // 'inverted' text
//   display.println(3.141592);
//   display.setTextSize(2);
//   display.setTextColor(WHITE);
//   display.print("0x"); display.println(0xDEADBEEF, HEX);
//   display.display();
//   display.setTextSize(3);
//   display.setTextColor(WHITE);
//   display.println("Alarm");
//   display.display();
// }

void loop()
{
}

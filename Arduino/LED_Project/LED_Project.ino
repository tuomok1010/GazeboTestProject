#include <SPI.h>
#include <WiFiNINA.h>

#include "network_data.h"

char ssid[] = SSID_NAME;
char pass[] = PASS;
int status = WL_IDLE_STATUS;

IPAddress targetAddr(192, 168, 243, 208);
const unsigned int targetPort = 8445;

// how often retry connection to the wifi network
unsigned long wifiConnectStartMS = 0;
const unsigned long wifiConnectLimitMS = 5000;
bool bPrintWifiData = false;
const byte nInitialWifiConnectionAttempts = 10;

// how often retry a TCP connection to given port and address
// connect() blocks for a while, so we don't want to do this very frequently
unsigned long tcpConnectStartMS = 0;
const unsigned long tcpConnectFreqMS = 60000;

// how often does the led blink
unsigned long ledStartMS = 0;
const unsigned long ledBlinkFreqMs = 1000;
bool ledVal = false;

WiFiClient client;

void setup() 
{
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  if(SetupWifi())
    SetupConnection(targetAddr, targetPort);

  wifiConnectStartMS = millis();
  ledStartMS = millis();
}

void loop() 
{
  ReconnectWifi();
  if(status == WL_CONNECTED) 
  {
    RetryConnection(targetAddr, targetPort);
  }
  
  BlinkLED();
}

bool SetupWifi()
{
  for(unsigned int i = 0; i < nInitialWifiConnectionAttempts; ++i)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);
    delay(wifiConnectLimitMS);

    if(status == WL_CONNECTED) 
    {
      Serial.println("wifi connection success:");
      printCurrentNet();
      printWiFiData();
      return true;
    }
  }

  Serial.println("wifi connection failed!");
  return false;
}

void ReconnectWifi()
{
  // attempt to connect to WiFi network and wait for connectLimit milliseconds for a connection:
  if(status != WL_CONNECTED) 
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);

    unsigned long wifiConnectCurrentMS = millis();
    if(wifiConnectCurrentMS - wifiConnectStartMS >= wifiConnectLimitMS)
    {
      // Connect to WPA/WPA2 network:
      status = WiFi.begin(ssid, pass);
      wifiConnectStartMS = millis();
      bPrintWifiData = true;
    }
  }
  else
  {
    wifiConnectStartMS = 0;
    if(bPrintWifiData)
    {
      Serial.println("wifi connection success:");
      printCurrentNet();
      printWiFiData();
      bPrintWifiData = false;
    }
  }
}

void SetupConnection(IPAddress addr, unsigned int port)
{
  Serial.println("setting up TCP connection...");
  if(client.connect(addr, port))
  {
    Serial.print("succesfully connected to ");
  }
  else
  {
    Serial.print("unable to connect to ");
  }

  Serial.print(addr[0], DEC);
  Serial.print(".");
  Serial.print(addr[1], DEC);
  Serial.print(".");
  Serial.print(addr[2], DEC);
  Serial.print(".");
  Serial.print(addr[3], DEC);
  Serial.print(" on port ");
  Serial.println(port, DEC);
}

void RetryConnection(IPAddress addr, unsigned int port)
{
  if(!client.connected())
  {
    unsigned long tcpConnectCurrentMS = millis();
    if(tcpConnectCurrentMS - tcpConnectStartMS >= tcpConnectFreqMS)
    {
      Serial.println("connection lost, reconnecting...");
      if(client.connect(addr, port))
      {
        Serial.print("succesfully connected to ");
      }
      else
      {
        Serial.print("unable to connect to ");
      }

      Serial.print(addr[0], DEC);
      Serial.print(".");
      Serial.print(addr[1], DEC);
      Serial.print(".");
      Serial.print(addr[2], DEC);
      Serial.print(".");
      Serial.print(addr[3], DEC);
      Serial.print(" on port ");
      Serial.println(port, DEC);

      tcpConnectStartMS = millis();
    }
  }
}

void BlinkLED()
{
  unsigned long ledCurrentMS = millis();
  if(ledCurrentMS - ledStartMS >= ledBlinkFreqMs)
  {
    digitalWrite(LED_BUILTIN, ledVal);
    ledVal = !ledVal;
    ledStartMS = millis();
    SendData();
  }
}

void printWiFiData() 
{

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();

  Serial.print("IP address : ");
  Serial.println(ip);
  Serial.print("Subnet mask: ");
  Serial.println((IPAddress)WiFi.subnetMask());
  Serial.print("Gateway IP : ");
  Serial.println((IPAddress)WiFi.gatewayIP());

  // print your MAC address:
  byte mac[6];
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printMacAddress(mac);
}

void printCurrentNet() 
{

  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print the MAC address of the router you're attached to:
  byte bssid[6];
  WiFi.BSSID(bssid);
  Serial.print("BSSID: ");
  printMacAddress(bssid);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI): ");
  Serial.println(rssi);

  // print the encryption type:
  byte encryption = WiFi.encryptionType();
  Serial.print("Encryption Type: ");
  Serial.println(encryption, HEX);
  Serial.println();
}

void printMacAddress(byte mac[]) 
{
  for (int i = 5; i >= 0; i--) 
  {
    if (mac[i] < 16) 
    {
      Serial.print("0");
    }

    Serial.print(mac[i], HEX);
    if (i > 0) 
    {
      Serial.print(":");
    }
  }
  Serial.println();
}

void SendData()
{
  int data = digitalRead(LED_BUILTIN);
  String dataStr = "/led:" + (String)data;
  Serial.print("sending data: ");
  Serial.println(dataStr);

  if(client.print(dataStr) != dataStr.length() + 1)
  {
    Serial.println("error sending data");
  }
}
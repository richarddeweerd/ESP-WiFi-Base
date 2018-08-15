/*
Library for storing the config on a esp8266
*/
#ifndef EspConfig_h
#define EspConfig_h

#include "Arduino.h"
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

#define configVersion 4

#define default_username "admin"
#define default_devicename "ESP Demo"
#define default_ntp "pool.ntp.org"


class EspConfig
{
  public:
    EspConfig();    
    void init();
    void setVersion(int _value);

    void setDevicename(String _value);
    void setUsername(String _value);
    void setUserpass(String _value);
    void setNtpServer(String _value);
   
    void setDhcp(bool _value);

    void setIpAddress(IPAddress _value);
    void setGateway(IPAddress _value);
    void setSubnet(IPAddress _value);
    void setDns0(IPAddress _value);
    void setDns1(IPAddress _value);

    void dbg();

    
    int cfgversion;
    char devicename[32];
    char username[20];
    char userpass[20];
    
    char ntpserver[64];

    bool dhcp;
    IPAddress ip;
    IPAddress sn;
    IPAddress gw;
    IPAddress ns0;
    IPAddress ns1;
    
  private:
    void writeInt(String _filename, int _value);
    void writeStr(String _filename, String _value);
    void writeIp(String _filename, IPAddress _value);
    void writeBool(String _filename, bool _value);
    int readInt(String _filename);
    String readStr(String _filename);    
    IPAddress readIp(String _filename);
    bool readBool(String _filename);

    
    void createConfig();
    bool readError;
};

#endif

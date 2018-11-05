#include "Arduino.h"
#include "EspConfig.h"
#include "FS.h"
#include <ESP8266WiFi.h>



EspConfig::EspConfig(){
  readError = false;

}

void EspConfig::init(){
  readError = false;

  cfgversion = readInt("version");
  if (cfgversion == configVersion){
    readStr("devicename").toCharArray(devicename, sizeof(devicename));
    readStr("username").toCharArray(username, sizeof(username));
    readStr("userpass").toCharArray(userpass, sizeof(userpass));
    readStr("ntp").toCharArray(ntpserver, sizeof(ntpserver));
    dhcp = readBool("dhcp");
    ip = readIp("ipaddress");
    sn = readIp("subnet");
    gw = readIp("gateway");
    ns0 = readIp("dns0");
    ns1 = readIp("dns1");
    autodst=readBool("autodst");
    timezone=readInt("timezone");
    dstoffset=readInt("dst");
  } else{
    readError = true;
  }


  if (readError){
    Serial.println(F("Error reading config files."));
    createConfig();
  }
}



void EspConfig::createConfig(){
  setVersion(configVersion);

  setDevicename(default_devicename);
  setUsername(default_username);
  setUserpass("");
  setNtpServer(default_ntp);
  setAutoDst(true);
  setDhcp(true);
  setDst(3600);
  setTimeZone(3600);
  setIpAddress({0, 0, 0, 0});
  setSubnet({0, 0, 0, 0});
  setGateway({0, 0, 0, 0});
  setDns0({0, 0, 0, 0});
  setDns1({0, 0, 0, 0});
  
}

void EspConfig::setVersion(int _value){
  cfgversion = _value;
  writeInt("version", _value);
}

void EspConfig::setDevicename(String _value){
  _value.toCharArray(devicename, sizeof(devicename));
  writeStr("devicename", _value);
}

byte EspConfig::checkDevicename(String _value){
  //check if name is valid
  //errorcodes:
  //
  // 0 = Ok
  // 1 = illegal character
  // 2 = name too short
  // 3 = name too long

  #define minlen 1
  #define maxlen 32
  
  bool errorfound = false;
  for (byte i = 0; i < _value.length(); i++) {    
    if (!isAlphaNumeric(_value.charAt(i))) {
      // not alpha numeric
      if (i > 0) {

        if ((_value.charAt(i) != '-') && (_value.charAt(i) != '_')) {
          errorfound = true;
        }       
      }
      else {
        errorfound = true;
      }
    }
  }
  if (errorfound) {
    return 1;
  } else {
    if (_value.length() < minlen) {
      return 2;
    } else {
      if (_value.length() > maxlen) {  
        return 3;
      } 
    }
  }
  return 0;
}

byte EspConfig::checkUsername(String _value){
  //check if username is valid
  //errorcodes:
  //
  // 0 = Ok
  // 1 = illegal character
  // 2 = name too short
  // 3 = name too long
  
  #define minlen 3
  #define maxlen 20

  bool errorfound = false;
  for (byte i = 0; i < _value.length(); i++) {    
    if (!isAlphaNumeric(_value.charAt(i))) {
      errorfound = true;
    }
  }
  if (errorfound) {
    return 1;
  } else {
    if (_value.length() < minlen) {
      return 2;
    } else {
      if (_value.length() > maxlen) {  
        return 3;
      } 
    }
  }
  return 0;  
}

byte EspConfig::checkUserpass(String _value, String _value2){
  //check if passwords are valid
  //errorcodes:
  //
  // 0 = Ok
  // 1 = passwords are not the same
  // 2 = passwords too short
  // 3 = passwords too long
  
  #define minlen 3
  #define maxlen 20

  bool errorfound = false;
  if (_value != _value2) {
    return 1;
  }
  if (_value.length() < minlen) {
    return 2;
  } else {
    if (_value.length() > maxlen) {  
      return 3;
    } 
  }
  return 0;  
}

void EspConfig::setUsername(String _value){
  _value.toCharArray(username, sizeof(username));
  writeStr("username", _value);
}

void EspConfig::setUserpass(String _value){
  _value.toCharArray(userpass, sizeof(userpass));
  writeStr("userpass", _value);
}

void EspConfig::setNtpServer(String _value){
  _value.toCharArray(ntpserver, sizeof(ntpserver));
  writeStr("ntp", _value);
}

void EspConfig::setDhcp(bool _value){
  dhcp = _value;
  writeBool("dhcp", _value);
}

void EspConfig::setAutoDst(bool _value){
  autodst = _value;
  writeBool("autodst", _value);
}


void EspConfig::setDst(int _value){
  dstoffset = _value;
  writeInt("dst", _value);
}

void EspConfig::setTimeZone(int _value){
  timezone= _value;
  writeInt("timezone", _value);
}


void EspConfig::setIpAddress(IPAddress _value){
  for (byte i = 0; i < 4; i++ ){
    ip[i] = _value[i];
  }  
  writeIp("ipaddress", _value);
}

void EspConfig::setSubnet(IPAddress _value){
  for (byte i = 0; i < 4; i++ ){
    sn[i] = _value[i];
  }  
  writeIp("subnet", _value);
}

void EspConfig::setGateway(IPAddress _value){
  for (byte i = 0; i < 4; i++ ){
    gw[i] = _value[i];
  }  
  writeIp("gateway", _value);
}

void EspConfig::setDns0(IPAddress _value){
  for (byte i = 0; i < 4; i++ ){
    ns0[i] = _value[i];
  }  
  writeIp("dns0", _value);
}

void EspConfig::setDns1(IPAddress _value){
  for (byte i = 0; i < 4; i++ ){
    ns1[i] = _value[i];
  }  
  writeIp("dns1", _value);
}



void EspConfig::writeInt(String _filename, int _value){
  StaticJsonBuffer<100> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["val"] = _value;
  String fpath = "/cfg/" + _filename + ".json";
  File configFile = SPIFFS.open(fpath, "w");
  if (!configFile) {
    Serial.println(F("Failed to open file for writing"));
    //return false;
  }
  json.printTo(configFile);
  configFile.close();
 
}

void EspConfig::writeStr(String _filename, String _value){
  StaticJsonBuffer<100> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["val"] = _value;
  String fpath = "/cfg/" + _filename + ".json";
  File configFile = SPIFFS.open(fpath, "w");
  if (!configFile) {
    Serial.println(F("Failed to open file for writing"));
    //return false;
  }
  json.printTo(configFile);
  configFile.close();
}

void EspConfig::writeIp(String _filename, IPAddress _value){
  StaticJsonBuffer<100> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  
  JsonArray& ip = json.createNestedArray("ip");
  for (byte i = 0; i < 4; i++ ){
    ip.add(_value[i]);
  }  
  String fpath = "/cfg/" + _filename + ".json";
  File configFile = SPIFFS.open(fpath, "w");
  if (!configFile) {
    Serial.println(F("Failed to open file for writing"));
  }
  json.printTo(configFile);
  configFile.close();
}

void EspConfig::writeBool(String _filename, bool _value){
  StaticJsonBuffer<100> jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["val"] = _value;
  String fpath = "/cfg/" + _filename + ".json";
  File configFile = SPIFFS.open(fpath, "w");
  if (!configFile) {
    Serial.println(F("Failed to open file for writing"));
    //return false;
  }
  json.printTo(configFile);
  configFile.close();
}

int EspConfig::readInt(String _filename){

  String fpath = "/cfg/" + _filename + ".json";

  File configFile = SPIFFS.open(fpath, "r");
  if (configFile) {
    size_t size = configFile.size();
    if (size <=  1024) {
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      StaticJsonBuffer<500> jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      if (json.success()) {
        if (json.containsKey("val")){
          int res = json["val"];
          return res;
        }
      }
    }
  }
  Serial.println(F("Failed to open config file"));
  return 0;
}


String EspConfig::readStr(String _filename){

  String fpath = "/cfg/" + _filename + ".json";

  File configFile = SPIFFS.open(fpath, "r");
  if (configFile) {
    size_t size = configFile.size();
    if (size <=  1024) {
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      StaticJsonBuffer<500> jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      if (json.success()) {
        if (json.containsKey("val")){
          String res = json["val"];
          return res;
        }
      }
    }
  }
  Serial.println(F("Failed to open config file"));
  return "";
}


bool EspConfig::readBool(String _filename){

  String fpath = "/cfg/" + _filename + ".json";

  File configFile = SPIFFS.open(fpath, "r");
  if (configFile) {
    size_t size = configFile.size();
    if (size <=  1024) {
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      StaticJsonBuffer<500> jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      if (json.success()) {
        if (json.containsKey("val")){
          bool res = json["val"];
          return res;
        }
      }
    }
  }
  Serial.println(F("Failed to open config file"));
  return false;
}


IPAddress EspConfig::readIp(String _filename){

  String fpath = "/cfg/" + _filename + ".json";

  File configFile = SPIFFS.open(fpath, "r");
  if (configFile) {
    size_t size = configFile.size();
    if (size <=  1024) {
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);
      configFile.readBytes(buf.get(), size);
      StaticJsonBuffer<500> jsonBuffer;
      JsonObject& json = jsonBuffer.parseObject(buf.get());
      if (json.success()) {
        if (json.containsKey("ip")){
          IPAddress res;
          for (byte i = 0; i < 4; i++ ){
            res[i] = json["ip"][i];
          }
          return res;
        }
      }
    }
  }
  Serial.println(F("Failed to open config file"));
  return {0,0,0,0};
}

void EspConfig::dbg(){
  Dir dir = SPIFFS.openDir("/cfg");
  while (dir.next()) {
    Serial.println(dir.fileName());
    File f = dir.openFile("r");
    Serial.println(f.readString());
    f.close();
  }
}




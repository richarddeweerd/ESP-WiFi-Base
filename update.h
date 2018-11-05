#define dev_type "nixie"
#define dev_revision "1"
#define code_build "1"
#define updateServer "esp.deweerd.biz"
#define updatePort 8266

void otaUpdate(){
    //t_httpUpdate_return ret = ESPhttpUpdate.update(updateServer, updatePort, "/esp/update.php", "optional current version string here");
  String devtype = dev_type;
  String devrev = dev_revision;
  String target = "/esp/update.php?type=" + devtype + "_v" + dev_revision + "&name=" + config.devicename;
  Serial.println(target);
  t_httpUpdate_return ret = ESPhttpUpdate.update(updateServer, updatePort, target, code_build);
  switch(ret) {
      case HTTP_UPDATE_FAILED:
          Serial.println("[update] Update failed.");
          break;
      case HTTP_UPDATE_NO_UPDATES:
          Serial.println("[update] Update no Update.");
          break;
      case HTTP_UPDATE_OK:
          Serial.println("[update] Update ok."); // may not called we reboot the ESP
          break;
  }
}


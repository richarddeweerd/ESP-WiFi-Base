
String ipToString(IPAddress ip)
{
  String s = "";
  for (int i = 0; i < 4; i++)
    s += i ? "." + String(ip[i]) : String(ip[i]);
  return s;
}

void handleRoot()
{
  digitalWrite(led, 0);
  //time_t now;
  //now = time(nullptr);

  Serial.println(now());
  Serial.println(config.timezone);
  Serial.println(getDSTofset());

  String message = F("<html>");
  message += htmlHead(F("Home"), 10);
  message += F("<body>");
  message += topMenu(1);
  message += F("<div class="
               "main"
               ">");

  message += F("<h1>Hello from ");
  message += config.devicename;
  message += F("!</h1>");
  message += F("<p>Date: ");
  message += getDateString(now() + config.timezone + getDSTofset());
  message += F("<br>Time: ");
  message += getTimeString(now() + config.timezone + getDSTofset());
  message += F("</p>");
  message += F("</div>");
  message += F("</body></html>");

  webServer.send(200, F("text/html"), message);
  digitalWrite(led, 1);
}

void handleWifiSetup()
{
  if (!webServer.authenticate(config.username, config.userpass))
    return webServer.requestAuthentication();
  digitalWrite(led, 0);
  String message = F("<html>");

  if (WiFi.scanComplete() == -2)
  {
    //start wifiscan
    WiFi.scanNetworks(true);
  }
  if (WiFi.scanComplete() == -1)
  {
    //wifiscan in progress
    message += htmlHead(F("Setup"), 1);
    message += F("<body>");
    message += F("<div class="
                 "main"
                 ">");
    message += F("WiFi Scan in progress....");
  }
  else
  {
    //wifiscan complete
    message += htmlHead(F("WiFi Config"));
    message += F("<body>");
    message += topMenu(9);
    message += F("<div class="
                 "main"
                 ">");
    if (WiFi.scanComplete() == 0)
    {
      message += F("No networks in range");
    }
    else
    {
      message += F("<form action="
                   "/connect"
                   " method="
                   "post"
                   ">");
      message += F("<table>");
      message += F("<tr>");
      message += F("<td>Network SSID:</td><td>");
      message += F(" <select name ="
                   "ssid"
                   ">");
      for (int i = 0; i < WiFi.scanComplete(); i++)
      {
        message += F("<option value="
                     "");
        message += WiFi.SSID(i);
        if (WiFi.SSID(i) == WiFi.SSID())
        {
          message += F(""
                       " selected>");
        }
        else
        {
          message += F(""
                       ">");
        }
        message += WiFi.SSID(i);
        message += F("</option>");
      }
      message += F("</select>");
      message += F("</td></tr><tr>");
      message += F("<td>Password:</td><td>");

      message += F("<input type="
                   "password"
                   " name="
                   "pass"
                   ">");
      message += F("</td></tr><tr><td colspan='2' class='bcell'>");
      message += F("<button type='submit' value='Submit'>Save</button>&ensp;");
      message += F("<button type='reset' value='Reset'>Clear</button>");
      message += F("</td></tr></table>");
      message += F("</form>");
    }
    message += F("</div>");
    WiFi.scanDelete();
  }

  message += F("</body></html>");
  webServer.send(200, F("text/html"), message);
  digitalWrite(led, 1);
}

void handleConnect()
{
  digitalWrite(led, 0);
  String message = F("<html>");
  if (webServer.method() == HTTP_POST)
  {
    //save
    WiFi.mode(WIFI_AP_STA);
    char _ssid[webServer.arg("ssid").length() + 1];
    webServer.arg("ssid").toCharArray(_ssid, webServer.arg("ssid").length() + 1);

    char _pass[webServer.arg("pass").length() + 1];
    webServer.arg("pass").toCharArray(_pass, webServer.arg("pass").length() + 1);

    WiFi.begin(_ssid, _pass);
    //WiFi.begin(webServer.arg("ssid"), webServer.arg("pass"));
  }

  if (WiFi.status() == 6)
  {
    //still connecting
    message += htmlHead(F("Connect"), 1);
    message += F("<body>");
    message += F("<div class="
                 "main"
                 ">");
    message += F("Connecting...");
  }
  else
  {
    if (WiFi.status() == 3)
    {
      //connected!
      WiFi.setAutoReconnect(true);
      message += htmlHead(F("Connected"));
      message += F("<body>");
      message += F("<div class="
                   "main"
                   ">");
      message += F("Connected to: ");
      message += WiFi.SSID();
      message += F("<br><br>");
      message += F("Reboot the device and connect to IP address: ");
      message += ipToString(WiFi.localIP());
    }
    else
    {
      message += htmlHead(F("Error"));
      message += F("<body>");
      message += F("<div class="
                   "main"
                   ">");
      message += F("Connecting to: ");
      message += webServer.arg("ssid");
      message += F(" Failed");
      message += F("<br> Reason: ");
      if (WiFi.status() == 1)
      {
        //SSID not found!
        message += F("SSID not found!");
      }
      if (WiFi.status() == 4)
      {
        //Password failed!
        message += F("Password not accepted1");
      }
      message += F("<br><br><a href="
                   "setup"
                   ">Reenter network settings</a>");
    }
  }
  message += F("</div>");
  message += F("</body></html>");
  webServer.send(200, F("text/html"), message);
  digitalWrite(led, 1);
}

void handleIpSetup()
{
  if (!webServer.authenticate(config.username, config.userpass))
    return webServer.requestAuthentication();

  digitalWrite(led, 0);

  String message = F("<html>");

  if (webServer.method() == HTTP_POST)
  {
    //save
    message += htmlHead(F("IP Config"), 2);
    message += F("<body>");
    message += topMenu(3);
    message += F("<div class="
                 "main"
                 ">");
    config.setDevicename(webServer.arg("name"));
    if (webServer.arg("dhcp") == "dhcp")
    {
      //dhcp mode
      config.setDhcp(true);
      config.setIpAddress({0, 0, 0, 0});
      config.setSubnet({0, 0, 0, 0});
      config.setGateway({0, 0, 0, 0});
      config.setDns0({0, 0, 0, 0});
      config.setDns1({0, 0, 0, 0});
    }
    else
    {
      //manual IP
      config.setDhcp(false);
      IPAddress tempIp;

      String fieldName = "ip";
      for (byte i = 0; i < 4; i++)
      {
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setIpAddress(tempIp);

      fieldName = "sn";
      for (byte i = 0; i < 4; i++)
      {
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setSubnet(tempIp);

      fieldName = "gw";
      for (byte i = 0; i < 4; i++)
      {
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setGateway(tempIp);

      fieldName = "ns0";
      for (byte i = 0; i < 4; i++)
      {
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setDns0(tempIp);

      fieldName = "ns1";
      for (byte i = 0; i < 4; i++)
      {
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setDns1(tempIp);
    }
  }
  else
  {
    message += htmlHead(F("IP Config"));
    message += F("<body>");
    message += topMenu(9);
    message += F("<div class=\"main\">\n");
    message += F("<form action=\"/ipconfig\" method=\"post\">\n");
    message += F("<table>\n");
    message += F("<tr>\n");

    message += F("<td>Device name</td>\n");
    message += F("<td><input type=\"text\" name=\"name\" pattern=\"[a-zA-Z0-9\\-_ ]{1,32}\" title=\"Maximum length is 32, only alphanumeric characters and space, -,_\" value=\"");

    message += config.devicename;
    message += F("\"></td>\n</tr>\n");
    message += F("<tr>\n");
    message += F("<td>DHCP</td>\n");
    message += F("<td><input type=\"checkbox\" name=\"dhcp\" value=\"dhcp\" onchange=\"checkdhcp()\"");
    if (config.dhcp == true)
    {
      message += F(" checked");
    }
    message += F("></td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>IP address</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++)
    {

      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"ip");
      message += i;
      message += F("\" value=\"");
      if (config.dhcp == true)
      {
        message += WiFi.localIP()[i];
      }
      else
      {
        message += config.ip[i];
      }
      message += F("\">");
      if (i < 3)
      {
        message += F(" . ");
      }
    }
    message += F("</td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>Subnet mask</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++)
    {

      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"sn");
      message += i;
      message += F("\" value=\"");
      if (config.dhcp == true)
      {
        message += WiFi.subnetMask()[i];
      }
      else
      {
        message += config.sn[i];
      }
      message += F("\">");
      if (i < 3)
      {
        message += F(" . ");
      }
    }
    message += F("</td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>Gateway</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++)
    {

      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"gw");
      message += i;
      message += F("\" value=\"");
      if (config.dhcp == true)
      {
        message += WiFi.gatewayIP()[i];
      }
      else
      {
        message += config.gw[i];
      }

      message += F("\">");
      if (i < 3)
      {
        message += F(" . ");
      }
    }
    message += F("</td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>DNS 1</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++)
    {

      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"ns0");
      message += i;
      message += F("\" value=\"");

      if (config.dhcp == true)
      {
        message += WiFi.dnsIP(0)[i];
      }
      else
      {
        message += config.ns0[i];
      }

      message += F("\">");
      if (i < 3)
      {
        message += F(" . ");
      }
    }
    message += F("</td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>DNS 2</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++)
    {

      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"ns1");
      message += i;
      message += F("\" value=\"");
      if (config.dhcp == true)
      {
        message += WiFi.dnsIP(1)[i];
      }
      else
      {
        message += config.ns1[i];
      }
      message += F("\">");
      if (i < 3)
      {
        message += F(" . ");
      }
    }

    message += F("</td>\n</tr>\n");
    message += F("<tr>\n<td colspan='2' class='bcell'>");
    message += F("<button type='submit' value='Submit'>Save</button>&ensp;");
    message += F("<button type='reset' value='Reset'>Clear</button>");
    message += F("</td>\n</tr>\n</table>\n");
    message += F("</form>\n");
    message += F("<script>\n");
    message += F("function checkdhcp() {\n");
    message += F("if (document.getElementsByName('dhcp')[0].checked){\n");

    message += F("document.getElementsByName('ip0')[0].disabled = true;\n");
    message += F("document.getElementsByName('ip1')[0].disabled = true;\n");
    message += F("document.getElementsByName('ip2')[0].disabled = true;\n");
    message += F("document.getElementsByName('ip3')[0].disabled = true;\n");
    message += F("document.getElementsByName('sn0')[0].disabled = true;\n");
    message += F("document.getElementsByName('sn1')[0].disabled = true;\n");
    message += F("document.getElementsByName('sn2')[0].disabled = true;\n");
    message += F("document.getElementsByName('sn3')[0].disabled = true;\n");
    message += F("document.getElementsByName('gw0')[0].disabled = true;\n");
    message += F("document.getElementsByName('gw1')[0].disabled = true;\n");
    message += F("document.getElementsByName('gw2')[0].disabled = true;\n");
    message += F("document.getElementsByName('gw3')[0].disabled = true;\n");
    message += F("document.getElementsByName('ns00')[0].disabled = true;\n");
    message += F("document.getElementsByName('ns01')[0].disabled = true;\n");
    message += F("document.getElementsByName('ns02')[0].disabled = true;\n");
    message += F("document.getElementsByName('ns03')[0].disabled = true;\n");
    message += F("document.getElementsByName('ns10')[0].disabled = true;\n");
    message += F("document.getElementsByName('ns11')[0].disabled = true;\n");
    message += F("document.getElementsByName('ns12')[0].disabled = true;\n");
    message += F("document.getElementsByName('ns13')[0].disabled = true;\n");

    message += F("} else {\n");
    message += F("document.getElementsByName('ip0')[0].disabled = false;\n");
    message += F("document.getElementsByName('ip1')[0].disabled = false;\n");
    message += F("document.getElementsByName('ip2')[0].disabled = false;\n");
    message += F("document.getElementsByName('ip3')[0].disabled = false;\n");
    message += F("document.getElementsByName('sn0')[0].disabled = false;\n");
    message += F("document.getElementsByName('sn1')[0].disabled = false;\n");
    message += F("document.getElementsByName('sn2')[0].disabled = false;\n");
    message += F("document.getElementsByName('sn3')[0].disabled = false;\n");
    message += F("document.getElementsByName('gw0')[0].disabled = false;\n");
    message += F("document.getElementsByName('gw1')[0].disabled = false;\n");
    message += F("document.getElementsByName('gw2')[0].disabled = false;\n");
    message += F("document.getElementsByName('gw3')[0].disabled = false;\n");
    message += F("document.getElementsByName('ns00')[0].disabled = false;\n");
    message += F("document.getElementsByName('ns01')[0].disabled = false;\n");
    message += F("document.getElementsByName('ns02')[0].disabled = false;\n");
    message += F("document.getElementsByName('ns03')[0].disabled = false;\n");
    message += F("document.getElementsByName('ns10')[0].disabled = false;\n");
    message += F("document.getElementsByName('ns11')[0].disabled = false;\n");
    message += F("document.getElementsByName('ns12')[0].disabled = false;\n");
    message += F("document.getElementsByName('ns13')[0].disabled = false;\n");
    message += F("}\n");
    message += F("}\n");

    message += F("</script>\n");
    message += F("<script>");
    message += F("document.onload = checkdhcp();");
    message += F("</script>\n");
  }

  message += F("</div>\n");
  message += F("</body>\n</html>");

  webServer.send(200, F("text/html"), message);
  digitalWrite(led, 1);
}

void handleAbout()
{
  digitalWrite(led, 0);
  String message = F("<html>\n");
  message += htmlHead(F("About"), 10);
  message += F("<body>\n");
  message += topMenu(99);
  message += F("<div class="
               "main"
               ">\n");

  message += F("<h1>About</h1>\n");
  message += F("<p>Created by R. de Weerd");
  message += F("</p>\n");
  message += F("</div>\n");
  message += F("</body>\n</html>\n");

  webServer.send(200, F("text/html"), message);
  digitalWrite(led, 1);
}

void handleInstall()
{
#define pageDevicename 1
#define pageAccount 2
#define pageWifi 3
#define pageDhcp 4
#define pageNetwork 5
#define pageTime 6
#define pageSave 7

#define pageWifiScan 101
#define pageWifiLogin 102

  digitalWrite(led, 0);
  int page = webServer.arg("page").toInt();
  int lastpage = 7;
  bool errorfound = false;
  String errormsg = "";
  String message = "";
  String err_dev;
  String err_user;
  String err_pass;

  if (webServer.method() == HTTP_POST)
  {
    //save action
    page--;
    if (page == 0)
    {
      page++;
    }
    else if (page == pageDevicename)
    {
      String devname = webServer.arg("devicename");
      devname.trim();
      
      String err_dev = config.checkDevicename(devname);
      if (err_dev == "")
      {
        //name ok, save
        config.setDevicename(devname);
        page++;
      }
    }
    else if (page == pageAccount)
    {
      String usrname = webServer.arg("username");
      usrname.trim();

      err_user = config.checkUsername(usrname);
      err_pass = config.checkUserpass(webServer.arg("pwd1"), webServer.arg("pwd2"));


      if ((err_user == "") && (err_pass == ""))
      {
        //Data is ok saving
        config.setUsername(usrname);
        config.setUserpass(webServer.arg("pwd1"));
        page = 101;
      }
    }
    else if (page == pageWifi)
    {
      //save
      WiFi.mode(WIFI_AP_STA);
      char _ssid[webServer.arg("ssid").length() + 1];
      webServer.arg("ssid").toCharArray(_ssid, webServer.arg("ssid").length() + 1);

      char _pass[webServer.arg("pass").length() + 1];
      webServer.arg("pass").toCharArray(_pass, webServer.arg("pass").length() + 1);

      WiFi.begin(_ssid, _pass);
      page = 102;
    }
    else if (page == pageDhcp)
    {
      page++;
      if (webServer.arg("dhcp") == "dhcp")
      {
        //dhcp mode
        config.setDhcp(true);
        config.setIpAddress({0, 0, 0, 0});
        config.setSubnet({0, 0, 0, 0});
        config.setGateway({0, 0, 0, 0});
        config.setDns0({0, 0, 0, 0});
        config.setDns1({0, 0, 0, 0});
        page++;
      }
      else
      {
        //manual IP
        config.setDhcp(false);
      }
    }
    else if (page == pageNetwork)
    {
      IPAddress tempIp;

      String fieldName = "ip";
      for (byte i = 0; i < 4; i++)
      {
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setIpAddress(tempIp);

      fieldName = "sn";
      for (byte i = 0; i < 4; i++)
      {
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setSubnet(tempIp);

      fieldName = "gw";
      for (byte i = 0; i < 4; i++)
      {
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setGateway(tempIp);

      fieldName = "ns0";
      for (byte i = 0; i < 4; i++)
      {
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setDns0(tempIp);

      fieldName = "ns1";
      for (byte i = 0; i < 4; i++)
      {
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setDns1(tempIp);
      page++;
    }
    else if (page == pageTime)
    {
      config.setNtpServer(webServer.arg("ntpserver"));
      config.setTimeZone((webServer.arg("tz").toInt()) * 60);
      config.setDst((webServer.arg("dstoffset").toInt()) * 60);

      if (webServer.arg("autodst") == "autodst")
      {
        config.setAutoDst(true);
      }
      else
      {
        config.setAutoDst(false);
      }
      page++;
    }
    else if (page == pageSave)
    {
      String url;
      String content = F("<h1>Rebooting...</h1>");
      content += F("After the reboot you can manage your device on this address:<br>");

      if (config.dhcp)
      {
        url = "http://" + String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(WiFi.localIP()[3]) + "/";
      }
      else
      {
        url = "http://" + String(config.ip[0]) + "." + String(config.ip[1]) + "." + String(config.ip[2]) + "." + String(config.ip[3]) + "/";
      }

      content += "<a href =\"" + url + "\">" + url + "</a>";

      message = buildInstallPage(htmlHead("Install - rebooting", 1, url), lastpage + 1, lastpage, content, errormsg, false, false, false);
      webServer.send(200, F("text/html"), message);
      digitalWrite(led, 1);

      Serial.println(F("Rebooting"));
      for (int i = 0; i < 40; i++)
      {
        delay(100);
        webServer.handleClient();
      }
      Serial.println(F("Rebooting"));
      ESP.restart();
    }
  }

  if (page == pageWifiLogin)
  {
    //connection to wifi
    if (WiFi.status() == 6)
    {
      //still connecting
      String content = F("<h1>Connecting to Wifi..</h1>");
      message = buildInstallPage(htmlHead("Install - Step 4", 1, "/install?page=" + String(pageWifiLogin)), pageWifi, lastpage, content, errormsg, false, false, false);
    }
    else if (WiFi.status() == 3)
    {
      //connected!
      WiFi.setAutoReconnect(true);
      String content = F("<h1>Connected to Wifi!</h1>");
      message = buildInstallPage(htmlHead("Install - Step 4", 1, "/install?page=4"), page, lastpage, content, errormsg, true);
    }
    else
    {
      Serial.println("WE1");
      Serial.println(WiFi.status());
      // wifi error
      page = 3;
      if (WiFi.status() == 1)
      {
        //SSID not found!
        errormsg = F("SSID not found!");
      }
      if (WiFi.status() == 4)
      {
        //Password failed!
        errormsg = F("Password not accepted!");
      }
    }
  }

  if (page == pageWifiScan)
  {
    // Wifi scanning

    int wifirefresh = webServer.arg("wifirefresh").toInt();
    if (wifirefresh > 0)
    {
      WiFi.scanDelete();
    }

    if (WiFi.scanComplete() == -2)
    {
      //start wifiscan
      WiFi.scanNetworks(true);
    }
    if (WiFi.scanComplete() == -1)
    {
      //wifiscan in progress
      String content = F("<h1>Wifi scan in progress..</h1>");

      message = buildInstallPage(htmlHead("Install - Step 3", 1, "/install?page=" + String(pageWifiScan)), pageWifi, lastpage, content, errormsg, false, false, false);
    }
    else if (WiFi.scanComplete() == 0)
    {
      //wifiscan complete no networks found

      String content = "No networks in range <a href =\"/install?page=" + String(pageWifiScan) + "&wifirefresh=1\">Refresh Wifi networks. </a>";
      message = buildInstallPage(htmlHead("Install - Step " + String(pageWifi)), pageWifi, lastpage, content, errormsg, false, false, false);
    }
    else
    {
      //wifiscan complete networks found, next page

      String content = F("Networks found, processing please wait.");
      message = buildInstallPage(htmlHead("Install - Step " + String(pageWifi), 1, "/install?page=" + String(pageWifi)), pageWifi, lastpage, content, errormsg, false, false, false);
    }
  }

  if (page == pageSave)
  {
    //Save and reboot message
    String url;
    String content = F("<h1>Save and reboot</h1>");
    content += F("After the reboot you can manage your device on this address:<br>");

    if (config.dhcp)
    {
      url = "http://" + String(WiFi.localIP()[0]) + "." + String(WiFi.localIP()[1]) + "." + String(WiFi.localIP()[2]) + "." + String(WiFi.localIP()[3]) + "/";
    }
    else
    {
      url = "http://" + String(config.ip[0]) + "." + String(config.ip[1]) + "." + String(config.ip[2]) + "." + String(config.ip[3]) + "/";
    }

    content += "<a href =\"" + url + "\">" + url + "</a>";

    message = buildInstallPage(htmlHead("Install - Step " + String(pageSave)), pageSave, lastpage, content, errormsg, true, false, true);
  }

  if (page == pageTime)
  {
    //Ipconfig
    String content = F("Please enter the time configuration.<br><br>");
    content += F("<table>\n");
    content += F("<tr>\n");
    content += F("<td>NTP time server:</td>\n");
    content += F("<td><input type=\"text\" name=\"ntpserver\" value=\"");
    content += config.ntpserver;
    content += F("\"></td>\n");
    content += F("</tr>\n");

    content += F("<tr>\n");
    content += F("<td>Timezone offset in minutes:</td>\n");
    content += F("<td><input type=\"number\" style=\"width: 50px;\" max=800 min=-800 name=\"tz\" value=\"");
    content += (config.timezone / 60);
    content += F("\"></td>");
    content += F("</tr>\n");

    content += F("<tr>\n");
    content += F("<td>DST offset in minutes:</td>\n");
    content += F("<td><input type=\"number\" style=\"width: 50px;\" max=800 min=-800 name=\"dstoffset\" value=\"");
    content += (config.dstoffset / 60);
    content += F("\"></td>");
    content += F("</tr>\n");

    content += F("<tr>\n");
    content += F("<td>Auto DST (Europe only):</td>\n");
    if (config.autodst)
    {
      content += F("<td><input type=\"checkbox\" name=\"autodst\" value=\"autodst\" checked></td>");
    }
    else
    {
      content += F("<td><input type=\"checkbox\" name=\"autodst\" value=\"autodst\"></td>");
    }

    content += F("</tr>\n");

    content += F("</table>\n");

    message = buildInstallPage(htmlHead("Install - Step " + String(pageNetwork)), pageTime, lastpage, content, errormsg);
  }

  if (page == pageNetwork)
  {
    if (config.dhcp)
    {
      page = pageDhcp;
    }
    else
    {
      //Ipconfig
      String content = F("Fill in the IP configuration");

      content += F("<table>");

      content += F("<tr>");
      content += F("<td>IP address:</td>\n");
      content += F("<td>");
      content += ipInputFields(WiFi.localIP(), "ip");
      content += F("</td>\n");
      content += F("</tr>");

      content += F("<tr>");
      content += F("<td>Subnetmask:</td>\n");
      content += F("<td>");
      content += ipInputFields(WiFi.subnetMask(), "sn");
      content += F("</td>\n");
      content += F("</tr>");
      content += F("<tr>");
      content += F("<td>Default gateway:</td>\n");
      content += F("<td>");
      content += ipInputFields(WiFi.gatewayIP(), "gw");
      content += F("</td>\n");
      content += F("</tr>");

      content += F("<tr>");

      content += F("<td>DNS server 1:</td>\n");
      content += F("<td>");
      content += ipInputFields(WiFi.dnsIP(0), "ns0");
      content += F("</td>\n");

      content += F("</tr>");
      content += F("<tr>");

      content += F("<td>DNS server 2:</td>\n");
      content += F("<td>");
      content += ipInputFields(WiFi.dnsIP(1), "ns1");
      content += F("</td>\n");
      content += F("</tr>");

      content += F("</table>");
      message = buildInstallPage(htmlHead("Install - Step " + String(pageNetwork)), pageNetwork, lastpage, content, errormsg);
    }
  }

  if (page == pageDhcp)
  {
    //dhcp question
    String content = F("Check the box below if you want to use DHCP. Usage of DCHP is not recommended!<br><br>");
    content += F("<table><tr>");
    content += F("<td>Use DHCP:</td>\n");
    content += F("<td><input type=\"checkbox\" name=\"dhcp\" value=\"dhcp\"></td>");
    content += F("</tr></table>");

    message = buildInstallPage(htmlHead("Install - Step " + String(pageDhcp)), pageDhcp, lastpage, content, errormsg);
  }

  if (page == pageWifi)
  {
    // Page to enter wifi configuration
    String content = F("Wifi settings");
    content += F("<table>");
    content += F("<tr>");
    content += F("<td>Network SSID:</td><td>");
    content += F(" <select name ="
                 "ssid"
                 ">");
    for (int i = 0; i < WiFi.scanComplete(); i++)

    {
      content += F("<option value="
                   "");
      content += WiFi.SSID(i);
      if (WiFi.SSID(i) == WiFi.SSID())
      {
        content += F(""
                     " selected>");
      }
      else
      {
        content += F(""
                     ">");
      }
      content += WiFi.SSID(i);
      content += F("</option>");
    }
    content += F("</select>");
    content += F("</td></tr><tr>");
    content += F("<td>Password:</td><td>");

    content += F("<input type="
                 "password"
                 " name="
                 "pass"
                 ">");
    content += F("</td></tr></table>");
    content += F("<a href =\"/install?page=101&wifirefresh=1\">Refresh Wifi networks.</a>");

    message = buildInstallPage(htmlHead("Install - Step " + String(pageWifi)), page, lastpage, content, errormsg);
  }

  if (page == pageAccount)
  {
    //Account name and password
    String content = F("This is the account needed to change your settings later.<br><br>");
    content += F("<table>\n");
    content += F("<tr>\n");
    content += F("<td>Username:</td>\n");
    content += F("<td><input type=\"text\" name=\"username\" value=\"");
    content += config.username;
    content += F("\"></td>\n");
    content += F("</tr>\n");

    content += F("<tr>\n");
    content += F("<td>Password:</td>\n");
    content += F("<td><input type=\"password\" name=\"pwd1\"></td>\n");
    content += F("</tr>\n");

    content += F("<tr>\n");
    content += F("<td>Reypte Password:</td>\n");
    content += F("<td><input type=\"password\" name=\"pwd2\"></td>\n");
    content += F("</tr>\n");

    content += F("</table>\n");

    message = buildInstallPage(htmlHead("Install - Step " + String(pageAccount)), pageAccount, lastpage, content, err_user + " " + err_pass);
  }

  if (page == pageDevicename)
  {
    // Device name

    String content = F("Please enter these basic settings to setup the device.<br><br>");
    content += F("<table>\n");
    content += F("<tr>\n");
    content += F("<td>Device name:</td>\n");
    content += F("<td><input type=\"text\" name=\"devicename\" value=\"");
    content += config.devicename;
    content += F("\"></td>\n");
    content += F("</tr>\n");
    content += F("</table>\n");

    message = buildInstallPage(htmlHead("Install - Step " + String(pageDevicename)), pageDevicename, lastpage, content, err_dev);
  }

  if (page == 0)
  {
    //Information

    String content = F("<H2>Welcome to your new WiFi device.</H2>");
    content += F("Please follow this wizzard and enter these basic settings to install the device.<br>");
    message = buildInstallPage(htmlHead("Install - Intro"), page, lastpage, content, errormsg, false, false);
  }
  webServer.send(200, F("text/html"), message);
  digitalWrite(led, 1);
}

void handleNetworkSettings()
{
  if (!webServer.authenticate(config.username, config.userpass))
    return webServer.requestAuthentication();

  digitalWrite(led, 0);

  String message = F("<html>");

  if (webServer.method() == HTTP_POST)
  {
    //save
  }
  else
  {
    //showpage
    message += htmlHead(F("Network settings"));
    message += F("<body>");
    message += topMenu(9);
    message += F("<div class=\"main\">\n");
    message += F("<form action=\"/networkconfig\" method=\"post\">\n");
    message += F("<table>\n");
    message += F("<tr>\n");

    message += F("<td>WiFi SSID</td>\n");
    message += F("<td><input type=\"text\" name=\"wifissid\" value=\"");
    message += WiFi.SSID();
    message += F("\"></td>\n</tr>\n");

    message += F("<tr>\n");
    message += F("<td>WiFi password</td>\n");
    message += F("<td><input type=\"password\" name=\"wifipass\" ></td>\n");
    message += F("</tr>\n");

    message += F("<tr>\n");
    message += F("<td>Retype password</td>\n");
    message += F("<td><input type=\"password\" name=\"wifipass2\" ></td>\n");
    message += F("</tr>\n");

    message += F("</table>\n");

    message += F("<table>\n");
    message += F("<tr>\n");
    message += F("<td>DHCP</td>\n");
    message += F("<td><input type=\"checkbox\" name=\"dhcp\" value=\"dhcp\" onchange=\"checkdhcp()\"");
    if (config.dhcp == true)
    {
      message += F(" checked");
    }
    message += F("></td>\n");
    message += F("</tr>\n");
    message += F("</table>\n");

    message += F("<div id=\"ipinfo\">\n");

    message += F("<table>\n");

    message += F("<tr>\n");
    message += F("<td>IP address</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++)
    {

      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"ip");
      message += i;
      message += F("\" value=\"");
      if (config.dhcp == true)
      {
        message += WiFi.localIP()[i];
      }
      else
      {
        message += config.ip[i];
      }
      message += F("\">");
      if (i < 3)
      {
        message += F(" . ");
      }
    }
    message += F("</td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>Subnet mask</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++)
    {

      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"sn");
      message += i;
      message += F("\" value=\"");
      if (config.dhcp == true)
      {
        message += WiFi.subnetMask()[i];
      }
      else
      {
        message += config.sn[i];
      }
      message += F("\">");
      if (i < 3)
      {
        message += F(" . ");
      }
    }
    message += F("</td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>Gateway</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++)
    {

      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"gw");
      message += i;
      message += F("\" value=\"");
      if (config.dhcp == true)
      {
        message += WiFi.gatewayIP()[i];
      }
      else
      {
        message += config.gw[i];
      }

      message += F("\">");
      if (i < 3)
      {
        message += F(" . ");
      }
    }
    message += F("</td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>DNS 1</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++)
    {

      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"ns0");
      message += i;
      message += F("\" value=\"");

      if (config.dhcp == true)
      {
        message += WiFi.dnsIP(0)[i];
      }
      else
      {
        message += config.ns0[i];
      }

      message += F("\">");
      if (i < 3)
      {
        message += F(" . ");
      }
    }
    message += F("</td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>DNS 2</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++)
    {

      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"ns1");
      message += i;
      message += F("\" value=\"");
      if (config.dhcp == true)
      {
        message += WiFi.dnsIP(1)[i];
      }
      else
      {
        message += config.ns1[i];
      }
      message += F("\">");
      if (i < 3)
      {
        message += F(" . ");
      }
    }

    message += F("</td>\n</tr>\n");

    message += F("</table>\n");

    message += F("</div>\n");

    message += F("<table>\n");

    message += F("<tr>\n<td colspan='2' class='bcell'>");
    message += F("<button type='submit' value='Submit'>Save</button>&ensp;");
    message += F("<button type='reset' value='Reset'>Clear</button>");
    message += F("</td>\n</tr>\n</table>\n");
    message += F("</form>\n");
    message += F("<script>\n");
    message += F("function checkdhcp() {\n");
    message += F("var x = document.getElementById(\"ipinfo\");\n");
    message += F("if (document.getElementsByName('dhcp')[0].checked){\n");

    message += F("x.style.display = \"none\";\n");
    message += F("} else {\n");
    message += F("x.style.display = \"block\";\n");
    message += F("}\n");
    message += F("}\n");

    message += F("</script>\n");
    message += F("<script>");
    message += F("document.onload = checkdhcp();");
    message += F("</script>\n");
  }

  message += F("</div>\n");
  message += F("</body>\n</html>");

  webServer.send(200, F("text/html"), message);
  digitalWrite(led, 1);
}

void handleSettings()
{
  if (!webServer.authenticate(config.username, config.userpass))
    return webServer.requestAuthentication();

  digitalWrite(led, 0);
#define cfgGeneral 0
#define cfgWifi 1
#define cfgNetwork 2
#define cfgTime 3

  int page = webServer.arg("page").toInt();
  String message = "";
  String err_dev;
  String err_user;
  String err_pass;

  bool save = false;

  if (webServer.method() == HTTP_POST)
  {
    //save
    save = true;

    if (page == cfgGeneral)
    {
      String devname = webServer.arg("devicename");
      String usrname = webServer.arg("username");
      String pwd1 = webServer.arg("pwd1");
      devname.trim();
      usrname.trim();
      pwd1.trim();

      err_dev = config.checkDevicename(devname);
      err_user = config.checkUsername(usrname);
      err_pass = "";

      if (pwd1.length() > 1)
      {
        err_pass = config.checkUserpass(webServer.arg("pwd1"), webServer.arg("pwd2"));
      }

      if (err_dev == "")
      {
        //name ok, save
        config.setDevicename(devname);
      }

      if (err_user == "0")
      {
        //Data is ok saving
        config.setUsername(usrname);
      }

      if ((err_pass == "") && (pwd1.length() > 1))
      {
        //Data is ok saving
        config.setUserpass(webServer.arg("pwd1"));
      }
    }
  }

  //showpage

  if (page == cfgGeneral)
  {
    String content;
    if (err_dev != ""){
      content += err_dev + "\n";
    }
    if (err_user != ""){
      content += err_user + "\n";
    }
    if (err_pass != ""){
      content += err_pass + "\n";
    }
    content += "<form action=\"/settings?page=" + String(page) + "\" method=\"post\">\n";
    content += F("<table>\n");
    content += F("<tr>\n");
    content += F("<td>Device name:</td>\n");
    content += F("<td><input type=\"text\" name=\"devicename\" value=\"");
    content += config.devicename;
    content += F("\"></td>\n");
    content += F("</tr>\n");
    content += F("<tr>\n");
    content += F("<td>Username:</td>\n");
    content += F("<td><input type=\"text\" name=\"username\" value=\"");
    content += config.username;
    content += F("\"></td>\n");
    content += F("</tr>\n");

    content += F("<tr>\n");
    content += F("<td>Password:</td>\n");
    content += F("<td><input type=\"password\" name=\"pwd1\"></td>\n");
    content += F("</tr>\n");

    content += F("<tr>\n");
    content += F("<td>Reypte Password:</td>\n");
    content += F("<td><input type=\"password\" name=\"pwd2\"></td>\n");
    content += F("</tr>\n");
    content += F("<tr>\n");
    content += F("<td colspan=2>");
    content += F("<table width=100%>\n<tr>\n");
    content += F("<td width=50% align='center'>");
    content += F("<button id='cancelbutton' type='reset' value='Reset'>Reset</button>");

    content += F("</td>\n");
    content += F("<td width=50% align='center'>");
    content += F("<button id='savebutton' type='submit' value='Submit'>Save</button>");
    content += F("</td>\n");

    content += F("</tr>\n");
    content += F("</table>\n");

    content += F("</td>\n");

    content += F("</tr>\n");

    content += F("</table>\n");
    content += F("</form>\n");

    message += buildConfigPage(page, "General Settings", content);
  }

  if (page == cfgWifi)
  {
    String content;
    content = "Wifi page";
    message += buildConfigPage(page, "General Settings", content);
  }
  if (page == cfgNetwork)
  {
    String content;
    content = "Network page";
    message += buildConfigPage(page, "General Settings", content);
  }
  if (page == cfgTime)
  {
    String content;
    content = "Time page";
    message += buildConfigPage(page, "General Settings", content);
  }

  webServer.send(200, F("text/html"), message);
  digitalWrite(led, 1);
}

void handleDebug()
{
  digitalWrite(led, 0);

  String message = "";

  Dir dir = SPIFFS.openDir("/cfg");
  while (dir.next())
  {
    File f = dir.openFile("r");
    message += (f.readString());
    f.close();
  }

  webServer.send(200, F("text/json"), message);
  digitalWrite(led, 1);
}

void initWebServer()
{

  if (installMode)
  {
    webServer.on("/css", handleCSSinstall);
    webServer.on("/install", handleInstall);
    webServer.onNotFound(handleNotFoundInstall);
  }
  else
  {
    webServer.on("/", handleRoot);
    webServer.on("/css", handleCSS);
    //webServer.on("/ip", handleWifiSetup);
    //    webServer.on("/network", handleNetworkSettings);
    webServer.on("/settings", handleSettings);
    //webServer.on("/ipconfig", handleIpSetup);

    //webServer.on ( "/connect", handleConnect );

    webServer.on("/dbg", handleDebug);
    webServer.onNotFound(handleNotFound);
  }

  webServer.begin();
}

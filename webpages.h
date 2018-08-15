


String ipToString(IPAddress ip){
  String s="";
  for (int i=0; i<4; i++)
    s += i  ? "." + String(ip[i]) : String(ip[i]);
  return s;
}



void handleRoot() {
  digitalWrite ( led, 0 );  
  time_t now;
  now = time(nullptr);
  String message = F("<html>");
  message += htmlHead(F("Home"), 10);
  message += F("<body>");
  message += htmlMenu(1);
  message += F("<div class=""main"">");

  
  message += F("<h1>Hello from ");
  message += config.devicename;
  message += F("!</h1>");
  message += F("<p>Date: ");
  message += ctime(&now);
  message += F("</p>");
  message += F("</div");
  message += F("</body></html>");

  webServer.send ( 200, F("text/html"), message);
  digitalWrite ( led, 1 );
}

void handleWifiSetup() {
  if(!webServer.authenticate(config.username, config.userpass))
    return webServer.requestAuthentication();
  digitalWrite ( led, 0 );
  String message = F("<html>");

  if (WiFi.scanComplete() == -2){
    //start wifiscan
    WiFi.scanNetworks(true);
  }    
  if (WiFi.scanComplete() == -1){
    //wifiscan in progress
    message += htmlHead(F("Setup"), 1);
    message += F("<body>");
    message += F("<div class=""main"">");
    message += F("WiFi Scan in progress....");

  } else {
    //wifiscan complete
    message += htmlHead(F("WiFi Config"));
    message += F("<body>");
    message += htmlMenu(2);
    message += F("<div class=""main"">");
    if (WiFi.scanComplete() == 0){
      message += F("No networks in range");
    } else {
      message += F("<form action=""/connect"" method=""post"">");
      message += F("<table>");
      message += F("<tr>");
      message += F("<td>Network SSID:</td><td>");
      message += F(" <select name =""ssid"">");
      for (int i = 0; i < WiFi.scanComplete(); i++) {
        message += F("<option value=""");
        message += WiFi.SSID(i);
        if (WiFi.SSID(i) == WiFi.SSID()){
          message += F(""" selected>");  
        } else {
          message += F(""">");  
        }
        message += WiFi.SSID(i);
        message += F("</option>");
      }
      message += F("</select>");
      message += F("</td></tr><tr>");
      message += F("<td>Password:</td><td>");
      
      message += F("<input type=""password"" name=""pass"">");
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
  webServer.send ( 200, F("text/html"), message );
  digitalWrite ( led, 1 );
}

void handleConnect() {
  digitalWrite ( led, 0 );
  String message = F("<html>");
  if (webServer.method() == HTTP_POST){
    //save
    WiFi.mode(WIFI_AP_STA);
    char _ssid[webServer.arg("ssid").length()+1];
    webServer.arg("ssid").toCharArray(_ssid, webServer.arg("ssid").length()+1);
    
    char _pass[webServer.arg("pass").length()+1];
    webServer.arg("pass").toCharArray(_pass, webServer.arg("pass").length()+1);
    
    WiFi.begin(_ssid, _pass);
    //WiFi.begin(webServer.arg("ssid"), webServer.arg("pass"));
  }

  if (WiFi.status() == 6){
    //still connecting
    message += htmlHead(F("Connect"), 1);
    message += F("<body>");
    message += F("<div class=""main"">");
    message += F("Connecting...");
  } else {
    if (WiFi.status() == 3){  
      //connected!
      WiFi.setAutoReconnect(true);
      message += htmlHead(F("Connected"));
      message += F("<body>");
      message += F("<div class=""main"">");
      message += F("Connected to: ");
      message += WiFi.SSID();
      message += F("<br><br>");
      message += F("Reboot the device and connect to IP address: ");
      message += ipToString(WiFi.localIP());
    } else {
      message += htmlHead(F("Error"));
      message += F("<body>");
      message += F("<div class=""main"">");
      message += F("Connecting to: ");
      message += webServer.arg("ssid");
      message += F(" Failed");
      message += F("<br> Reason: ");
      if (WiFi.status() == 1){
        //SSID not found!
        message += F("SSID not found!");
      }
      if (WiFi.status() == 4){
        //Password failed!
        message += F("Password not accepted1");
      }
      message += F("<br><br><a href=""setup"">Reenter network settings</a>");
    }    
  }
  message += F("</div>");
  message += F("</body></html>");
  webServer.send ( 200, F("text/html"), message );
  digitalWrite ( led, 1 );
}

void handleTimeSetup() {
  if(!webServer.authenticate(config.username, config.userpass))
    return webServer.requestAuthentication();
  digitalWrite ( led, 0 );
  time_t now;
  now = time(nullptr);
  String message = F("<html>");
  message += htmlHead(F("Time"), 10);
  message += F("<body>");
  message += htmlMenu(4);
  message += F("<div class=""main"">");
  message += F("<h1>NTP Setup</h1>");
  message += F("<p>Date: ");
  message += ctime(&now);
  message += F("</p>");
  message += F("</div>");
  message += F("</body></html>");

  webServer.send ( 200, F("text/html"), message);
  digitalWrite ( led, 1 );
}

void handleIpSetup() {
  if(!webServer.authenticate(config.username, config.userpass))
    return webServer.requestAuthentication();

  digitalWrite ( led, 0 );

  String message = F("<html>");

  if (webServer.method() == HTTP_POST){
    //save
    message += htmlHead(F("IP Config"),2);
    message += F("<body>");
    message += htmlMenu(3);
    message += F("<div class=""main"">");    
    config.setDevicename(webServer.arg("name"));
    if (webServer.arg("dhcp") == "dhcp"){
      //dhcp mode
      config.setDhcp(true);
      config.setIpAddress({0,0,0,0});
      config.setSubnet({0,0,0,0});
      config.setGateway({0,0,0,0});
      config.setDns0({0,0,0,0});
      config.setDns1({0,0,0,0});
    } else {
      //manual IP
      config.setDhcp(false);
      IPAddress tempIp;
      
      String fieldName = "ip";
      for (byte i = 0; i < 4; i++ ){
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setIpAddress(tempIp);

      fieldName = "sn";
      for (byte i = 0; i < 4; i++ ){
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setSubnet(tempIp);
       
      fieldName = "gw";
      for (byte i = 0; i < 4; i++ ){
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setGateway(tempIp);

      fieldName = "ns0";
      for (byte i = 0; i < 4; i++ ){
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setDns0(tempIp);        

      fieldName = "ns1";
      for (byte i = 0; i < 4; i++ ){
        tempIp[i] = webServer.arg(fieldName + i).toInt();
      }
      config.setDns1(tempIp);        
      
    }
      
  } else {
    message += htmlHead(F("IP Config"));
    message += F("<body>");
    message += htmlMenu(3);
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
    if (config.dhcp == true){
      message += F(" checked");
    }
    message += F("></td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>IP address</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++ ){
  
      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"ip");
      message += i;
      message += F("\" value=\"");
      if (config.dhcp == true){
        message += WiFi.localIP()[i];
      } else {
        message += config.ip[i];
      }
      message += F("\">");
      if (i < 3){
        message += F(" . ");  
      }
    }
    message += F("</td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>Subnet mask</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++ ){
  
      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"sn");
      message += i;
      message += F("\" value=\"");
      if (config.dhcp == true){
        message += WiFi.subnetMask()[i];
      } else {
        message += config.sn[i];
      }            
      message += F("\">");
      if (i < 3){
        message += F(" . ");  
      }
    }
    message += F("</td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>Gateway</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++ ){
  
      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"gw");
      message += i;
      message += F("\" value=\"");
      if (config.dhcp == true){
        message += WiFi.gatewayIP()[i];
      } else {
        message += config.gw[i];
      }            

      message += F("\">");
      if (i < 3){
        message += F(" . ");  
      }
    }
    message += F("</td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>DNS 1</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++ ){
  
      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"ns0");
      message += i;
      message += F("\" value=\"");

      if (config.dhcp == true){
        message += WiFi.dnsIP(0)[i];
      } else {
        message += config.ns0[i];
      }            

      message += F("\">");
      if (i < 3){
        message += F(" . ");  
      }
    }
    message += F("</td>\n");
    message += F("</tr>\n");
    message += F("<tr>\n");
    message += F("<td>DNS 2</td>\n");
    message += F("<td>");
    for (byte i = 0; i < 4; i++ ){
  
      message += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"ns1");
      message += i;
      message += F("\" value=\"");
      if (config.dhcp == true){
        message += WiFi.dnsIP(1)[i];
      } else {
        message += config.ns1[i];
      }            
      message += F("\">");
      if (i < 3){
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

  webServer.send ( 200, F("text/html"), message);
  digitalWrite ( led, 1 );
}

void handleAbout() {
  digitalWrite ( led, 0 );
  String message = F("<html>\n");
  message += htmlHead(F("About"), 10);
  message += F("<body>\n");
  message += htmlMenu(3);
  message += F("<div class=""main"">\n");

  message += F("<h1>About</h1>\n");
  message += F("<p>Information about the program");
  message += F("</p>\n");
  message += F("</div>\n");
  message += F("</body>\n</html>\n");

  webServer.send ( 200, F("text/html"), message);
  digitalWrite ( led, 1 );
}


void processInstall() {
  digitalWrite ( led, 0 );
  String message = F("<html>");
  if (webServer.method() == HTTP_POST){
    //save
    WiFi.mode(WIFI_AP_STA);
    char _ssid[webServer.arg("ssid").length()+1];
    webServer.arg("ssid").toCharArray(_ssid, webServer.arg("ssid").length()+1);
    
    char _pass[webServer.arg("wifipwd").length()+1];
    webServer.arg("wifipwd").toCharArray(_pass, webServer.arg("wifipwd").length()+1);

    config.setUsername(webServer.arg("username"));
    config.setUserpass(webServer.arg("pwd1"));
    config.setDevicename(webServer.arg("devicename"));
    
    
    WiFi.begin(_ssid, _pass);
    //WiFi.begin(webServer.arg("ssid"), webServer.arg("pass"));
  }

  if (WiFi.status() == 6){
    //still connecting
    message += htmlHead(F("Connect"), 1);
    message += F("<body>");
    message += F("<div class=""main"">");
    message += F("Connecting...");
  } else {
    if (WiFi.status() == 3){  
      //connected!
      WiFi.setAutoReconnect(true);
      message += htmlHead(F("Connected"));
      message += F("<body>");
      message += F("<div class=""main"">");
      message += F("Connected to: ");
      message += WiFi.SSID();
      message += F("<br><br>");
      message += F("Reboot the device and connect to IP address: ");
      message += ipToString(WiFi.localIP());
    } else {
      message += htmlHead(F("Error"));
      message += F("<body>");
      message += F("<div class=""main"">");
      message += F("Connecting to: ");
      message += webServer.arg("ssid");
      message += F(" Failed");
      message += F("<br> Reason: ");
      if (WiFi.status() == 1){
        //SSID not found!
        message += F("SSID not found!");
      }
      if (WiFi.status() == 4){
        //Password failed!
        message += F("Password not accepted1");
      }
      message += F("<br><br><a href=""/install"">Reenter network settings</a>");
    }    
  }
  message += F("</div>");
  message += F("</body></html>");
  
  webServer.send ( 200, F("text/html"), message );
  digitalWrite ( led, 1 );
}



void handleInstall_Old() {
  digitalWrite ( led, 0 );
  String message = F("<html>\n");
 
  message += htmlHead(F("Setup"));
  message += F("<body>\n");
  message += F("<div class=\"main\">\n");
  message += F("<form action=\"/install_save\" method=\"post\">\n");

  message += F("<div class=\"setup\">\n");
  message += F("<table>\n");
  message += F("<tr>\n");
  message += F("<td>Device name:</td>\n");
  message += F("<td><input type=\"text\" name=\"devicename\" onchange=\"checkdevname()\" value=\"");
  message += config.devicename;
  message += F("\"></td>\n");
  message += F("<td id=\"devname_error\" class=\"error\"></td>\n");
  message += F("</tr>\n");
  message += F("</table>\n");
  message += F("</div>\n");

  message += F("<div class=\"setup\">\n");
  message += F("<table>\n");
  message += F("<tr>\n");
  message += F("<td>Username:</td>\n");
  message += F("<td><input type=\"text\" name=\"username\" onchange=\"checkusername()\" value=\"admin\"></td>\n");
  message += F("<td id=\"name_error\" class=\"error\"></td>\n");
  message += F("</tr>\n");
  message += F("<tr>\n");
  message += F("<td>Password:</td>\n");
  message += F("<td><input type=\"password\" name=\"pwd1\" onchange=\"checkpass()\"></td>\n");
  message += F("<td id=\"pwd1_error\" class=\"error\"></td>\n");
  message += F("</tr>\n");
  
  message += F("<tr>\n");
  message += F("<td>Reypte Password:</td>\n");
  message += F("<td><input type=\"password\" name=\"pwd2\" onchange=\"checkpass()\"></td>\n");
  message += F("<td id=\"pwd2_error\" class=\"error\"></td>\n");
  message += F("</tr>\n");
  
  message += F("</table>\n");
  message += F("</div>\n");

  message += F("<div class=\"setup\">\n");
  message += F("<table>\n");
  
  message += F("<tr>\n");
  message += F("<td colspan=\"3\">WiFi Setup:</td>\n");
  message += F("</tr>\n");
  
  message += F("<tr>\n");
  message += F("<td>SSID:</td>\n");
  message += F("<td><input type=\"text\" name=\"ssid\" onchange=\"checkssid()\"></td>\n");
  message += F("<td id=\"ssid_error\" class=\"error\"></td>\n");
  message += F("</tr>\n");

  message += F("<tr>\n");
  message += F("<td>WiFi Password:</td>\n");
  message += F("<td><input type=\"password\" name=\"wifipwd\"></td>\n");
  message += F("<td>&nbsp;</td>\n");
  message += F("</tr>\n");
 
  message += F("</table>\n");
  message += F("</div>\n");
  

  
  message += F("<table>\n");
  message += F("<tr>\n<td class='bcell'>");
  message += F("<button id='savebutton' type='submit' value='Submit'>Save</button>&ensp;");
  message += F("<button type='reset' value='Reset'>Clear</button>");
  message += F("</td>\n</tr>\n</table>\n");
  message += F("</form>\n");
  message += F("<script>\n");
    message += F("var devnameOK = true;");
    message += F("var usernameOK = true;");
    message += F("var passOK = false;");
    message += F("var ssidOK = false;");

    message += F("function checkpass() {\n");  
      message += F("var errormsg = \"\"\n");      
      message += F("passOK = false;\n");
      message += F("pwd1 = document.getElementsByName(\"pwd1\")[0].value;\n");
      message += F("pwd2 = document.getElementsByName(\"pwd2\")[0].value;\n");
      message += F("if (pwd1.length <8){\n");
        message += F("errormsg = \"Too short\";\n");
      message += F("} else {\n");
        message += F("if (pwd1 != pwd2){\n");
          message += F("errormsg = \"Passwords don't match\";\n");
        message += F("} else {\n");
          message += F("passOK = true;\n");
        message += F("}\n");        
      message += F("}\n");
      message += F("document.getElementById(\"pwd1_error\").innerHTML = errormsg;\n");
      message += F("checkfields();");
    message += F("}\n");

    message += F("function checkdevname() {\n");  
      message += F("var errormsg = \"\"\n");
      message += F("var re = /[\\w -]+/g\n");
      message += F("var input = document.getElementsByName(\"devicename\")[0].value.trim();\n");
      message += F("var regex = input.replace(re, '')\n");
      message += F("devnameOK = false;\n");
      message += F("if (regex!=\"\"){\n");
        message += F("errormsg = \"(\" + regex + \") Character not allowed\";\n");
      message += F("} else {\n");
        message += F("if (input.length < 1){\n");
          message += F("errormsg = \"Name is too short\";\n");
        message += F("} else if (input.length > 32){\n");
          message += F("errormsg = \"Name is too long\";\n");
        message += F("} else {\n");
          message += F("devnameOK = true;\n");
        message += F("}\n");
      message += F("}\n");
      message += F("document.getElementById(\"devname_error\").innerHTML = errormsg;\n");
      message += F("checkfields();");
    message += F("}\n");

    message += F("function checkusername() {\n");  
      message += F("var errormsg = \"\"\n");
      message += F("var re = /[\\w-]+/g\n");
      message += F("var input = document.getElementsByName(\"username\")[0].value.trim();\n");
      message += F("var regex = input.replace(re, '')\n");
      message += F("usernameOK = false;\n");
      message += F("if (regex!=\"\"){\n");
        message += F("errormsg = \"(\" + regex + \") Character not allowed\";\n");
      message += F("} else {\n");
        message += F("if (input.length < 3){\n");
          message += F("errormsg = \"Name is too short\";\n");
        message += F("} else if (input.length > 32){\n");
          message += F("errormsg = \"Name is too long\";\n");
        message += F("} else {\n");
          message += F("usernameOK = true;\n");
        message += F("}\n");
      message += F("}\n");
      message += F("document.getElementById(\"name_error\").innerHTML = errormsg;\n");
      message += F("checkfields();");
    message += F("}\n");

    message += F("function checkssid() {\n");  
      message += F("var errormsg = \"\"\n");
      message += F("var re = /[\\w-]+/g\n");
      message += F("var input = document.getElementsByName(\"ssid\")[0].value.trim();\n");
      message += F("var regex = input.replace(re, '')\n");
      message += F("ssidOK = false;\n");
      message += F("if (regex!=\"\"){\n");
        message += F("errormsg = \"(\" + regex + \") Character not allowed\";\n");
      message += F("} else {\n");
        message += F("if (input.length < 1){\n");
          message += F("errormsg = \"SSID is too short\";\n");
        message += F("} else if (input.length > 32){\n");
          message += F("errormsg = \"SSID is too long\";\n");
        message += F("} else {\n");
          message += F("ssidOK = true;\n");
        message += F("}\n");
      message += F("}\n");
      message += F("document.getElementById(\"ssid_error\").innerHTML = errormsg;\n");
      message += F("checkfields();");
    message += F("}\n");    
    
    message += F("function checkfields() {\n");
      message += F("if (devnameOK && passOK && usernameOK && ssidOK){\n");
        message += F("document.getElementById(\"savebutton\").disabled = false;\n");
      message += F("} else {\n");
        message += F("document.getElementById(\"savebutton\").disabled = true;\n");
      message += F("}\n");
    message += F("}\n");        

  message += F("</script>\n");
  message += F("<script>\n");
  message += F("document.onload = checkfields();\n");
  message += F("</script>\n");
  message += F("</div>\n");
  message += F("</body>\n</html>\n");

  webServer.send ( 200, F("text/html"), message);
  digitalWrite ( led, 1 );
}

void handleInstall() 
{
  digitalWrite ( led, 0 );
  int page = webServer.arg("page").toInt();
  int lastpage = 6;
  bool errorfound = false;
  String errormsg = "";
  String message = "";

  
  if (webServer.method() == HTTP_POST)
  {
    //save action
    if (page == 2)
    {
      //save device name
      String devname = webServer.arg("devicename");
      devname.trim();
      for (byte i = 0; i < devname.length(); i++)
      {
        if (!isAlphaNumeric(devname.charAt(i)))
        {
          errormsg = "Invalid character";
          errorfound = true;          
        }
      }
      if (!errorfound)
      {
        if (devname.length() > 32)
        {
            errormsg = "The devicename is too long";
            errorfound = true;                  
        } 
        else if (devname.length() < 1)
        {
            errormsg = "The devicename is too short";
            errorfound = true;                  
        }        
      }

      if (errorfound)
      {
        page--;
      }
      else
      {
        config.setDevicename(devname);
      }
    }
    else if (page == 3)
    {
      //save user acount
      String usrname = webServer.arg("username");
      String pwd1 = webServer.arg("pwd1");
      String pwd2 = webServer.arg("pwd2");

      usrname.trim();
     
      for (byte i = 0; i < usrname.length(); i++)
      {
        if (!isAlphaNumeric(usrname.charAt(i)))
        {
          errormsg = "Invalid character";
          errorfound = true;          
        }
      }

      if (!errorfound)
      {
        if (usrname.length() > 20)
        {
            errormsg = "The username is too long";
            errorfound = true;
        }
        else if (usrname.length() < 3)
        {
            errormsg = "The username is too short";
            errorfound = true;
        }
        else if (pwd1.length() > 20)
        {
            errormsg = "The password is too long";
            errorfound = true;
        }
        else if (pwd1.length() < 8)
        {
            errormsg = "The password is too short";
            errorfound = true;
        }
        else if (pwd1 != pwd2)
        {
            errormsg = "The passwords are not equal";
            errorfound = true;
        }
        if (!errorfound){
          page = 101;
        }
      }
      
      if (errorfound)
      {
        page--;
      }
      else
      {
        config.setUsername(usrname);
        config.setUserpass(pwd1);
      }
    }
    else if (page == 5)
    {
      //save
      WiFi.mode(WIFI_AP_STA);
      char _ssid[webServer.arg("ssid").length()+1];
      webServer.arg("ssid").toCharArray(_ssid, webServer.arg("ssid").length()+1);
      
      char _pass[webServer.arg("pass").length()+1];
      webServer.arg("pass").toCharArray(_pass, webServer.arg("pass").length()+1);
      
      WiFi.begin(_ssid, _pass);
      page = 102;
    }
  }


  if (page == 102)
  {
    //connection to wifi
    if (WiFi.status() == 6)
    {
      //still connecting
      String content = F("<h1>Connection to Wifi..</h1>");
      message = buildInstallPage(htmlHead("Install - Step 4", 1, "/install?page=102" ),page,lastpage ,content, errormsg, false, false, false);    

    } 
    else if (WiFi.status() == 3)
    {  
      //connected!
      WiFi.setAutoReconnect(true);
      String content = F("<h1>Connected to Wifi!</h1>");
      message = buildInstallPage(htmlHead("Install - Step 4", 1, "/install?page=4"),page,lastpage ,content, errormsg, true);
    } else {
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


  if (page == 101)    
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
      
      message = buildInstallPage(htmlHead("Install - Step 3", 1, "/install?page=101" ),page,lastpage ,content, errormsg, false, false, false);    

    } 
    else if (WiFi.scanComplete() == 0)
    {
      //wifiscan complete no networks found

      String content = F("No networks in range <a href =\"/install?page=101&wifirefresh=1\">Refresh Wifi networks. </a>");
      message = buildInstallPage(htmlHead("Install - Step 3"),page,lastpage ,content, errormsg, false, false, false);    

    } 
    else 
    {
      //wifiscan complete networks found, next page
      
      String content = F("Networks found, processing please wait.");
      message = buildInstallPage(htmlHead("Install - Step 3", 1, "/install?page=3"),page,lastpage ,content, errormsg, false, false, false);    
    }
  } 



  if (page == 6)
  {
    //Save and reboot message
    String content = F("<h1>Save and reboot</h1>");
    message = buildInstallPage(htmlHead("Install - Step 6"),page,lastpage ,content, errormsg);    
  }  

  if (page == 5)
  {
    //Ipconfig
    String content = F("<h1>Ipconfig page..</h1>");
    message = buildInstallPage(htmlHead("Install - Step 5"),page,lastpage ,content, errormsg);
  }  

  if (page == 4)
  {
    //dhcp question
    String content = F("<h1>Dhcp page..</h1>");
    message = buildInstallPage(htmlHead("Install - Step 4"),page,lastpage ,content, errormsg);

  }  
  

  if (page == 3)
  {
    // Page to enter wifi configuration
    String content = F("Wifi settings");
    content += F("<table>");
    content += F("<tr>");
    content += F("<td>Network SSID:</td><td>");
    content += F(" <select name =""ssid"">");
    for (int i = 0; i < WiFi.scanComplete(); i++)
    
    {
      content += F("<option value=""");
      content += WiFi.SSID(i);
      if (WiFi.SSID(i) == WiFi.SSID())
      {
        content += F(""" selected>");  
      } 
      else 
      {
        content += F(""">");  
      }
      content += WiFi.SSID(i);
      content += F("</option>");
    }
    content += F("</select>");
    content += F("</td></tr><tr>");
    content += F("<td>Password:</td><td>");
    
    content += F("<input type=""password"" name=""pass"">");
    content += F("</td></tr></table>");
    content += F("<a href =\"/install?page=101&wifirefresh=1\">Refresh Wifi networks. </a>");
    
    message = buildInstallPage(htmlHead("Install - Step 3"),page ,lastpage ,content, errormsg);    
  }
  
  
  if (page == 2)
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

    message = buildInstallPage(htmlHead("Install - Step 2"),page ,lastpage ,content, errormsg);
  }

  if (page == 1)
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

    message = buildInstallPage(htmlHead("Install - Step 1"),page ,lastpage ,content, errormsg );
  }


  if (page == 0)
  {
    //Information
    
    String content = F("<H2>Welcome to your new WiFi device.</H2>");
    content += F("Please follow this wizzard and enter these basic settings to install the device.<br>");
    message = buildInstallPage(htmlHead("Install - Intro"),page ,lastpage ,content, errormsg, false, false);

  }
  webServer.send ( 200, F("text/html"), message);
  digitalWrite ( led, 1 );
}

void handleDebug() {
  digitalWrite ( led, 0 );
 
  String message = "";
  
  File configFile = SPIFFS.open("/config.json", "r");
  if (!configFile) {
    message+=(F("Failed to open config file"));
  } else {
  
    message = configFile.readString();
  }

  webServer.send ( 200, F("text/json"), message);
  digitalWrite ( led, 1 );
}



void initWebServer(){
  
  if (installMode){
    webServer.on ( "/css", handleCSSinstall );
    webServer.on ( "/install", handleInstall );
    webServer.on ( "/install_save", processInstall );
    webServer.onNotFound ( handleNotFoundInstall );
  } else {

    webServer.on ( "/install", handleInstall );
    //webServer.on ( "/css", handleCSS );
    webServer.on ( "/css", handleCSSinstall );

    webServer.on ( "/", handleRoot );
    webServer.on ( "/setup", handleWifiSetup );
    webServer.on ( "/connect", handleConnect );    
    webServer.on ( "/ipconfig", handleIpSetup );
    webServer.on ( "/time", handleTimeSetup );
    webServer.on ( "/dbg", handleDebug );    
    webServer.onNotFound ( handleNotFound );
  }

  
  
  webServer.begin();
}


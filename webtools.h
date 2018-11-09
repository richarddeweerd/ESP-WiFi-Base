String htmlHead(String pagename, byte refresh = 0, String target = "")
{
  String msg = F("\n<head>\n<title>");
  msg += pagename;
  msg += F(" - ");
  msg += String(config.devicename);
  msg += F("</title>\n");
  if (refresh > 0)
  {
    msg += F("<meta http-equiv=\"refresh\" content=\"");
    msg += String(refresh);
    if (target != "")
    {
      msg += F(";url=");
      msg += String(target);
    }
    msg += F("\">\n");
  }
  msg += F("<link rel=\"stylesheet\" href=\"css\">\n");
  msg += F("</head>\n");
  return msg;
}

String topMenu(byte selecteditem = 0)
{
  String msg = F("<nav role=\"main\">\n<ul>\n");
  msg += F("<li><a ");
  if (selecteditem == 1)
  {
    msg += F("class=\"active\" ");
  }
  msg += F("href=\"/\">Home</a></li>\n");

  msg += F("<li><a ");
  if (selecteditem == 2)
  {
    msg += F("class=\"active\" ");
  }
  msg += F("href=\"fu\">Future</a></li>\n");


  msg += F("<li style=\"float:right\"><a ");  
  if (selecteditem == 99)
  {
    msg += F("class=\"active\" ");
  }
  msg += F("href=\"about\">About</a></li>\n");

  msg += F("<li style=\"float:right\"><a ");
  if (selecteditem == 9)
  {
    msg += F("class=\"active\" ");
  }
  msg += F("href=\"/settings\">Configuration</a></li>\n");


  msg += F("</ul>\n</nav>\n");
  return msg;
}


void handleCSSinstall()
{
  digitalWrite(led, 0);
  String message = F("#installcontent {padding: 10px;color: white;text-align: left;width: 100%;height: 210px;box-sizing: border-box;}");
  message += F("body {background-color: rgb(42, 42, 42);font-family: Arial, sans-serif;}");
  message += F("#installscreen {position: absolute;top: 50%;left: 50%;width: 450;height: 300;margin-top: -150;margin-left: -225;border: 3px solid #2f6dd8;}");
  message += F("#installheader {border-bottom: 3px solid #2f6dd8;padding: 10px;padding-right: 20px;font-size: 20px;color: white;text-align: right;width: 100%;box-sizing: border-box;}");
  message += F("td {color: white;}");
  message += F("#installbuttons {float: bottom;padding: 10px;background-color: #2f6dd8;width: 100%;box-sizing: border-box;}");
  message += F("#errormsg {font-size: 10px;color:red; text-align: right;}");

  message += F("a.button {font: bold 13px Arial, sans-serif; text-decoration: none; background-color: #EEEEEE; color: #333333; padding: 2px 6px 2px 6px; border-top: 1px solid #CCCCCC; border-right: 1px solid #CCCCCC; border-bottom: 1px solid #CCCCCC; border-left: 1px solid #CCCCCC;}");
  message += F("a {color: white;}");
  message += F(".ipdigit{width: 45px;text-align: right;}");
  webServer.send(200, F("text/css"), message);
  digitalWrite(led, 1);
}

void handleCSS()
{
  digitalWrite(led, 0);
  String message = F("body { background-color: white;font-family: Arial, sans-serif;}");
  message += F("h1 { color: black; }");
  message += F("p { color: black; font-family: Arial, sans-serif;}");

  message += F("nav[role=\"main\"] ul li {background-color:#333;}");
  message += F("nav[role=\"side\"] ul li {background-color:#777;width:200px;}");
  message += F("nav[role=\"main\"] ul {background-color: #333;}");
  message += F("nav[role=\"side\"] ul {height: 100%; width:200px; position: fixed;}");
  message += F("nav[role=\"side\"] a{text-align: left;}");
  message += F("nav[role=\"main\"] a{text-align: center;}");
  message += F("ul {list-style-type: none; margin: 0; padding: 0; overflow: hidden; font-family: Arial, sans-serif;}");
  message += F("li {float: left;}");
  message += F("li a {display: block; color: white; padding: 14px 16px; text-decoration: none; font-family: Arial, sans-serif;}");
  message += F("li a:hover:not(.active){background-color: #111;}");
  message += F(".active {background-color: #2f6dd8;}");
  message += F(".main {background-color: #2f6dd8;padding: 10px;}");
  message += F(".bcell {text-align: right;align:right;}");

  message += F("#main { width: 100%;overflow: hidden;}");
  message += F("#sidemenu { width: 200px; float:left;}");
  message += F("#content { overflow: hidden; margin-left:200px;}");

  webServer.send(200, F("text/css"), message);
  digitalWrite(led, 1);
}

String ipInputFields(IPAddress addr, String nameBase)
{
  String res;
  for (byte i = 0; i < 4; i++)
  {
    res += F("<input type=\"number\" style=\"width: 45px;\" max=255 min=0 name=\"");
    res += nameBase;
    res += i;
    res += F("\" value=\"");
    res += addr[i];
    res += F("\">");
    if (i < 3)
    {
      res += F(" . ");
    }
  }
  return res;
}



String buildConfigPage(byte page, String pagename, String content)
{
  String msg = F("<html><head><title>");
  msg += pagename;
  msg += F(" - ");
  msg += String(config.devicename);
  msg += F("</title>\n");
  msg += F("<link rel=\"stylesheet\" href=\"css\">\n");
  msg += F("</head>\n");
  msg += F("<body>\n");
  msg += topMenu(9);
  msg += F("<div id=main>");
  msg += F("<div id=sidemenu>");
  msg += F("<nav role=\"side\">");
  msg += F("<ul>\n");

  msg += F("<li><a ");
  if (page == 0)
  {
    msg += F("class=\"active\" ");
  }
  msg += F("href=\"/settings?page=0\">General</a></li>\n");

  msg += F("<li><a ");
  if (page == 1)
  {
    msg += F("class=\"active\" ");
  }
  msg += F("href=\"/settings?page=1\">WiFi</a></li>\n");

  msg += F("<li><a ");
  if (page == 2)
  {
    msg += F("class=\"active\" ");
  }
  msg += F("href=\"/settings?page=2\">IP address</a></li>\n");

  msg += F("<li><a ");
  if (page == 3)
  {
    msg += F("class=\"active\" ");
  }
  msg += F("href=\"/settings?page=3\">Time</a></li>\n");

  msg += F("</ul>\n");
  msg += F("</nav>");
  msg += F("</div>");
  msg += F("<div id=content>");
  msg += content;
  msg += F("</div>");
  msg += F("</div>");
  msg += F("</body></html>");

  return msg;
}

String buildInstallPage(String header, byte installstep, byte totalsteps, String content, String errormsg, bool showBackButton = true, bool showClearButton = true, bool showNextButton = true)
{
  String message = F("<html>\n");
  message += header;
  message += F("<body>\n");
  message += F("<div id=\"installscreen\">\n");
  message += F("<div id=\"installheader\">\n");
  if (installstep == 0)
  {
    //first page
    message += F("&nbsp;");
  }
  else if (installstep > totalsteps)
  {
    //lasst page
    message += F("Installation complete\n");
  }
  else
  {
    message += "Installation step " + String(installstep) + " of " + String(totalsteps) + "\n";
  }

  message += F("</div>\n");
  message += F("<div class=\"form\">\n");
  message += "<form action=\"/install?page=" + String(installstep + 1) + "\" method=\"post\">\n";

  message += F("<div id=\"installcontent\">\n");
  message += content;
  if (errormsg != "")
  {
    message += "<div id=errormsg>Error: " + errormsg + "</div>\n";
  }

  message += F("</div>\n");
  message += F("<div id=\"installbuttons\">\n");

  message += F("<table width=100%>\n<tr>\n");
  message += F("<td width=33% align='left'>");

  if (showBackButton)
  {
    message += "<a href=\"/install?page=" + String(installstep - 1) + "\" class=\"button\">Back</a>";
  }
  else
  {
    message += F("&nbsp;");
  }

  message += F("</td>\n");
  message += F("<td width=34% align='center'>");

  if (showClearButton)
  {
    message += F("<button type='reset' value='Reset'>Clear</button>");
  }
  else
  {
    message += F("&nbsp;");
  }

  message += F("</td>\n");
  message += F("<td width=33% align='right'>");

  if (showNextButton)
  {
    if (installstep != totalsteps)
    {
      message += F("<button id='savebutton' type='submit' value='Submit'>Next</button>");
    }
    else
    {
      message += F("<button id='savebutton' type='submit' value='Submit'>Reboot</button>");
    }
  }
  else
  {
    message += F("&nbsp;");
  }

  message += F("</td>\n");
  message += F("</tr>\n</table>\n");

  message += F("</form>\n");
  message += F("</div>\n");
  message += F("</div>\n");

  message += F("</body>\n</html>\n");
  return message;
}

void handleNotFoundInstall()
{
  digitalWrite(led, 0);
  String message = F("<html>\n");
  message += htmlHead(F("Not found"), 1, "/install");
  message += F("<body>\n");
  message += F("Redirecting... \n");
  message += F("</body></html>\n");
  webServer.send(200, F("text/html"), message);
  digitalWrite(led, 1);
}

void handleNotFound()
{
  digitalWrite(led, 0);
  String message = F("File Not Found\n\n");
  message += F("URI: ");
  message += webServer.uri();
  message += F("\nMethod: ");
  message += (webServer.method() == HTTP_GET) ? "GET" : "POST";
  message += F("\nArguments: ");
  message += webServer.args();
  message += F("\n");

  for (uint8_t i = 0; i < webServer.args(); i++)
  {
    message += " " + webServer.argName(i) + ": " + webServer.arg(i) + "\n";
  }
  webServer.send(404, F("text/plain"), message);
  digitalWrite(led, 1);
}

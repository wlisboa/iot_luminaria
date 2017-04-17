#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

WiFiUDP ntpUDP;
 
int16_t utc = -4; //UTC -4:00 Brazil
uint32_t currentMillis = 0;
uint32_t previousMillis = 0;
boolean bAlarme;  
NTPClient timeClient(ntpUDP, "a.st1.ntp.br", utc*3600, 60000);


//************************************************************
// Define Area
//************************************************************

// EEPROM Addresses

#define ID_LEN          50
#define TY_LEN          5
#define HR_LEN          6

#define EE_ID_ADDR      0                     // The EEPROM adress used to store the ID (to check if sketch already has ran) - 1 byte long
#define EE_A_TYPE       EE_ID_ADDR   + 1      // Type of alarm. That can be luz or son
#define EE_HOUR         EE_A_TYPE    + TY_LEN
#define EE_WIFI_SSID    EE_HOUR      + HR_LEN // Wifi SSID - 30 bytes long
#define EE_WIFI_PASS    EE_WIFI_SSID + ID_LEN // Wifi PASS - 30 bytes long
#define EE_MAC_ADDR     EE_WIFI_PASS + ID_LEN // First byte address of the MAC - 6 bytes long
#define EE_IP_ADDR      EE_MAC_ADDR  + 6      // First byte address of the IP - 4 bytes long
#define EE_GW_ADDR      EE_IP_ADDR   + 4      // First byte address of the GATEWAY - 4 bytes long
#define EE_MASK_ADDR    EE_GW_ADDR   + 4      // First byte address of the SUBNETMASK - 4 bytes long
#define ID              0x00                  // Used to identify if valid date in EEPROM the "know" bit,
                                              // if this is written in EEPROM the sketch has ran before

const char *ssidAP     = "luminaria";
const char *passwordAP = "";

char ssidSTAT[ID_LEN]     = "Wlan_Lisboa";
char passwordSTAT[ID_LEN] = "familia2015";

char atype[TY_LEN] = "luz";
char ahour[HR_LEN] = "06:00";

// Network Parameters
uint8_t mac[]    = {0x00,0x00,0x00,0x00,0x00,0x00};
IPAddress ip(192,168,0,54);
IPAddress subnet(255,255,255,0);
IPAddress gateway(192,168,0,1);

ESP8266WebServer server(80);

const int led = 0;
const int lamp =5;

String monta_resposta_config(){
  
String pagina_resposta= "<!DOCTYPE html>";
pagina_resposta += "<html lang=\"pt-br\">";
pagina_resposta +=     "<head>";
pagina_resposta +=         "<title>IOT_CONFIG</title>";
pagina_resposta +=         "<meta charset=\"uft=8\">";
pagina_resposta +=         "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
pagina_resposta +=     "</head>";
pagina_resposta +=     "<body>";
pagina_resposta +=        "<header>";
pagina_resposta +=            "<h1> Lumin&aacute;ria </h1>";
pagina_resposta +=        "</header>";
pagina_resposta +=       "<main>";
pagina_resposta +=       "<p>Configura&ccedil;&atilde;o enviada com sucesso!</p>";
pagina_resposta +=       "</main>";
pagina_resposta +=        "<footer>";
pagina_resposta +=            "<p>Criado por: Washington Lisboa</p>";
pagina_resposta +=            "<a href=\"https://github.com/wlisboa/iot_luminaria\">fonte do codigo</a>";
pagina_resposta +=        "</footer>";
pagina_resposta +=     "</body>";
pagina_resposta +=     "<style>";
pagina_resposta +=        "body {";
pagina_resposta +=            "text-align: center;";
pagina_resposta +=        "}";
pagina_resposta +=        "header {";
pagina_resposta +=            "padding-top: 5px;";
pagina_resposta +=            "border-bottom: 2px solid #000;";
pagina_resposta +=            "background-color: #38C2B3;";
pagina_resposta +=        "}";
pagina_resposta +=        "main {";
pagina_resposta +=            "border-bottom: 2px solid #000;";
pagina_resposta +=        "}";
pagina_resposta +=     "</style>";
pagina_resposta +="</html>";
return pagina_resposta;
}


String monta_index(){
  
String pagina_index = "<!DOCTYPE html>";
pagina_index += "<html lang=\"pt-br\">";
pagina_index +=     "<head>";
pagina_index +=         "<title>IOT_CONFIG</title>";
pagina_index +=         "<meta charset=\"uft=8\">";
pagina_index +=         "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
pagina_index +=     "</head>";
pagina_index +=     "<body onload=\"load()\">";
pagina_index +=       "<header>";
pagina_index +=         "<h1> Lumin&aacute;ria </h1>";
pagina_index +=       "</header>";
pagina_index +=       "<main>";
pagina_index +=        "<form class=\"form-conf\" method=\"POST\" action=\"/conf\" >";
pagina_index +=             "<div>";
pagina_index +=                 "<h3>Configura&ccedil;&atilde;o do despertador</h3>";
pagina_index +=                 "<select id=\"desp\" name=\"desp\">";
pagina_index +=                     "<option value=\"luz\">Luz</option>";
pagina_index +=                     "<option value=\"som\">Som</option>";
pagina_index +=                 "</select>";
pagina_index +=                 "<label>Hora:</label>";
pagina_index +=                 "<input type=\"time\" id=\"time\" name=\"time\">";
pagina_index +=             "</div>";
pagina_index +=             "<div>";
pagina_index +=                 "<h3>Configura&ccedil;&atilde;o da rede</h3>";

pagina_index +=                 "<label>Nome da rede:</label>";
pagina_index +=                 "<div class=\"data\">";
pagina_index +=                     "<input type=\"text\" id=\"rede\" name=\"rede\">";
pagina_index +=                 "</div>";

pagina_index +=                 "<label>Senha:</label>";
pagina_index +=                 "<div class=\"data\">";
pagina_index +=                     "<input type=\"password\" id=\"pass\" name=\"pass\">";
pagina_index +=                 "</div>";

pagina_index +=                 "<label>IP:</label>";
pagina_index +=                 "<div class=\"data\">";
pagina_index +=                     "<input id=\"ip\" name=\"ip\" required pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\">";
pagina_index +=                 "</div>";

pagina_index +=                 "<label>Subnet Mask:</label>";
pagina_index +=                  "<div class=\"data\">";
pagina_index +=                     "<input id=\"subn\" name=\"subn\" required pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\">";
pagina_index +=                  "</div>";

pagina_index +=                 "<label>Gateway:</label>";
pagina_index +=                 "<div class=\"data\">";
pagina_index +=                     "<input id=\"gatw\" name=\"gatw\" required pattern=\"^([0-9]{1,3}\.){3}[0-9]{1,3}$\">";
pagina_index +=                 "</div>";
pagina_index +=                 "<div class=\"d-sub1\">";
pagina_index +=                     "<input class=\"sub1\" type=\"submit\" value=\"Enviar\">";
pagina_index +=                 "</div>";
pagina_index +=             "</div>";
pagina_index +=         "</form>";
pagina_index +=       "</main>";
pagina_index +=       "<footer>";
pagina_index +=         "<p>Criado por: Washington Lisboa</p>";
pagina_index +=         "<a href=\"https://github.com/wlisboa/iot_luminaria\">fonte do codigo</a>";
pagina_index +=       "</footer>";
pagina_index +=     "</body>";
pagina_index +=       "<script>";
pagina_index +=       "function load() {";
pagina_index +=         "var xhttp = new XMLHttpRequest();";
pagina_index +=         "xhttp.onreadystatechange = function()";
pagina_index +=         "{";
pagina_index +=           "if (xhttp.readyState == 4 && xhttp.status == 200)";
pagina_index +=           "{";
pagina_index +=               "var data = xhttp.responseText;";
pagina_index +=               "var data_s = data.split(\" \");";
pagina_index +=               "document.getElementById(\"desp\").value = data_s[0];";
pagina_index +=               "document.getElementById(\"time\").value = data_s[1];";
pagina_index +=               "document.getElementById(\"rede\").value = data_s[2];";
pagina_index +=               "document.getElementById(\"pass\").value = data_s[3];";
pagina_index +=               "document.getElementById(\"ip\").value = data_s[4];";
pagina_index +=               "document.getElementById(\"subn\").value = data_s[5];";
pagina_index +=               "document.getElementById(\"gatw\").value = data_s[6];";
pagina_index +=           "}";
pagina_index +=        "};";
pagina_index +=         "xhttp.open(\"POST\", \"/load\", true);";
pagina_index +=         "xhttp.send();";
pagina_index +=       "}";
pagina_index +=     "</script>";
pagina_index +=     "<style>";
pagina_index +=       "body {";
pagina_index +=           "text-align: center;";
pagina_index +=       "}";
pagina_index +=       "header {";
pagina_index +=           "padding-top: 2px;";
pagina_index +=           "background-color: #38C2B3;";
pagina_index +=           "border-bottom: 2px solid #666;";
pagina_index +=       "}";
pagina_index +=       "main {";
pagina_index +=           "border-bottom: 2px solid #666;";
pagina_index +=       "}";
pagina_index +=       ".data{";
pagina_index +=           "padding-bottom: 5px;";
pagina_index +=       "}";
pagina_index +=       ".data input{";
pagina_index +=           "font-size: 16px;";
pagina_index +=       "}";
pagina_index +=       ".form-conf .d-sub1{";
pagina_index +=           "padding-top: 20px;";
pagina_index +=           "padding-bottom: 20px;";
pagina_index +=       "}";
pagina_index +=       ".form-conf .d-sub1 .sub1{";
pagina_index +=           "font-size: 25px;";
pagina_index +=           "width: 150px;";
pagina_index +=       "}";
pagina_index +=     "</style>";
pagina_index +=  "</html>";

return pagina_index;
}

String monta_pagina()
{
String pagina_root = "<!DOCTYPE html>";
pagina_root += "<html lang=\"pt-br\">";
pagina_root +=               "<head>";
pagina_root +=        "<title>Luminaria</title>";
pagina_root +=        "<meta charset=\"uft=8\">";
pagina_root +=        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">";
pagina_root +=    "</head>";
pagina_root +=    "<body>";
pagina_root +=    	"<header>";
pagina_root +=    		"<h1> Lumin&aacute;ria </h1>";
pagina_root +=    	"</header>";
pagina_root +=    	"<main>";
pagina_root +=    		"<div>";
pagina_root +=    			"<p>Demonstra&ccedil;&atilde;o IOT</p>";
pagina_root +=    		"</div>";
pagina_root +=    		"<div class=\"sens-area\">";
pagina_root +=    			"<button type=button onclick=\"sensor()\">Ler Sensor</button>";
pagina_root +=    			"<p id=\"sensor\">valor lido</p>";
pagina_root +=    		"</div>";
pagina_root +=    		"<div class=\"cmd-area\">";
pagina_root +=    			"<button type=\"button\" id=\"btn1\" onclick=\"comando()\">Liga</button>";
pagina_root +=    		"</div>";
pagina_root +=    		"<div class=\"cmd-area\">";
pagina_root +=    			"<button type=\"button\" id=\"1\" onclick=\"dimmer(this)\">10%</button>";
pagina_root +=    			"<button type=\"button\" id=\"2\" onclick=\"dimmer(this)\">25%</button>";
pagina_root +=    			"<button type=\"button\" id=\"3\" onclick=\"dimmer(this)\">50%</button>";
pagina_root +=    			"<button type=\"button\" id=\"4\" onclick=\"dimmer(this)\">80%</button>";
pagina_root +=    		"</div>";
pagina_root +=    	"</main>";
pagina_root +=    	"<footer>";
pagina_root +=    		"<p>instrutor: Washington Lisboa</p>";
pagina_root +=    		"<a href=\"https://github.com/wlisboa/iot_web_server\">fonte do codigo</a>";
pagina_root +=    	"</footer>";
pagina_root +=    "</body>";
pagina_root +=    	"<script>";
pagina_root +=			"function sensor() {";
pagina_root +=				"var xhttp = new XMLHttpRequest();";
pagina_root +=				"xhttp.onreadystatechange = function()";
pagina_root +=				"{";
pagina_root +=					"if (xhttp.readyState == 4 && xhttp.status == 200)";
pagina_root +=					"{";
pagina_root +=						"document.getElementById(\"sensor\").innerHTML = xhttp.responseText;";
pagina_root +=					"}";
pagina_root +=				"};";
pagina_root +=				"xhttp.open(\"POST\", \"/sensor\", true);";
pagina_root +=				"xhttp.send();";
pagina_root +=			"}";
pagina_root +=			"function comando() {";
pagina_root +=				"var xhttp = new XMLHttpRequest();";
pagina_root +=				"xhttp.onreadystatechange = function()";
pagina_root +=				"{";
pagina_root +=					"if (xhttp.readyState == 4 && xhttp.status == 200)";
pagina_root +=					"{";
pagina_root +=						"document.getElementById(\"btn1\").innerHTML = xhttp.responseText;";
pagina_root +=					"}";
pagina_root +=				"};";
pagina_root +=				"xhttp.open(\"POST\", \"/cmd1\", true);";
pagina_root +=				"xhttp.send();";
pagina_root +=			"}";
pagina_root +=			"function dimmer(button){";
pagina_root +=				"if (button.id=='1'){";
pagina_root +=				    "var xhttp=new XMLHttpRequest();";   
pagina_root +=				    "xhttp.open(\"POST\",\"/10\",true);";
pagina_root +=				    "xhttp.send();";
pagina_root +=			        "}";
pagina_root +=				"if (button.id=='2'){";
pagina_root +=				    "var xhttp = new XMLHttpRequest();";   
pagina_root +=				    "xhttp.open(\"POST\", \"/25\", true);";
pagina_root +=				    "xhttp.send();";
pagina_root +=			        "}";
pagina_root +=				"if (button.id=='3'){";
pagina_root +=				    "var xhttp = new XMLHttpRequest();";   
pagina_root +=				    "xhttp.open(\"POST\", \"/50\", true);";
pagina_root +=				    "xhttp.send();";
pagina_root +=			        "}";
pagina_root +=				"if (button.id=='4'){";
pagina_root +=				    "var xhttp = new XMLHttpRequest();";   
pagina_root +=				    "xhttp.open(\"POST\", \"/80\", true);";
pagina_root +=				    "xhttp.send();";
pagina_root +=			        "}";	
pagina_root +=			"}";
pagina_root +=		"</script>";
pagina_root +=		"<style>";
pagina_root +=			"body {";
pagina_root +=				"text-align: center;";
pagina_root +=			"}";
pagina_root +=			"button {";
pagina_root +=				"height: 50px;";
pagina_root +=  				"width: 150px;";
pagina_root +=			"}";
pagina_root +=			"header {";
pagina_root +=				"padding-top: 5px;";
pagina_root +=				"border-bottom: 2px solid #000;";
pagina_root +=				"background-color: #38C2B3;";
pagina_root +=			"}";
pagina_root +=			"main {";
pagina_root +=				"border-bottom: 2px solid #000;";
pagina_root +=			"}";
pagina_root +=			".sens-area{";
pagina_root +=				"border-bottom: 2px solid #000;";
pagina_root +=			"}";
pagina_root +=			".cmd-area{";
pagina_root +=				"display: fex;";
pagina_root +=				"align-items: center;";
pagina_root +=				"justify-content: space-between;";
pagina_root +=				"border-bottom: 2px solid #000;";
pagina_root +=				"padding-top: 10px;";
pagina_root +=				"padding-bottom: 10px;";
pagina_root +=			"}";
pagina_root +=		"</style>";
pagina_root += "</html>";

  return pagina_root;
}
/*
 *=============================================================================
 *NAME:         void handleRoot()
 *
 *DESCRIPTION:  Essa função é usada pelo servidor web e é responsavel por
 *              enviar uma pagina para o navegador.
 *=============================================================================
*/
void handleRoot() {
    
  digitalWrite(led, 0);
  server.send(200, "text/html", monta_pagina());
  digitalWrite(led, 1);
}
/*
 *=============================================================================
 *NAME:         void handleLoad()
 *
 *DESCRIPTION:  
 *
 *=============================================================================
*/
void handleLoad() {

  String data;
  data =  atype;
  data += " ";
  data += ahour;
  data += " "; 
  data += ssidSTAT;
  data += " ";
  data += passwordSTAT;
  data += " ";
  data += ip.toString();
  data += " ";
  data += subnet.toString();
  data += " ";
  data += gateway.toString();

  Serial.println(data);     
  server.send(200, "text/text", data);  
}
/*
 *=============================================================================
 *NAME:         void handleLoad()
 *
 *DESCRIPTION:  
 *
 *=============================================================================
*/
void handleConf(){
  String sArg, sSub;
  char cIp[20];
  int firstPoint;
  
  Serial.println("Handle Config");
  if (server.hasArg("desp")){
      sArg = server.arg("desp");
      sArg.toCharArray(atype, TY_LEN); 
      Serial.println(sArg);      
      sArg = server.arg("time");
      sArg.toCharArray(ahour, HR_LEN);
      Serial.println(sArg);
      sArg = server.arg("rede");
      Serial.println(sArg);
      sArg.toCharArray(ssidSTAT, ID_LEN);   
      sArg = server.arg("pass");
      Serial.println(sArg);
      sArg.toCharArray(passwordSTAT, ID_LEN);
      ip.fromString(server.arg("ip"));           
      Serial.println(ip);
      subnet.fromString(server.arg("subn"));
      Serial.println(subnet);
      gateway.fromString(server.arg("gatw"));
      Serial.println(gateway);                         
      saveBoardConfig();
      server.send(200, "text/html", monta_resposta_config());
   }
   else
   {
      server.send(200, "text/html", monta_index());
   }
}
/*
 *=============================================================================
 *NAME:         void handleSensor()
 *
 *DESCRIPTION:  
 *=============================================================================
*/
void handleSensor() 
{
  float valor;
  String convertido;
  digitalWrite(led, 1);
  convertido = "100";
  server.send ( 200, "text/text", convertido); 
  digitalWrite(led, 1);
}
/*
 *=============================================================================
 *NAME:         void handleCmd1()
 *
 *DESCRIPTION:
 *=============================================================================
*/
void handleCmd1(){ 
  static int controle = 0;  
  digitalWrite(lamp, 0);
  
  Serial.println("Comando 1"); 
  if (controle == 0){
    controle = 1;
    digitalWrite(lamp, 1);
    analogWrite(lamp,1023);
    server.send ( 200, "text/text", "Desliga"); 
  }else{
    controle = 0;
    analogWrite(lamp,0);
    server.send ( 200, "text/text", "Liga");     
  }
  
}
/*
 *=============================================================================
 *NAME:         
 *
 *DESCRIPTION:  
 *=============================================================================
*/
void handleCmd10() {
  Serial.println("Comando 10%");
  analogWrite(lamp,40);
}
/*
 *=============================================================================
 *NAME:         
 *
 *DESCRIPTION:  
 *=============================================================================
*/
void handleCmd25() {
  Serial.println("Comando 25%");
    analogWrite(lamp,100);
}
/*
 *=============================================================================
 *NAME:         
 *
 *DESCRIPTION:  
 *=============================================================================
*/
void handleCmd50() {
  Serial.println("Comando 50%");
    analogWrite(lamp,200);
}
/*
 *=============================================================================
 *NAME:         
 *
 *DESCRIPTION:  
 *=============================================================================
*/
void handleCmd80() {
  Serial.println("Comando 80%");
  analogWrite(lamp,600);
}
/*
 *=============================================================================
 *NAME:         handleNotFound(){
 *
 *DESCRIPTION:  Essa função é usada pelo servidor web e é responsavel por
 *              enviar uma página de erro.
 *=============================================================================
*/
void handleNotFound(){
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

/*
 *=============================================================================
 *NAME:         
 *
 *DESCRIPTION:  
 *=============================================================================
*/
void setup_AP(){

  WiFi.disconnect(true);
  delay(200);
  WiFi.mode(WIFI_AP);
  
  Serial.println();
  Serial.println("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssidAP, passwordAP);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  Serial.println("");
}
/*
 *=============================================================================
 *NAME:         
 *
 *DESCRIPTION:  
 *=============================================================================
*/
int setup_STATION(){
  
  Serial.println();
  Serial.println("Configuring station...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.begin(ssidSTAT, passwordSTAT);
  delay(100);
  WiFi.config(ip, gateway, subnet);
  
  // Wait for connection
  double initTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    analogWrite(lamp,200);
    delay(200);
    analogWrite(lamp,0);
    delay(200);
    if((millis() - initTime) > 8000) {
      Serial.println("Connection Failed!");
      analogWrite(lamp,0);
      return -1;
    }  
  }
  analogWrite(lamp,1023);
  IPAddress myIP = WiFi.localIP();
  Serial.println("Connected!");
  Serial.print("IP address: ");
  Serial.println(myIP);
  myIP = WiFi.subnetMask();  
  Serial.print("SubnetMask: ");
  Serial.println(myIP);
  myIP = WiFi.gatewayIP();  
  Serial.print("Gateway Ip: ");
  Serial.println(myIP);
  
  Serial.println("");
  return 0;
}
/*
 *=============================================================================
 *NAME:         
 *
 *DESCRIPTION:  
 *=============================================================================
*/
void readBoardConfig(void)
{
  int idcheck = EEPROM.read(EE_ID_ADDR);
  int index;
  
  //Serial.print("Read eeprom: ");
  //Serial.println(idcheck);
  
  if (idcheck == ID){
    // Load atype
    for ( index = 0; index < TY_LEN; index++){
      atype[index] = EEPROM.read(EE_A_TYPE + index);
    }    
    // Load ahour
    for ( index = 0; index < HR_LEN; index++){
      ahour[index] = EEPROM.read(EE_HOUR + index);
    }    
    //Serial.print("Load recorded");
    // Load WIFI SSID
    for ( index = 0; index < ID_LEN; index++){
      ssidSTAT[index] = EEPROM.read(EE_WIFI_SSID + index);
    }
    // Load WIFI PASS
    for ( index = 0; index < ID_LEN; index++){
      passwordSTAT[index] = EEPROM.read(EE_WIFI_PASS + index);
    }
    // Load MAC Address from EEPROM
    for ( index = 0; index < 6; index++){
      mac[index] = EEPROM.read(EE_MAC_ADDR + index);
    }

    // Load IP Address from EEPROM
    for ( index = 0; index < 4; index++){
      ip[index] = EEPROM.read(EE_IP_ADDR + index);
    }

    // Load SUBNET Address from EEPROM
    for ( index = 0; index < 4; index++){
      subnet[index] = EEPROM.read(EE_MASK_ADDR + index);
    }

    // Load Gateway Address from EEPROM
    for ( index = 0; index < 4; index++){
      gateway[index] = EEPROM.read(EE_GW_ADDR + index);
    }
  }
  else
  {
    //Serial.println("Load Default");
  }
}
/*
 *=============================================================================
 *NAME:         
 *
 *DESCRIPTION:  
 *=============================================================================
*/
void saveBoardConfig(void)
{
  int index;
  Serial.println("Save board config");

  // Save Wifi atype in EEPROM
  for ( index = 0 ; index < TY_LEN; index++)
    EEPROM.write(EE_A_TYPE + index, atype[index]);

  // Save Wifi ahour in EEPROM
  for ( index = 0 ; index < HR_LEN; index++)
    EEPROM.write(EE_HOUR + index, ahour[index]);
  
  // Save Wifi SSID in EEPROM
  for ( index = 0 ; index < ID_LEN; index++)
    EEPROM.write(EE_WIFI_SSID + index, ssidSTAT[index]);
  
  // Save Wifi PASS in EEPROM
  for ( index = 0 ; index < ID_LEN; index++)
    EEPROM.write(EE_WIFI_PASS + index, passwordSTAT[index]);
  
  // Save MAC Address in EEPROM
  for ( index = 0 ; index < 6; index++)
    EEPROM.write(EE_MAC_ADDR + index, mac[index]);

  // Save IP Address in EEPROM
  for ( index = 0 ; index < 4; index++)
    EEPROM.write(EE_IP_ADDR + index, ip[index]);

  // Save Subnet Mask in EEPROM
  for ( index = 0 ; index < 4; index++)
    EEPROM.write(EE_MASK_ADDR + index, subnet[index]);

  // Save Gateway Address in EEPROM
  for ( index = 0 ; index < 4; index++)
    EEPROM.write(EE_GW_ADDR + index, gateway[index]);

  // set ID to the known bit, so when you reset the Arduino is will use the EEPROM values
  EEPROM.write(EE_ID_ADDR, ID); 
  EEPROM.commit();
  delay(300);
} 
/*
 *=============================================================================
 *NAME:         void setup(void){
 *
 *DESCRIPTION:  Essa é a função de inicialização do sistema, nessa função
 *              é feita a configuração de todoo hardware que será utilizado
 *=============================================================================
*/
void setup(void)
{
  delay(2000);
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);

  pinMode(lamp, OUTPUT);
  digitalWrite(lamp, 0);
    
  Serial.begin(115200);

  EEPROM.begin(512);  
  readBoardConfig();
  
  ESP.eraseConfig();
  WiFi.setAutoConnect(false);
  delay(300); 

  readBoardConfig();
  if ( setup_STATION() == -1){
    setup_AP();
  }

  server.on("/", handleRoot);
  server.on("/load", handleLoad);
  server.on("/conf", handleConf);
  server.on("/sensor", handleSensor);
  server.on("/cmd1", handleCmd1);
  server.on("/10", handleCmd10);
  server.on("/25", handleCmd25);
  server.on("/50", handleCmd50);
  server.on("/80", handleCmd80);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  
  timeClient.begin();
  timeClient.update();
  bAlarme = false;
}
/*
 *=============================================================================
 *NAME:         
 *
 *DESCRIPTION:   
 *=============================================================================
*/
void checkALARM(void) {
  currentMillis = millis();//Tempo atual em ms
  String fTime, sAHour;
  static int iCount = 10;
  static boolean bBlink = false;
  //Lógica de verificação do tempo
  if (bAlarme == false)
  {
    if (currentMillis - previousMillis > 1000)
    {
        previousMillis = currentMillis;    // Salva o tempo atual
        fTime = timeClient.getFormattedTime();
        sAHour = ahour;
        Serial.println(fTime.substring(0,5));
        if (fTime.substring(0,5)==sAHour)
        {
            bAlarme = true;
            Serial.println("Alarme");
        }
     }
  }
  else // bAlarme == true
  {
    if (currentMillis - previousMillis > 800)
    {
      previousMillis = currentMillis;    // Salva o tempo atual
      if (bBlink == false)
      {
        bBlink = true;
        analogWrite(lamp,800);
        Serial.println("Alarme ON");
        if ((iCount--) <= 0)
        {
          bAlarme = false;
          iCount = 10;
        }               
      }
      else
      {
        bBlink = false;
        Serial.println("Alarme Off");       
        analogWrite(lamp,0);
      }  
    }
  }
}
/*
 *=============================================================================
 *NAME:         void loop(void)
 *
 *DESCRIPTION:  Essa é a função a função principal, essa função é chamada
 *              repetidamente. 
 *=============================================================================
*/
void loop(void)
{
  server.handleClient();
  checkALARM();
  
  //Depuracao
  if(Serial.available()){
    byte valor = Serial.read();
    switch (valor){
      case '1':
        timeClient.update();
        Serial.println(timeClient.getFormattedTime());           
      break;
      case '2':           
      break;     
    }
  }        
}

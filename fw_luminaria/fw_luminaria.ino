#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char *ssid = "luminaria";
const char *password = "";

ESP8266WebServer server(80);

const int led = 0;
const int lamp =5;

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
void handleCmd1() {
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

  Serial.println();
  Serial.print("Configuring access point...");

  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  Serial.println("");
  
  server.on("/", handleRoot);
  server.on("/sensor", handleSensor);
  server.on("/cmd1", handleCmd1);
  server.on("/10", handleCmd10);
  server.on("/25", handleCmd25);
  server.on("/50", handleCmd50);
  server.on("/80", handleCmd80);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
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
}

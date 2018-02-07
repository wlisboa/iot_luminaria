#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

#include "index.h"

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

char ssidSTAT[ID_LEN]     = "";
char passwordSTAT[ID_LEN] = "";

char atype[TY_LEN] = "luz";
char ahour[HR_LEN] = "06:00";

// Network Parameters
uint8_t mac[]    = {0x00,0x00,0x00,0x00,0x00,0x00};
IPAddress ip(192,168,0,54);
IPAddress subnet(255,255,255,0);
IPAddress gateway(192,168,0,1);

ESP8266WebServer server(80);

const int led = 0;
const int lamp = 5;
const int buzzer = 16;

/*
 *=============================================================================
 *NAME:         void handleRoot()
 *
 *DESCRIPTION:  Essa função é usada pelo servidor web e é responsavel por
 *              enviar uma pagina para o navegador.
 *=============================================================================
*/
void handleRoot() 
{
  digitalWrite(led, 0);
  server.send(200, "text/html", INDEX_page);
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
void handleLoad() 
{
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
 *NAME:         void handleConf()
 *
 *DESCRIPTION:  
 *
 *=============================================================================
*/
void handleConf()
{
  String sArg, sSub;
  char cIp[20];
  int firstPoint;
  
  Serial.println("Handle Config");
  
  if (server.hasArg("desp"))
  {
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
	  server.send(200, "text/html", CONF_page);
   }
   else
   {
	  server.send(200, "text/html", CTRL_page);
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
  digitalWrite(led, 1);
  server.send ( 200, "text/text", timeClient.getFormattedTime()); 
  digitalWrite(led, 1);
}


/*
 *=============================================================================
 *NAME:         void handleCmd1()
 *
 *DESCRIPTION:
 *=============================================================================
*/
void handleCmd1()
{ 
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
void handleBuzzer() {
  Serial.println("Buzzer");
  
  digitalWrite(buzzer,HIGH);
  delay(200);
  digitalWrite(buzzer,LOW);
  delay(100);
  digitalWrite(buzzer,HIGH);
  delay(300);
  digitalWrite(buzzer,LOW);
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
int setup_STATION()
{
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

  analogWriteFreq(1000);

  pinMode(lamp, OUTPUT);
  digitalWrite(lamp, 0);
    
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, 0);
  
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
  server.on("/buzzer", handleBuzzer);
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
  String fTime, sAHour, sAType;
  static int iCount;
  static boolean bBlink = false;
  //Lógica de verificação do tempo
  if (bAlarme == false)
  {
    if (currentMillis - previousMillis > 1000)
    {
        previousMillis = currentMillis;    // Salva o tempo atual
        fTime = timeClient.getFormattedTime();
        sAHour = ahour;
        sAHour += ":00";
        //Serial.print("sAHour");
        //Serial.println(sAHour);
        //Serial.println(fTime);
        if (fTime==sAHour) //.substring(0,5)
        {
            bAlarme = true;
            iCount = 40;
            Serial.println("Alarme");
        }
     }
  }
  else // bAlarme == true
  {
    sAType = atype;
    if (sAType == "luz")
    {
        if (currentMillis - previousMillis > 500)
        {
          previousMillis = currentMillis;    // Salva o tempo atual
          if (bBlink == false)
          {
            bBlink = true;
            analogWrite(lamp,800);
            Serial.println("Alarme ON");
            iCount = iCount - 2; // O alarme pisca 20 vezes
            if (iCount <= 0)
            {
              bAlarme = false;
            }               
          }
          else{
            bBlink = false;
            Serial.println("Alarme Off");       
            analogWrite(lamp,0);
          }  
        }
    }
    else{
        if (currentMillis - previousMillis > 200)
        {
          previousMillis = currentMillis;    // Salva o tempo atual
          if (bBlink == false)
          {
            bBlink = true;
            digitalWrite(buzzer,HIGH);
            Serial.println("Alarme ON");
            if ((iCount--) <= 0)
            {
              bAlarme = false;
              digitalWrite(buzzer,LOW);
            }               
          }
          else{
            bBlink = false;
            Serial.println("Alarme Off");       
            digitalWrite(buzzer,LOW);
          }  
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

static int valor1 = 0;

void loop(void)
{  
  server.handleClient();
  checkALARM();
  
  // Depuracao
  if (Serial.available())
  {
    byte valor = Serial.read();
    switch (valor)
	{
      case '1':           
      break;

      case '2':          
      break;     
    }
  }        
}

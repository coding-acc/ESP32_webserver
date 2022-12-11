#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <Arduino.h>
#include <ArduinoJson.h>


const char* ssid = "PX-1001-1522";
const char* password = "ac171a4363";
String gptest = "188:299";
String windspeed = "55";
String windpressure = "100";
String temperature = "40";
String altitude = "150";
long previous = 0;
long interval = 1000;
String command="";

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len)
{
  
  if(type == WS_EVT_CONNECT)
  {
    Serial.println("Websocket client connection received...");
    sendmsg("1");
  } 
  
  else if(type == WS_EVT_DISCONNECT)
  {
  Serial.println("Client disconnected...");

  }
   
  else if (type == WS_EVT_DATA)
  {
    Serial.println("message received...");
    decodemsg(arg, data, len);
    //sendmsg();
  /**  Serial.println("input detected");
    for(int i=0; i<len; i++)
    {
      Serial.print((char)data[i]);
      
    }
    Serial.println();
    //String num = "poop";
    //toClient(num);
    //Serial.println();**/
  }
}

void decodemsg(void *arg, uint8_t *data, size_t len)
{
 
  Serial.println("decoding message...");
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) 
    {
        const uint8_t size = JSON_OBJECT_SIZE(1);
        StaticJsonDocument<size> json; 
        DeserializationError err = deserializeJson(json, data);
        if (err) 
        {
            Serial.print(F("deserializeJson() failed with code "));
            Serial.println(err.c_str());
            return;
        }
        String message = json["message"];
        
      command = comm(message);
    Serial.println(message);
    }   
    if (command== "A")
    {
      Serial.println(command);
      //servo();
    }
    else if (command=="B")
    {
      Serial.println(command);
      //reverse servo direction code
      //servoUp()
      //send confirmation
      sendmsg("3");
 
    }
    else if(command.toInt()>=0)
    {
      int servo_pos=command.toInt();
      servo(servo_pos);
    }
}
 

void sendmsg(String comm)
{ 
  Serial.println("attempting send...");
  const uint8_t size = JSON_OBJECT_SIZE(1)+200;
  StaticJsonDocument<size> json;
  
  if (comm == "1")
  {

  json["gps"] = getGps();
  String k = json["gps"];
  Serial.println(k);

  json["temperature"]=getTemp(temperature);
  String t=json["temperature"];
  Serial.println(t);

  json["windspeed"]=getWs(windspeed);
 
  String w=json["windspeed"];
  Serial.println(w);
 json["pressure"] = getWp(windpressure);
 json["altitude"] = getAltitude(altitude);
  char data[200];
  size_t len = serializeJson(json,data);
  ws.textAll(data,len);
  }
  else if (comm=="2")
  {
   json["message"]="arm extended";
  char data[200];
   size_t len = serializeJson(json,data);
   ws.textAll(data,len); 
  }
  else if(comm=="3")
  {
     json["message"]="Sample Collected";
   char data[200];
   size_t len = serializeJson(json,data);
   ws.textAll(data,len);
  }
  
}
void setup() 
{
  Serial.begin(115200);
  if( !SPIFFS.begin())
  {
    Serial.println("Error mounting SPIFFS");
    while(1);
  }
 WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/final.html", "text/html");
  });
   ws.onEvent(onWsEvent);
  server.addHandler(&ws);
  server.begin();
}

void servo(int pos)
{
  sendmsg("2");
  Serial.println(pos);
  //code to control servo
}
String getGps()
{
 //gps parsed from gps 
String gps="-29.857000 / S 29° 51' 25.2 : 31.024700 / E 31° 1' 28.919";
  return gps;
}
String getTemp(String temp)
{
  return temp;
}
String getAltitude(String alt)
{
  return alt;
}
String getWs(String ws)
{
  return ws;
}
String getWp(String wp)
{
  return wp;
}

String comm(String c)
{
  return c;
}
/**if(gps.location.isValid())
{
  String lat=(gps.location.lat());
  String longt = (gps.location.lng());

  Serial.println(lat);
  Serial.println(longt);
  String g = lat+""+longt;

  getGps(g);
}**/

void loop() 
{
  ws.cleanupClients();
  unsigned long current = millis();
  delay(100);
/**if(current-previous>interval)
  {
    previous = current;
    sendmsg("1");
  }**/
}

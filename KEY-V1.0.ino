#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* DATOS DEL SERVIDOR  *********************************/

#define AIO_SERVER      "52.70.203.194" // io.adafruit.com
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "dgorostieta"
#define AIO_KEY         "40fe8b094faf450ebeb083965cbb0dec"

/************ DECLARACIONES PARA SERVIDOR MQTT ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client ;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_USERNAME, AIO_KEY);

/****************************** FEEDS ***************************************/

// Setup a feed called 'onoff' for subscribing to changes to the button
Adafruit_MQTT_Subscribe salidaDigital = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/salidaDigital", MQTT_QOS_1);


/*************************** FUNCION CALLBACK ************************************/

void digitalCallback(char *data, uint16_t len) {
  Serial.print("LLAMADO DEL SERVIDOR EL BOTON ESTA: ");
  Serial.println(data);

     String message = String(data);
      message.trim();
      if (message == "ON") {digitalWrite(D7, HIGH); AnalogRead(A0);}
      if (message == "OFF") {digitalWrite(D7, LOW);} 
}




//-------------------VARIABLES GLOBALES--------------------------
int contconexion = 0;

const char *ssid = "Nimbus_WiFi";
const char *password = "N1m8u5N3t";

unsigned long previousMillis = 0;

//----------------------------- SETUP INICIO ------------------------------------

void setup() {

  pinMode(D7, OUTPUT); // D6 salida digital
  pinMode(A0, INPUT); // 
  digitalWrite(D7, LOW);


  // Inicia Serial
  Serial.begin(115200);
  Serial.println("");

  // Conexión WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED and contconexion <50) { //Cuenta hasta 50 si no se puede conectar lo cancela
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
  if (contconexion <50) {
      //para usar con ip fija
      IPAddress ip(192,168,15,150); 
      IPAddress gateway(192,168,15,1); 
      IPAddress subnet(255,255,255,0); 
      WiFi.config(ip, gateway, subnet); 
      
      Serial.println("");
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
  }
  else { 
      Serial.println("");
      Serial.println("Error de conexion");
  }

  salidaDigital.setCallback(digitalCallback);
  mqtt.subscribe(&salidaDigital);
  
}

//--------------------------LOOP--------------------------------
void loop() {

  MQTT_connect();

  
   

  // this is our 'wait for incoming subscription packets and callback em' busy subloop
  // try to spend your time here:
  mqtt.processPackets(500);
  
  // ping the server to keep the mqtt connection alive
  // NOT required if you are publishing once every KEEPALIVE seconds
  
  // if(! mqtt.ping()) {
  //   mqtt.disconnect();
  // }
}

//===========================FUNCION PARA CONEXION CON MQQTT================

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 10 seconds...");
       mqtt.disconnect();
       delay(10000);  // wait 10 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}


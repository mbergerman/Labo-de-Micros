// Grupo 1 - LM

// Board NodeMCU 1.0 (ESP-12 Module) 

#include <ESP8266WiFi.h> 
#include <PubSubClient.h>   
/***************************************
 * PubSubClient Documentation
 * https://pubsubclient.knolleary.net/
 **************************************/ 
 						
// Mqtt Stuff

// ****************************** Cloud *********************************************
#define  CLOUD_MQTT  0         //https://www.cloudmqtt.com/ (Amazon Web Services)
#define  CLOUD_DANY 1         //http://www.dioty.co/       (colombian Server)
#define  CLOUD_HOME  2         //At home Local network      (At home)
//==========Select cloud======================

#define CLOUD CLOUD_HOME

//============================================
#if CLOUD==CLOUD_HOME

IPAddress MqttServer(192,168,1,107);				// MQTT server URL or IP (see ipconfig -> IPv4)
const unsigned int MqttPort=1883; 					// MQTT port (default 1883)
const char MqttUser[]="user";		              // user name 
const char MqttPassword[]="password";				  // user password
const char MqttClientID[]="ESP-Amica-Mati";		// Device ID (warning: must be unique) 

#elif CLOUD==CLOUD_DANY

IPAddress MqttServer(54,232,42,75);    
const unsigned int MqttPort=1883; 
const char MqttUser[]="itba.jacoby@gmail.com";
const char MqttPassword[]="password";
const char MqttClientID[]="Mati";

#elif CLOUD==CLOUD_MQTT

const char MqttServer[]="m12.cloudmqtt.com";
const unsigned int MqttPort=16111; 
const char MqttUser[]="user";
const char MqttPassword[]="password";
const char MqttClientID[]="aname";

#endif

// Create Cloud sockets
WiFiClient wclient;
PubSubClient mqtt_client(wclient);
//=====================================================================
// WiFi credentials
char ssid[] = "TimTam";
char password[] = "wolfram123";   // Set password to "" for open networks.

//char ssid[] = "NODE_RED";
//char password[] = "GEDA2016";   // Set password to "" for open networks.
//=====================================================================

//Board defifinitions 
 
//======================
#define HARD AMICA              // Select Board Type or External pin
//======================

#define AMICA 0
#define LOLIN 1
#define EXTERN_PIN 2
  
// NODEMCU Boards & pin defs
#define AMICA_OnBoard_LED D0    // AMICA NodeMCU OnBoadrd LED  (on board blue led same as D0 but inverted)
#define LOLIN_OnBoard_LED D4    // LOLIN NodeMCU V3 OnBoadrd LED  (on board blue led same as D4 but inverted)
#define External_LED D1         // External led on D1

// **** Pin polarity **** 
// Board
#define Board_LED_OFF  1
#define Board_LED_ON   0
// External
#define External_LED_OFF  0
#define External_LED_ON   1
// **********************//
// Select here the LED pin to be used: Board Build In led or External Pin//
#if HARD == AMICA
#define MYLED   AMICA_OnBoard_LED        // User LED
#define LED_ON  Board_LED_ON
#define LED_OFF Board_LED_OFF

#elif HARD == LOLIN
#define MYLED   LOLIN_OnBoard_LED        // User LED
#define LED_ON  Board_LED_ON
#define LED_OFF Board_LED_OFF

#elif HARD ==  EXTERN_PIN
#define MYLED   External_LED        // User LED
#define LED_ON  External_LED_ON
#define LED_OFF External_LED_OFF
#endif



/////////////////////////////////////////////
#define OFF  LOW
#define ON HIGH


#define MQTT_STS  D6 
#define STATUS_LED MYLED //Wifi - broker connection status indicator

// Status Led (blue)
// If it blinks fast: Unable to connect to Wifi network
// If it blinks every 2 seconds: Unable to find Broker 
// If it Stays on device ready to operate  


// *************************************************
//
// 				Print debug messages macro
//
// *************************************************
#define DEBUG_OFF  0
#define DEBUG_ON   1

#define debug DEBUG_OFF

#define debug_message(fmt,...)          \
  do {              \
    if (debug)          \
       Serial.printf (fmt, ##__VA_ARGS__);     \
  } while(0)

//==================================================

void setup_wifi(void);    
void setup_mqtt(void);
void get_color_number(char* ptr1, char* ptr2, char* const color);
void reconnect_();

long TimeSinceLastMeasure = 0;
uint32_t Ldr_delayMS=1000;  


unsigned char ldr_enable=true;
unsigned char count_ldr=0;

void setup() {

  Serial.begin(9600, SERIAL_8N1);  // Serial Port Parameters

    // Setup Digital Pins
    pinMode(STATUS_LED , OUTPUT);   //Wifi & MQTT server Status led 
       
    // Pins initial values     
    digitalWrite(STATUS_LED, LED_OFF);

    // Setup Wifi and MQTT  
    delay(1000);
    setup_wifi();          // initialize WIFI an connect to network
    setup_mqtt();          // initialize mqtt server 
}



#define INBOX_SIZE 1
#define TOPIC_SIZE 1
#define PAYLOAD_SIZE 1
#define PACKAGE_SIZE (TOPIC_SIZE+PAYLOAD_SIZE)
#define TOPIC(w)    (w=='B'?"brillo":( \
                    w=='S'?"suspender":( \
                    w=='P'?"position":( \
                    w=='L'?"ldr":"otro"))))

typedef struct {
    char topic[TOPIC_SIZE+1];
    byte payload[PAYLOAD_SIZE+1];
} package_t;


package_t inbox = {0,0,0,0};
package_t* inbox_ptr = &inbox;
//package_t* read_inbox = inbox_ptr;

bool ready_to_pub = true;

byte brightness = 0;
byte ldr = 0;
char suspended = 'A';
byte pos = 0;


void loop() {

// Check MQTT conection is still active

    if (!mqtt_client.connected()) {
        reconnect_();
    }

// This should be called regularly to allow the client 
// to process incoming messages and maintain
// its connection to the server

    mqtt_client.loop();  


    if (Serial.available()>=PACKAGE_SIZE) {
        Serial.readBytes(inbox.topic, TOPIC_SIZE);
        Serial.readBytes(inbox.payload, PAYLOAD_SIZE);
    }

    if(inbox.topic[0] == 'S') {
        if(suspended != inbox.payload[0]) {
           suspended = inbox.payload[0];
            mqtt_client.publish( String(TOPIC(inbox.topic[0])).c_str() , String(inbox.payload[0]).c_str(), false); //publish suspend
      
        }
    }

    if(inbox.topic[0] == 'B') {
        if(brightness != inbox.payload[0]) {
            brightness = inbox.payload[0];
            mqtt_client.publish( String(TOPIC(inbox.topic[0])).c_str() , String(inbox.payload[0]).c_str(), false); //publish brightness
        }
    }

    if(inbox.topic[0] == 'P') {
        if(pos != inbox.payload[0]) {
            pos = inbox.payload[0];
            byte posx = (inbox.payload[0] >> 4); //payload = xxxxyyyy
            byte posy = inbox.payload[0] & 0xF;
            byte position_payload[11] = {'(','x',',','y',')','=','(',(byte)('0'+posx),',',(byte)('0'+posy),')'};
            mqtt_client.publish( String(TOPIC(inbox.topic[0])).c_str() , position_payload, 11, false); //publish position
        }
    }

    if(inbox.topic[0] == 'L') {
        if(ldr != inbox.payload[0]) {
            ldr = inbox.payload[0];
            mqtt_client.publish( String(TOPIC(inbox.topic[0])).c_str() , String(inbox.payload[0]).c_str(), false); //publish brightness
        }
    }



} // End of main Loop

//=======================SET UP WiFi==================================================================
void setup_wifi(void) {

// Start connecting to a WiFi network
 
  debug_message("\n\nConnecting to %s\n",ssid);
  
  WiFi.softAPdisconnect (true);   // Shut Down SoftAP
  WiFi.disconnect();    		  // If warm reset (not power down esp) ensure to signal router that old conection has finished
  WiFi.begin(ssid, password);     // Connect to WiFi network
  
  // Wait until WiFi conection is done
  while (WiFi.status() != WL_CONNECTED) { 
    
    digitalWrite(STATUS_LED,LED_OFF);
    delay(250);
    debug_message(".");
    digitalWrite(STATUS_LED,LED_ON);
    delay(250);
  }

  digitalWrite(STATUS_LED,LED_OFF);

  debug_message("\nWiFi Connected!!  IP Address:%s \n\n",WiFi.localIP().toString().c_str() );

  
}

//=======================SET UP MQTT==================================================================
void setup_mqtt(void) {
  
//http://pubsubclient.knolleary.net/api.html   (Arduino mqtt API)

 mqtt_client.setServer(MqttServer, MqttPort);
 mqtt_client.setCallback(callback);
}


void callback(char* topic, byte* payload, unsigned int length) {

     payload[length]=0; // Set terminator
      
     //debug_message("Message arrived [ Topic:%s Length:%d Payload: %s ] \n",topic,length,payload);
    
     ParseTopic(topic,payload,length);

}



// ============================== MQTT PARSER ==================================================

void ParseTopic(char* topic, byte* payload, unsigned int length)
{

    if(!strcmp(topic,"color"))  
    {
    
        char* rgb = (char*)payload;
    
        char* rptr1 = strchr(rgb, '(');
        char* rptr2 = strchr(rgb, ',');
        char red[4] = {0};
        get_color_number(rptr1,rptr2,red);
        
        char* gptr1 = strchr(rgb, ' ');
        char* gptr2 = strchr(gptr1, ',');
        char green[4] = {0};
        get_color_number(gptr1,gptr2,green);
        
        char* bptr1 = strchr(gptr2, ' ');
        char* bptr2 = strchr(bptr1, ')');
        char blue[4] = {0};
        get_color_number(bptr1,bptr2,blue);
    
        Serial.write('r');
        Serial.write((byte)atoi(red));
        Serial.write('g');
        Serial.write((byte)atoi(green));
        Serial.write('b');
        Serial.write((byte)atoi(blue));
    }

    else if(!strcmp(topic,"brillo_nodered")) {
        char number_str[4] = {0};
        for(unsigned int i=0; i<length; i++) {
            number_str[i] = *(payload+i);
        }
        Serial.write('B');
        Serial.write((byte)atoi(number_str));
    }

    else if(!strcmp(topic,"suspender_nodered")) {

        Serial.write('S');
        Serial.write(!suspended+'0'); //toggle;
    }

    
    else if(!strcmp(topic,"amplitud")) {
        char number_str[6] = {0};
        for(unsigned int i=0; i<length; i++) {
            number_str[i] = *(payload+i);
        }
        Serial.write('V');
        Serial.write((byte)(atof(number_str)*10.0));
    }

    
    else if(!strcmp(topic,"frequency")) {
        char number_str[6] = {0};
        for(unsigned int i=0; i<length; i++) {
            number_str[i] = *(payload+i);
        }
        Serial.write('F');
        Serial.write((byte)atoi(number_str));
    }

    else if(!strcmp(topic,"adaptative")) {
        char bool_str[6] = {0};
        for(unsigned int i=0; i<length; i++) {
            bool_str[i] = *(payload+i);
        }
        if(!strcmp(bool_str,"true")) {
            Serial.write('M');
            Serial.write((byte)1);
        }
        else if(!strcmp(bool_str,"false")) {
            Serial.write('M');
            Serial.write((byte)0);
        }
    }
    
}

void get_color_number(char* ptr1, char* ptr2, char* const color) {
    int cant_digits = ptr2-ptr1-1;
    switch(cant_digits) {
        case 1:
            color[0] = '0';
            color[1] = '0';
            color[2] = *(ptr2-1);
            break;
        case 2:
            color[0] = '0';
            color[1] = *(ptr2-2);
            color[2] = *(ptr2-1);
            break;
        case 3:
            color[0] = *(ptr2-3);
            color[1] = *(ptr2-2);
            color[2] = *(ptr2-1);
            break;
    }
}


void reconnect_() {

  while (!mqtt_client.connected())      // Loop until we're reconnected
  {
      debug_message("Attempting MQTT connection...");
   
	    // Try to connect to broker
      if (mqtt_client.connect(MqttClientID,MqttUser,MqttPassword))
      {
            debug_message("Connected \r\n"); 
			      digitalWrite(STATUS_LED,LED_ON);
  
            // ... and subscribe to topics
           
//                    mqtt_client.subscribe("#");
            mqtt_client.subscribe("color");
            mqtt_client.subscribe("brillo_nodered");
            mqtt_client.subscribe("suspender_nodered");
            mqtt_client.subscribe("amplitud");
            mqtt_client.subscribe("frequency");
            mqtt_client.subscribe("adaptative");
      }
      else
      {
		    // If conection is not established just make a short blink
      
            debug_message("failed, rc=");       //For rc codes see https://pubsubclient.knolleary.net/api.html#state
            debug_message("%d",mqtt_client.state());
            debug_message(" try again in 2 seconds \r\n");
        
            delay(2000);  //Wait 2 seconds before retrying
            digitalWrite(STATUS_LED,LED_ON);
            delay(200);  //Short 200 ms blink
            digitalWrite(STATUS_LED,LED_OFF);
      }
      
  } // end of while
 
}// End reconnect

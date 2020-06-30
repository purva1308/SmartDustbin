#include <PubSubClient.h>
#include <ESP8266WiFi.h>

#define TRIGGER 2 //2
#define ECHO    0 //2

#define WIFI_AP "*****"
#define WIFI_PASSWORD "*****"

const char* mqttServer = "xx.xxx.xx.xxx";
const int mqttPort = xxxx;

#define TOKEN "xxxxxxxxxxxxx"

char thingsboardServer[] = "********";

WiFiClient espClient; //2
PubSubClient client(espClient); //2


//WiFiClient wifiClient;

//PubSubClient client(wifiClient);

int value=0; //2
int value1=0; //2

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void setup()
{
  Serial.begin(9600);
  delay(10);
  pinMode(TRIGGER, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(5, OUTPUT);
   Serial.begin (9600);
  InitWiFi();
  client.setServer( thingsboardServer, portNumber );
  lastSend = 0;
}

void loop()
{
  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 1000 ) { // Update and send only after 1 seconds
    getAndSendSensorData();
    lastSend = millis();
  }

  client.loop();
}

void getAndSendSensorData()
{
  Serial.println("Collecting Sensor data.");
  float duration,distance;
  digitalWrite(TRIGGER, LOW);  
  delayMicroseconds(20); 

  digitalWrite(TRIGGER, HIGH);
  delayMicroseconds(10); 

  
  digitalWrite(TRIGGER, LOW);
  duration = pulseIn(ECHO, HIGH);
  distance = (duration/2) / 29.1;
  
  // Reading temperature or humidity takes about 250 milliseconds!
  //float h = 100;
  // Read temperature as Celsius (the default)
  //float t = 30;

  // Check if any reads failed and exit early (to try again).
  if (isnan(duration) || isnan(distance)) {
    Serial.println("Failed to read from sensor!");
    return;
  }

  Serial.print("Duration: ");
  Serial.print(duration);
  Serial.print(" %\t");
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" *C ");

  String Distance = String(distance);
  String Duration = String(duration);


  // Just debug messages
  Serial.println( "Sending distance and duration : [" );
  Serial.println( Distance ); Serial.print( "," );
  Serial.println( Duration );
  Serial.println( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"Distance\":"; payload += Distance; payload += ",";
  payload += "\"Duration\":"; payload += Duration;
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish("v1/devices/me/telemetry", attributes);
  Serial.println("Data Published");
  Serial.println(attributes);

}

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network

  WiFi.begin(WIFI_AP, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to AP");
}


void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    status = WiFi.status();
    if ( status != WL_CONNECTED) {
      WiFi.begin(WIFI_AP, WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      }
      Serial.println("Connected to AP");
    }
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("ESP8266 Device", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );

//------*----*------*-------*--------*-------*------*--------*--------*------*------*-----*------*-----*-

    //For client Attributes

      float firmware_version,serial_number;

      String Firmware_version = String(firmware_version);
      String Serial_number = String(serial_number);

       // Prepare a JSON payload string
  String payload = "{";
  payload += "\"Firmware_version\":"; payload += "1.0.1"; payload += ",";
  payload += "\"Serial_number\":"; payload += "SmartDustbin";
  payload += "}";

  // Send payload
  char telemetry[100];
  payload.toCharArray( telemetry, 100 );
  client.publish("v1/devices/me/attributes", telemetry);
  Serial.println("Data Published");
  Serial.println(telemetry);

//----*----*------*-------*------*--------*--------*------*------*---------*-------*-------*------*------
  
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

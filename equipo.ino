// Import required libraries
#include <WiFiGeneric.h>
#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

const char* ssid = "wifi";
const char* password = "";
const char* PARAM_INPUT = "in";
const char* PARAM_INPUT_1 = "value";

// setting PWM properties
const int freq = 5000;
const int ledChannel = 0;
const int resolution = 8;

// Stores LED state
String in0State;
String in1State;
String in2State;
String in3State;
String sliderValue = "0";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

String getOnOff(int Pin) {
  String State = "";
  if(digitalRead(Pin)){
	State = "ON";
  }
    else{
    State = "OFF";
  }

  return State;
}

String sendMesures() {
  
  in1State = getOnOff(0);
  in2State = getOnOff(1);
  in1State = getOnOff(2);
  in2State = getOnOff(3);
  
  String json = "{\"in0\":\"" + String(in1State) + "\",";
  json += "\"in1\":\"" + String(in1State) + "\",";
  json += "\"in2\":\"" + String(in1State) + "\",";
  json += "\"in3\":\"" + String(in2State) + "\"}";

  return json;
}

void setup(){
  // Serial port for debugging purposes
	Serial.begin(115200);
	pinMode(LED_BUILTIN, OUTPUT);
	pinMode(1, OUTPUT);
	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	
	ledcSetup(ledChannel, freq, resolution);

	// Initialize SPIFFS
	if(!SPIFFS.begin()){
		Serial.println("An Error has occurred while mounting SPIFFS");
		return;
	}
	
	// Connect to Wi-Fi
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.println("Connecting to WiFi..");
	}

	// Print ESP32 Local IP Address
	Serial.println(WiFi.localIP());
	
	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(SPIFFS, "/index.html", "text/html");
	});
	
	server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(SPIFFS, "/bootstrap.min.css", "text/css");
	});
	
	server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(SPIFFS, "/style.css", "text/css");
	});
	
	server.on("/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(SPIFFS, "/bootstrap.min.js", "application/javascript");
	});
	
	server.on("/jquery.min.js", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(SPIFFS, "/jquery.min.js", "application/javascript");
	});	

	server.on("/estado", HTTP_GET, [](AsyncWebServerRequest *request){
		request->send(200, "application/json", sendMesures());
	});

	// Route to set GPIO to HIGH
	server.on("/on", HTTP_GET, [](AsyncWebServerRequest *request){
		String inputMessage;
		inputMessage = request->getParam(PARAM_INPUT)->value();
		digitalWrite(LED_BUILTIN, HIGH);    
		request->send(200, "text/plain", "OK");
	});
  
	// Route to set GPIO to LOW
	server.on("/off", HTTP_GET, [](AsyncWebServerRequest *request){
		String inputMessage;
		inputMessage = request->getParam(PARAM_INPUT)->value();
		digitalWrite(LED_BUILTIN, LOW);    
		request->send(200, "text/plain", "OK");
	});
  
	// Route to set GPIO to LOW
	server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request){
		String inputIn;
		String inputValue;
		int valor;
		inputIn = request->getParam(PARAM_INPUT)->value();
		inputValue = request->getParam(PARAM_INPUT_1)->value();
		valor = inputValue.toInt() * 25;
		ledcAttachPin(LED_BUILTIN, ledChannel);
		ledcWrite(ledChannel, valor);   
		request->send(200, "text/plain", "OK");
	}); 

	// Start server
	server.begin();
}

void loop() {
  
  }


 

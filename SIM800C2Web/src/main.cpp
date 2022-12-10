#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Arduino.h>
// #include <Regexp.h>
#include <LittleFS.h>

// const char *ssid = "sga50";
// const char *password = "mhhk16133";
const char *ssid = "jaD-Link";
const char *password = "1234567890";

AsyncWebServer server(80);

IPAddress local_IP(192, 168, 0, 101);
IPAddress gateway(192, 168, 0, 254);

IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional
char RXBuffer[25600];

#define SIM800C_Serial Serial2

bool isATOk = false;
bool requestFromFunctions = false;

int8_t CheckCModuleommunication(Stream &, int = 250, bool = false);
int8_t GetSignalStrength(Stream &, int = 250, bool = false);
String SendReceiveCustomCommand(Stream &, String, int = 250, bool = true, bool = false);
void PrintDebugMessage(Stream &, String);

void setup()
{
	Serial.begin(115200);
	SIM800C_Serial.begin(115200);

	if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
	{
		Serial.println("STA Failed to configure");
	}

	Serial.print("Connecting to ");
	Serial.println(ssid);
	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
	}

	Serial.println("");
	Serial.println("WiFi connected.");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());

	// Initialize LittleFS
	if (!LittleFS.begin())
	{
		Serial.println("An Error has occurred while mounting LittleFS");
		return;
	}

	server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
		  { request->send(200, "text/plain", "Hello World From Async Web Server!"); });

	server.on("/inbox", HTTP_GET, [](AsyncWebServerRequest *request)
		  { request->send(LittleFS, "/inbox.html"); });

	server.on("/get_pdu_smss", HTTP_GET, [](AsyncWebServerRequest *request)
		  { request->send(200, "text/plain", SendReceiveCustomCommand(SIM800C_Serial, "AT+CMGL=4,1", 1000)); });

	server.on("/di", HTTP_GET, [](AsyncWebServerRequest *request)
		  { request->send(200, "text/plain", SendReceiveCustomCommand(SIM800C_Serial, "AT+CMGDA=6")); });

	// UI Libs
	server.on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request)
		  { request->send(LittleFS, "/favicon.ico"); });
	server.on("/bootstrap.bundle.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
		  { request->send(LittleFS, "/bootstrap.bundle.min.js"); });

	server.on("/bootstrap.min.css", HTTP_GET, [](AsyncWebServerRequest *request)
		  { request->send(LittleFS, "/bootstrap.min.css"); });

	server.on("/bootstrap.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
		  { request->send(LittleFS, "/bootstrap.min.js"); });

	server.on("/inbox.js", HTTP_GET, [](AsyncWebServerRequest *request)
		  { request->send(LittleFS, "/inbox.js"); });

	server.on("/jquery-3.6.1.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
		  { request->send(LittleFS, "/jquery-3.6.1.min.js"); });
	server.on("/pdudecoder.js", HTTP_GET, [](AsyncWebServerRequest *request)
		  { request->send(LittleFS, "/pdudecoder.js"); });
	server.on("/popper.min.js", HTTP_GET, [](AsyncWebServerRequest *request)
		  { request->send(LittleFS, "/popper.min.js"); });
	// UI Libs

	server.begin();

	Serial.println(SendReceiveCustomCommand(SIM800C_Serial, "AT+CMGL=4,1", 5000));
}

void loop()
{
	// delay(5000);
	// AT+CMGL=1 Lists ALL messages!
	// if (!requestFromFunctions)
	// if (SIM800C_Serial.available() > 0)
	// {
	// 	// delay(500);
	// 	Serial.println(SIM800C_Serial.readString()); // Forward what Software Serial received to Serial Port
	// }
	// if (Serial.available() > 0)
	// {
	// 	delay(500);
	// 	SIM800C_Serial.println(Serial.readString()); // Forward what Software Serial received to Serial Port
	// }
}

// void PrintDebugMessage(Stream &DEBUG_Serial, String input_str)
// {
// 	DEBUG_Serial.print("\n\n*************** DEBUG ***************\n");
// 	DEBUG_Serial.print("R: \"");
// 	DEBUG_Serial.print(input_str);
// 	DEBUG_Serial.print("\"\nL: ");
// 	DEBUG_Serial.println(input_str.length());
// 	DEBUG_Serial.print("A: ");
// 	for (uint8_t i = 0; i < input_str.length(); i++)
// 	{
// 		DEBUG_Serial.print((uint8_t)input_str[i]);
// 		if (i < input_str.length() - 1)
// 			DEBUG_Serial.print(", ");
// 	}
// 	DEBUG_Serial.print("\n*************** DEBUG ***************\n\n");
// }

// int8_t CheckModuleCommunication(Stream &GSM_ModuleSerial, int answer_timeout, bool debug)
// {
// 	String resp = SendReceiveCustomCommand(GSM_ModuleSerial, "AT", answer_timeout, true, debug);
// 	if (!resp.equals("NDR"))
// 	{
// 		if (resp.equalsIgnoreCase("at\r\r\nok\r\n"))
// 			return 1;
// 		else
// 			return 0;
// 	}
// 	else
// 		return -2;
// }

// int8_t GetSignalStrength(Stream &GSM_ModuleSerial, int answer_timeout, bool debug)
// {
// 	String resp = SendReceiveCustomCommand(GSM_ModuleSerial, "AT+CSQ", answer_timeout, true, debug);
// 	if (!resp.equals("NDR"))
// 	{
// 		if (resp.substring(resp.length() - 4).equalsIgnoreCase("ok\r\n"))
// 		{
// 			return resp.substring(resp.indexOf(':') + 1, resp.indexOf(',')).toInt();
// 		}
// 		else
// 			return -1;
// 	}
// 	else
// 		return -2;
// }

// String SendReceiveCustomCommand(Stream &GSM_ModuleSerial, String command, int answer_timeout, bool flush, bool debug)
// {
// 	requestFromFunctions = true;
// 	GSM_ModuleSerial.println(command);
// 	delay(answer_timeout);
// 	if (GSM_ModuleSerial.available() > 0)
// 	{
// 		String resp = GSM_ModuleSerial.readString();
// 		if (debug)
// 		{
// 			PrintDebugMessage(Serial, resp);
// 		}
// 		if (flush)
// 			GSM_ModuleSerial.flush();
// 		requestFromFunctions = false;
// 		return resp;
// 	}
// 	else
// 	{
// 		GSM_ModuleSerial.flush();
// 		requestFromFunctions = false;
// 		return "NDR"; // No Data Received!
// 	}
// }

String SendReceiveCustomCommand(Stream &GSM_ModuleSerial, String command, int answer_timeout, bool flush, bool debug)
{
	if (flush)
		GSM_ModuleSerial.flush();
	requestFromFunctions = true;
	unsigned long startingTime = millis();
	uint16_t rxCounter = 0, txCounter = 0;
	String resp = "";

	while (GSM_ModuleSerial.available())
	{
		GSM_ModuleSerial.read();
	}

	GSM_ModuleSerial.setTimeout(answer_timeout);
	GSM_ModuleSerial.println(command);
	// delay(50);
	while (millis() - startingTime < answer_timeout)
	{
		if (GSM_ModuleSerial.available())
		{
			RXBuffer[rxCounter] = GSM_ModuleSerial.read();
			// Serial.println(millis() - startingTime);
			rxCounter++;
			// delay(1);
		}
	}
	Serial.print("Count: ");
	Serial.println(rxCounter);
	Serial.print("Data: ");
	while (txCounter < rxCounter)
	{
		// Serial.print(RXBuffer[txCounter]);
		resp += RXBuffer[txCounter];
		txCounter++;
	}

	return resp;
}

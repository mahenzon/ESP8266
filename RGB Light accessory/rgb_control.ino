#include "ESPHelper.h" // you need to download this library from here https://github.com/ItKindaWorks/ESPHelper and add to Arduino IDE

#define TOPIC "/home/RGBLight"  // topic that ESP8266 will be monitoring for new commands
#define STATUS TOPIC "/status"

#define RED_PIN 16    
#define GREEN_PIN 14
#define BLUE_PIN 12
#define POWER_SWITCH_PIN 13


typedef struct lightColor{
	int red;
	int green;
	int blue;
};


enum superModes {STANDBY, SET, MOOD};
enum updateTypes{RGB, POWER};


lightColor currentColor;
lightColor newColor;

int updateType = 0;


int superMode = STANDBY;
boolean newCommand = false;


char* myChipName = "myESP8266";
char* lightTopic = TOPIC;
char* statusTopic = STATUS;


const int redPin = RED_PIN;
const int greenPin = GREEN_PIN;
const int bluePin = BLUE_PIN;
const int builtInLED = 2; // on ESP 12 buitin led is on GPIO 2
const int powerPin = POWER_SWITCH_PIN;

char statusString[30];  //string containing the current setting for the light


//set this info for your own network
netInfo homeNet = {.name = myChipName, .mqtt = "YOUR MQTT-IP", .ssid = "YOUR SSID", .pass = "YOUR NETWORK PASS"};
ESPHelper myESP(&homeNet);


// If you want to use your chip with multiple networks modify next lines and remove that above

/*
netInfo homeNet1 = {.name = myChipName, .mqtt = "YOUR MQTT-IP1", .ssid = "YOUR SSID1", .pass = "YOUR NETWORK PASS1"};
netInfo homeNet2 = {.name = myChipName, .mqtt = "YOUR MQTT-IP2", .ssid = "YOUR SSID2", .pass = "YOUR NETWORK PASS2"};
netInfo homeNet3 = {.name = myChipName, .mqtt = "YOUR MQTT-IP3", .ssid = "YOUR SSID3", .pass = "YOUR NETWORK PASS3"};

netInfo *knownNetworks[3] = {
	&homeNet1,
	&homeNet2,
	&homeNet3
};

ESPHelper myESP(knownNetworks, 3);   
*/

void setup() {
	pinMode(builtInLED, OUTPUT); 
	pinMode(redPin, OUTPUT);
	pinMode(bluePin, OUTPUT);
	pinMode(greenPin, OUTPUT);
	pinMode(powerPin, OUTPUT);

	//all off
	digitalWrite(powerPin, LOW);
	digitalWrite(redPin, LOW);
	digitalWrite(greenPin, LOW);
	digitalWrite(bluePin, LOW);
	digitalWrite(builtInLED, HIGH);  // Builtin LED is off on logical 1  
	
	currentColor.red = 255;
	currentColor.green = 255;
	currentColor.blue = 255;
	delay(1000);

	colorTest();


	myESP.addSubscription(lightTopic);
	myESP.begin();
	myESP.setCallback(callback);

	myESP.publish(statusTopic, "Starting main", true);
	myESP.enableHeartbeat(builtInLED);
}



void loop(){
	if(myESP.loop() == FULL_CONNECTION){
		lightHandler();
	}
	yield();
}


void lightHandler(){
	if(newCommand){
		if (updateType == POWER){
			if (superMode == STANDBY){
				myESP.enableHeartbeat(builtInLED);

				analogWrite(redPin, 0);
				analogWrite(greenPin, 0);
				analogWrite(bluePin, 0);

				digitalWrite(powerPin, LOW);
			}else if (superMode == SET){
				digitalWrite(powerPin, HIGH);
				myESP.disableHeartbeat();


				analogWrite(redPin, currentColor.red);
				analogWrite(greenPin, currentColor.green);
				analogWrite(bluePin, currentColor.blue);

			}
			
		}else if (updateType == RGB){
		
			if(currentColor.red != newColor.red){
				currentColor.red = newColor.red;
				analogWrite(redPin, newColor.red * 4);
			}
			if(currentColor.green != newColor.green){
				analogWrite(greenPin, newColor.green * 4);
				currentColor.green = newColor.green;
			}
			if (currentColor.blue != newColor.blue){
				analogWrite(bluePin, newColor.blue * 4);
				currentColor.blue = newColor.blue;
			}
		}
	newCommand = false;
	}
}



//MQTT callback
void callback(char* topic, byte* payload, unsigned int length) {

	char newPayload[20];
	memcpy(newPayload, payload, length);
	newPayload[length] = '\0';

	if(payload[0] == 'p'){
		updateType = POWER;
		newCommand = true;
		if(payload[1] == '1'){
			superMode = SET;
		}
		else if(payload[1] == '0'){
			superMode = STANDBY;
		}
	}

	else if (payload[0] == 'c'){
		if (superMode == STANDBY){
			myESP.publish(statusTopic, "Power is off. Poweron first.", true);
		}else{
			updateType = RGB;
			newCommand = true;
			int newRed = atoi(&newPayload[1]);
			int newGreen = atoi(&newPayload[5]);
			int newBlue = atoi(&newPayload[9]);

			newColor.red = newRed;
			newColor.green = newGreen;
			newColor.blue = newBlue;
		}
	}

// uncomment next two lines to get notified on mqtt topic when command is proccessed
	// strcpy(statusString, newPayload);
	// myESP.publish(statusTopic, statusString, true);
}


void colorTest(){
	digitalWrite(redPin, HIGH);
	delay(300);
	digitalWrite(redPin, LOW);
	digitalWrite(greenPin, HIGH);
	delay(300);
	digitalWrite(greenPin, LOW);
	digitalWrite(bluePin, HIGH);
	delay(300);
	digitalWrite(bluePin, LOW);
	delay(200);

	digitalWrite(redPin, HIGH);
	digitalWrite(greenPin, HIGH);
	digitalWrite(bluePin, HIGH);
	delay(777);
	digitalWrite(redPin, LOW);
	digitalWrite(greenPin, LOW);
	digitalWrite(bluePin, LOW);
}

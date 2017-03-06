#include "ESPHelper.h" // you need to download this library from here https://github.com/ItKindaWorks/ESPHelper and add to Arduino IDE

#define TOPIC "/home/RGBLight1"  // topic that ESP8266 will be monitoring for new commands
#define STATUS TOPIC "/status"

#define RED_PIN_LEFT 16    
#define GREEN_PIN_LEFT 14
#define BLUE_PIN_LEFT 12

#define RED_PIN_RIGHT 5    
#define GREEN_PIN_RIGHT 4
#define BLUE_PIN_RIGHT 15

#define POWER_SWITCH_PIN 13


typedef struct lightColor{
	int red;
	int green;
	int blue;
};


enum superModes {STANDBY, SET, MOOD};
enum updateTypes{RGB, POWER};
enum updateSideTypes{LEFT, RIGHT};

// for controling 2 RGB strips with one ESP8266 chip
lightColor currentColorLeft;
lightColor newColorLeft;
lightColor currentColorRight;
lightColor newColorRight;

int updateType = 0;
int updateSide = 0;

int superMode = STANDBY;  // global mode for chip. STANDBY when both connected strips are off
boolean newCommand = false;

// to check if both are not poweredon disable powerunit
boolean isPoweredOn = false;
boolean isPoweredOnLeft = false;
boolean isPoweredOnRight = false;

char* myChipName = "myESP8266";
char* lightTopic = TOPIC;
char* statusTopic = STATUS;



const int redPinLeft = RED_PIN_LEFT;
const int greenPinLeft = GREEN_PIN_LEFT;
const int bluePinLeft = BLUE_PIN_LEFT;

const int redPinRight = RED_PIN_RIGHT;
const int greenPinRight = GREEN_PIN_RIGHT;
const int bluePinRight = BLUE_PIN_RIGHT;

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
	pinMode(builtInLED, OUTPUT);  //init builtin led as output
	digitalWrite(builtInLED, HIGH);  // Builtin LED is off on logical 1

	pinMode(powerPin, OUTPUT);
	digitalWrite(powerPin, LOW);  //init power pin to toggle power supply

	pinMode(redPinLeft, OUTPUT);
	pinMode(bluePinLeft, OUTPUT);
	pinMode(greenPinLeft, OUTPUT);

	pinMode(redPinRight, OUTPUT);
	pinMode(bluePinRight, OUTPUT);
	pinMode(greenPinRight, OUTPUT);
	
	//all off
	digitalWrite(redPinLeft, LOW);
	digitalWrite(greenPinLeft, LOW);
	digitalWrite(bluePinLeft, LOW);

	digitalWrite(redPinRight, LOW);
	digitalWrite(greenPinRight, LOW);
	digitalWrite(bluePinRight, LOW);

	
	currentColorLeft.red = 255;
	currentColorLeft.green = 255;
	currentColorLeft.blue = 255;

	currentColorRight.red = 255;
	currentColorRight.green = 255;
	currentColorRight.blue = 255;


	delay(1000);

	colorTest();


	myESP.addSubscription(lightTopic);
	myESP.begin();
	myESP.setCallback(callback);

	myESP.enableHeartbeat(builtInLED);
	myESP.publish(statusTopic, "Starting main", true);
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

				// powering off left strip
				digitalWrite(redPinLeft, LOW);
				digitalWrite(greenPinLeft, LOW);
				digitalWrite(bluePinLeft, LOW);

				// powering off right strip
				digitalWrite(redPinRight, LOW);
				digitalWrite(greenPinRight, LOW);
				digitalWrite(bluePinRight, LOW);

				// as no strip is ON, disabling PC power supply unit
				digitalWrite(powerPin, LOW);
			}else if (superMode == SET){
				if(!isPoweredOn){  // powering ON power supply unit
					digitalWrite(powerPin, HIGH);
					isPoweredOn = true;
					myESP.publish(statusTopic, "Enabled power supply unit", true);
				}
				if(updateSide == LEFT){
					if (isPoweredOnLeft){
						analogWrite(redPinLeft, currentColorLeft.red);
						analogWrite(greenPinLeft, currentColorLeft.green);
						analogWrite(bluePinLeft, currentColorLeft.blue);
					} else {
						digitalWrite(redPinLeft, LOW);
						digitalWrite(greenPinLeft, LOW);
						digitalWrite(bluePinLeft, LOW);
					}
				}
				else if (updateSide == RIGHT){
					if (isPoweredOnRight){
						analogWrite(redPinRight, currentColorRight.red);
						analogWrite(greenPinRight, currentColorRight.green);
						analogWrite(bluePinRight, currentColorRight.blue);
					} else {
						digitalWrite(redPinRight, LOW);
						digitalWrite(greenPinRight, LOW);
						digitalWrite(bluePinRight, LOW);
					}
				}
			}
			
		}else if (updateType == RGB){
			if (updateSide == LEFT){
				if(currentColorLeft.red != newColorLeft.red){
					analogWrite(redPinLeft, newColorLeft.red);
					currentColorLeft.red = newColorLeft.red;
				}
				if(currentColorLeft.green != newColorLeft.green){
					analogWrite(greenPinLeft, newColorLeft.green);
					currentColorLeft.green = newColorLeft.green;
				}
				if(currentColorLeft.blue != newColorLeft.blue){
					analogWrite(bluePinLeft, newColorLeft.blue);
					currentColorLeft.blue = newColorLeft.blue;
				}
				
			}else if (updateSide == RIGHT){
				if(currentColorRight.red != newColorRight.red){
					analogWrite(redPinRight, newColorRight.red);
					currentColorRight.red = newColorRight.red;
				}
				if(currentColorRight.green != newColorRight.green){
					analogWrite(greenPinRight, newColorRight.green);
					currentColorRight.green = newColorRight.green;
				}
				if(currentColorRight.blue != newColorRight.blue){
					analogWrite(bluePinRight, newColorRight.blue);
					currentColorRight.blue = newColorRight.blue;
				}	
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

	if(payload[0] == 'p'){  // if there is new power change
		updateType = POWER;
		newCommand = true;
		// check if second letter is 'l' or 'r' to get side
		if(payload[1] == 'l'){  // here is 'l' (small 'L') for 'left'
			updateSide = LEFT;
			if(payload[2] == '1'){  // 1 for power ON
				isPoweredOnLeft = true;
			} else if (payload[2] == '0'){
				isPoweredOnLeft = false;
			}
		}else if(payload[1] == 'r'){  // 'r' for right RGB strip
			updateSide = RIGHT;
			if(payload[2] == '1'){ 
				isPoweredOnRight = true;
			} else if (payload[2] == '0'){
				isPoweredOnRight = false;
			}
		}
		if(isPoweredOnLeft || isPoweredOnRight){
			superMode = SET;
			myESP.disableHeartbeat();
			digitalWrite(builtInLED, HIGH);  // Builtin LED is off on logical 1
		} else {
			isPoweredOn = false;
			superMode = STANDBY;
			myESP.enableHeartbeat(builtInLED);
		}
	}
	else if(payload[0] == 'c'){
		if(superMode == STANDBY){
			myESP.publish(statusTopic, "Power is off. Poweron first.", true);
		}else{
			updateType = RGB;
			newCommand = true;
			int newRed = atoi(&newPayload[2]);
			int newGreen = atoi(&newPayload[6]);
			int newBlue = atoi(&newPayload[10]);
			if(payload[1] == 'l'){
				updateSide = LEFT;
				newColorLeft.red = newRed;
				newColorLeft.green = newGreen;
				newColorLeft.blue = newBlue;
			}else if(payload[1] == 'r'){
				updateSide = RIGHT;
				newColorRight.red = newRed;
				newColorRight.green = newGreen;
				newColorRight.blue = newBlue;
			}
		}
	}
   strcpy(statusString, newPayload);
   myESP.publish(statusTopic, statusString, true);
}


void colorTest(){
	digitalWrite(redPinLeft, HIGH);
	digitalWrite(redPinRight, HIGH);
	delay(333);
	digitalWrite(redPinLeft, LOW);
	digitalWrite(redPinRight, LOW);
	digitalWrite(greenPinLeft, HIGH);
	digitalWrite(greenPinRight, HIGH);
	delay(333);
	digitalWrite(greenPinLeft, LOW);
	digitalWrite(greenPinRight, LOW);
	digitalWrite(bluePinLeft, HIGH);
	digitalWrite(bluePinRight, HIGH);
	delay(333);
	digitalWrite(bluePinLeft, LOW);
	digitalWrite(bluePinRight, LOW);
	delay(228);

	digitalWrite(redPinLeft, HIGH);
	digitalWrite(redPinRight, HIGH);
	digitalWrite(greenPinLeft, HIGH);
	digitalWrite(greenPinRight, HIGH);
	digitalWrite(bluePinLeft, HIGH);
	digitalWrite(bluePinRight, HIGH);
	delay(777);
	digitalWrite(redPinLeft, LOW);
	digitalWrite(redPinRight, LOW);
	digitalWrite(greenPinLeft, LOW);
	digitalWrite(greenPinRight, LOW);
	digitalWrite(bluePinLeft, LOW);
	digitalWrite(bluePinRight, LOW);
}

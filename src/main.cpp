#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>
#include "OneLed.h"
//#include "button.h"
#include "LedBlink.h"
#include "iled.h"

BH1750 lightMeter(0x23);

// Update these with values suitable for your network.

const char* ssid = "ivanych";
const char* password = "stroykomitet";
const char* mqtt_server = "192.168.1.34";
const int BUILTIN_LED = 17; //управление включением света
const int BUILTIN_LED_CHANEL {0};
const int IR_DATA = 34; //датчик движения
const int PIN_BUTTON = 16;
const int LED_BLINK_R = 26;  //мигающий светодиод
// const int LED_BLINK_G = 25;  //мигающий светодиод
// const int LED_BLINK_B = 27;  //мигающий светодиод
//i2c sda-21, scl-22
OneLed light(BUILTIN_LED_CHANEL);
WiFiClient espClient;
PubSubClient client(espClient);
Timer tMotion(20000);
Iled iled(LED_BLINK_R);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];
int value{};


const char* LedBlink_G="helga_table/ledBlinkG";
const char* LedBlink_B="helga_table/ledBlinkB";
const char* apb="helga_table/press_button";
const char* msg_motion="helga_table/motion";
const char* extLight = "helga_table/ext_light";
const char* topic_security = "helga_table/security";
const char* topic_security_on = "helga_table/security_on";
const char* TopicMaxLevel = "helga_table/maxLevel";
const char* TopicMaxLevelTime = "helga_table/maxLevelTime";
const char* Topic_Light = "helga_table/light";
const char* Topic_Lux = "helga_table/lux";
volatile int buttonStatus{};
volatile bool ir_motion{};
bool ledStatus{};
bool irLightOn{}; //свет включен по таймеру
bool lightStat{}; //состояние внешнего света
bool hardOn{};  //принудительное включение света
float lux{};  //яркость света в помещении
bool security{};  //стоит на охране
//************************************************** обработка нажатия на кнопку
const uint32_t TIME_SHORT {600};
const uint32_t TIME_DOUBLE {500};
const uint32_t TIMER_DOUBLE_PRESS {400};
const float LIGHT_LEVEL_BH {1.};
// const int PIN_BUTTON = 16;

enum class StatsButton {
  NONE,
  SHORT_PRESS,
  LONG_PRESS,
  DOUBLE_PRESS
};

StatsButton volatile statsButton {StatsButton::NONE};

uint32_t volatile t1{};
uint32_t volatile t2{};
uint32_t volatile t3{};

void IRAM_ATTR push_button_down();
//------------------------------------
void IRAM_ATTR push_button_up(){
    t2 = millis();
    if(t2 - t1 >= TIME_SHORT) {
      statsButton = StatsButton::LONG_PRESS;
    }
     else {
      if( t3 > 0 )
        statsButton = StatsButton::SHORT_PRESS;
    }
  attachInterrupt(PIN_BUTTON, push_button_down, RISING);
}
//------------------------------------
void IRAM_ATTR push_button_down(){
    t1 = millis();
    if( t1 - t3 < TIME_DOUBLE){
      statsButton = StatsButton::DOUBLE_PRESS;
      t2 = t3 = 0;
    } else t3 = t1;
  attachInterrupt(PIN_BUTTON, push_button_up, FALLING);
};
//********************************
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

/*
aisle/maxLevel - максимальная яркость
aisle/light - включение / выключение
aisle/motion - обнаружино движение
aisle/ext_light - включен свет
aisle/press_button - была нажата кнопка
*/
//***********************************************************************
void callback(char* topic, byte* payload, unsigned int length) {
  String str = {};
  String strTopic = topic;
  for (int i = 0; i < length; i++) {
    str += (char)payload[i];
  }
  if(strTopic == TopicMaxLevelTime){
    light.setMaxLevel(str.toInt());
  } else if(strTopic == TopicMaxLevel){
    light.setMaxLevel(str.toInt());
		light.setStat(StatLed::ON);
    hardOn = true;
  } else if(strTopic == Topic_Light){
    if ((char)payload[0] == '1') {
      hardOn = true;
      light.setStat(StatLed::ON);
    } else {
      light.setStat(StatLed::OFF);
      hardOn = false;
    }
  } else if (strTopic == topic_security_on){
			iled.blink();
	}
}
//******************************************************
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "Olga_table-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
//      client.publish("outTopic", "hello world");
      client.subscribe(Topic_Light);
      client.subscribe(TopicMaxLevel);
      client.subscribe(TopicMaxLevelTime);
			client.subscribe(topic_security_on);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//*********************************************
void ir_interr(){
  ir_motion = true;
//  Serial.println("ir_interr");
}
//*********************************************
void setup() {
  Serial.begin(115200);
  delay(3000);
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  pinMode(IR_DATA, INPUT);  //Датчик движения
  pinMode(PIN_BUTTON, INPUT);  //кнопка
  ledcSetup(BUILTIN_LED_CHANEL, 500, 8);
  ledcAttachPin(BUILTIN_LED, BUILTIN_LED_CHANEL);

  Serial.println("**********");
  setup_wifi();
  Wire.begin();
  //---------------------------
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  //........................
  attachInterrupt(digitalPinToInterrupt(PIN_BUTTON), push_button_down, RISING);
  attachInterrupt(digitalPinToInterrupt(IR_DATA), ir_interr, RISING);
  // //---------------------------
  // if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
  //   Serial.println(F("BH1750 Advanced begin"));
  // }
  // else {
  //   Serial.println(F("Error initialising BH1750"));
  // }
  // //------------------
  Serial.println("**********");
}
//************************************
float getLuxs(BH1750 *lightMeter, float &lux){
	if (lightMeter->measurementReady()) 
		lux = lightMeter->readLightLevel();
		// Serial.println(lux);
	return lux;
}
//************************************
 void ir_motion_func(){
 if(ir_motion){
    ir_motion = false;
    if(!irLightOn) {
      client.publish(msg_motion, "1");
      irLightOn = true;
    }
    tMotion.setTimer();
		lux = getLuxs(&lightMeter, lux);
		client.publish(Topic_Lux, (String(lux)).c_str());
    if(lux <= LIGHT_LEVEL_BH){
      light.setStat(StatLed::ON);
    }
  }
}
//************************************  
void fShort(){
  if(security){
    security = false;
    iled.off();
    client.publish(topic_security, "0");
  } else {
    iled.blink(1);
    hardOn = !hardOn;
    light.setStat(hardOn? StatLed::ON : StatLed::OFF);
    const char* msg = hardOn? "1":"0";
    client.publish(apb, msg);
  }
}
//************************************* 
void fDouble(){
  security = !security;
  if(security){
    hardOn = false;
    iled.off();
    light.setStat(StatLed::OFF);
  }
  iled.blink();
  client.publish(topic_security, (security? "1": "0"));
}
//************************************
void fLong(){
	if(security){
		security = false;
		iled.off();
		client.publish(topic_security, "0");
	} else {
		light.setMaxLevel(light.getMaxLevel() == 255? 30 : 255);
		iled.blink(2);
		if(!hardOn){
			hardOn = true;
			light.setStat(StatLed::ON);
		}
	}
}
//************************************
void loop() {

	if (!client.connected()) {
		reconnect();
	}
	client.loop();
	ir_motion_func();  
	//.................................
	if(statsButton != StatsButton::NONE){
		switch (statsButton)
		{
		case StatsButton::SHORT_PRESS:
			fShort();
			break;
		case StatsButton::DOUBLE_PRESS:
			fDouble();
			break;
		case StatsButton::LONG_PRESS:
			fLong();
		default:
			break;
		}
		statsButton = StatsButton::NONE;
	}
	//.................................
	// unsigned long now = millis();
	// if (now - lastMsg > 1000) {                                                                 
	// 	lux = getLuxs(&lightMeter, lux);
	// 	lastMsg = now;
	// 	if(lux < LIGHT_LEVEL_BH && lightStat){
	// 		client.publish(extLight, "0");
	// 		lightStat = false;
	// 	} else if(lux >= LIGHT_LEVEL_BH && !lightStat){
	// 		client.publish(extLight, "1");
	// 		lightStat = true;
	// 	}
	// }
	light.cycle();
	iled.cycle();
	//------------------------------------------ IR
	if(irLightOn && tMotion.getTimer()){
		client.publish(msg_motion, "0");
		irLightOn = false;
		if(!hardOn){
			light.setStat(StatLed::OFF);
		}    
	}
}
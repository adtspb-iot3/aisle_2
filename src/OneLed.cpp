#include "OneLed.h"

//*************************************************
void OneLed::setDim(int l) {
	levelDim = l; 
}
//***********************************************
void OneLed::trigger(){
	switch (statLed) {
		case StatLed::ON:levelDim = 0; statLed = StatLed::OFF; break;
		case StatLed::BLINK: break;
		case StatLed::DIM: break;
		case StatLed::OFF:levelDim = maxLevel; statLed = StatLed::ON; break;
	}
}
//*********************************************
void OneLed::setMaxLevel(int i){
	maxLevel = i;
	if(statLed == StatLed::ON) {
		levelDim = maxLevel;
	}
}
//*********************************************
void OneLed::setStat(StatLed stat) {
	statLed = stat; 
	switch (statLed) {
		case StatLed::OFF:
			levelDim = 0; 
			setDt(LONG_DT);
			break;
		case StatLed::BLINK:break;
		case StatLed::ON:
			levelDim = maxLevel; 
			setDt(SHORT_DT);
			break;
		case StatLed::DIM:break;
	}
}
//*************************************
void OneLed::cycle() {
	if (level != levelDim) {
		if (getTimer()) {
			setTimer();
			level += (level < levelDim? 1: -1);
		}
	}
	ledcWrite(ledChannel, level);
}


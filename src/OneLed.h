// OneLed.h

#ifndef _ONELED_h
#define _ONELED_h
#include "Timer.h"

const uint16_t LONG_DT {50};
const uint16_t SHORT_DT {20};

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
	#include "WProgram.h"
#endif

enum class StatLed { OFF, ON, BLINK, DIM };

class OneLed:	public Timer
{
	int level{ 0 };
	int levelDim{ 0 };
	int maxLevel{100};
	StatLed statLed{ StatLed::ON };
	int ledChannel { 0 };
public:
	OneLed(int ch): Timer(SHORT_DT), ledChannel(ch) {}
	void setStat(StatLed stat);
	void trigger();	//изменение состояния на обратное
	void setDim(int l);
	void setMaxLevel(int i);
	int getMaxLevel(){return maxLevel;}
	virtual void cycle();
};

#endif


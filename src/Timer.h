#pragma once
#include<Arduino.h>

class Timer
{
	unsigned long t{ 0 };
	unsigned long dT{ 50 };	//время задержки
public:
	Timer() {}
	Timer(unsigned long dt):dT(dt) {}
	bool getTimer() { return t < millis(); }
	void setTimer(unsigned long dt = 0);
	void setDt(unsigned long dt) { dT = dt; }
};


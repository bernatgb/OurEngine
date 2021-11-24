#pragma once
#include <stdint.h>

class AccuratedTimer
{
public:
	AccuratedTimer();

	void Start();
	void Stop();
	unsigned long long int Read();

private:
	static unsigned long long int frequency;
	bool stopped;
	unsigned long long int startCounter;
	unsigned long long int timerCounter;
};



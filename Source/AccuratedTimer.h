#pragma once
#include <stdint.h>

class AccuratedTimer
{
public:
	AccuratedTimer();

	void Start();
	void Stop();
	uint64_t Read();

private:
	static uint64_t frequency;
	bool stopped;
	uint64_t startCounter;
	uint64_t timerCounter;
};



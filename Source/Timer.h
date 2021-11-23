#pragma once
#include <stdint.h>

class Timer
{
public:
	Timer();

	void Start();
	void Stop();
	uint32_t Read();

private:
	bool stopped;
	uint32_t startTime;
	uint32_t timer;
};


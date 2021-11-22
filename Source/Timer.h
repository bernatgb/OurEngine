#pragma once

class Timer
{
public:
	Timer();

	void Start();
	void Stop();
	double Read();

private:
	bool stopped;
	double startTime;
	double timer;
};


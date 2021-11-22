#pragma once
class AccuratedTimer
{
public:
	AccuratedTimer();

	void Start();
	void Stop();
	double Read();

private:
	static double frequency;
	bool stopped;
	double startCounter;
	double timerCounter;
};



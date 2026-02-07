#pragma once

#include <deque>
#include <numeric>
#include <chrono>

class Clock
{
public:
	void StartClock();
	void EndClock();
	void CalculateAverage();

private:
	void ElapsedTime();

public:
	std::chrono::time_point<std::chrono::steady_clock> startTime;
	std::chrono::time_point<std::chrono::steady_clock> endTime;

	std::deque<double> updateTimeSamples;
	const size_t maxSamples = 120;

	double currentUpdateTime = 0.0f;
	double avgUpdateTime = 0.0f;
	double maxUpdateTime = 0.0f;
};
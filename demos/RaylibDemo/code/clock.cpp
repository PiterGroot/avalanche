#include "clock.hpp"

void Clock::StartClock()
{
	startTime = std::chrono::high_resolution_clock::now();
}

void Clock::EndClock()
{
	endTime = std::chrono::high_resolution_clock::now();
}

void Clock::ElapsedTime()
{
	std::chrono::duration<double, std::milli> duration = endTime - startTime;
	currentUpdateTime = duration.count();
}

void Clock::CalculateAverage()
{
	ElapsedTime();

	// Collect samples to calculate average using maxSamples.
	updateTimeSamples.push_back(currentUpdateTime);

	if (updateTimeSamples.size() > maxSamples)
		updateTimeSamples.pop_front();

	// Calculate average.
	avgUpdateTime = updateTimeSamples.size() > 0 ? std::accumulate
	(updateTimeSamples.begin(), updateTimeSamples.end(), 0.0) / updateTimeSamples.size() : 0.0;

	// Calculate max.
	maxUpdateTime = updateTimeSamples.size() > 0 ? *std::max_element
	(updateTimeSamples.begin(), updateTimeSamples.end()) : 0.0;
}
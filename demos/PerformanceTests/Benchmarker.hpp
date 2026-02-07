#pragma once
#include <DemoCore/include/avalanche_initializer.hpp>

#include <future>
#include <atomic>

namespace avl
{
	class World;
}

class Benchmarker
{
public:
	Benchmarker();

private:
	void RunBenchmarks();
	void Benchmark(std::string testName, const float fixedTimeStep, avl::World& world);

	const char* GetLogColor(const int level);
	void Log(const int level, const char* format, ...);
	void LogInPlace(const int level, const char* format, ...);
	void LogImplementation(const int level, bool newline, const char* format, va_list args);
	static std::string CreateCenteredSeparator(const char* format, ...);

public:
	bool isBenchmarking;

private:
	AvalancheInitializer avalancheInitializer = AvalancheInitializer();
};


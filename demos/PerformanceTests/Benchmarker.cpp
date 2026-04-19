#include "Benchmarker.hpp"

#include <DemoCore/include/cell-behaviours/sand.hpp>
#include <DemoCore/include/cell-behaviours/fire.hpp>
#include <DemoCore/include/cell-behaviours/water.hpp>
#include <DemoCore/include/cell-behaviours/smoke.hpp>
#include <DemoCore/include/cell-behaviours/lava.hpp>
#include <DemoCore/include/cell-behaviours/gun_powder.hpp>
#include <DemoCore/include/cell-behaviours/plant.hpp>

#include <string>
#include <chrono>
#include <iostream>
#include <hwinfo/hwinfo.h>
#include <vector>

#define LOG_SEPARATOR   Log(0, "-----------------------------------------------------------------------");
#define LOG_SEPARATOR_TEXT(...) Log(0, CreateCenteredSeparator(__VA_ARGS__).c_str());
#define LOG_INDENTATION std::string("           -")


Benchmarker::Benchmarker()
{
    if (IsCellMovementFlagSet())
    {
        Log(2, "Flag DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS is set inside the DemoCore (common-cell-moves.hpp). Benchmarker does not support it, disable it by not setting the flag.");
        return;
    }

    const auto cpus = hwinfo::getAllCPUs();
    
    Log(0, "Avalanche engine version v%i.%i.%i",
        AVALANCHE_VERSION_MAJOR, AVALANCHE_VERSION_MINOR, AVALANCHE_VERSION_PATCH);

    for (const auto& cpu : cpus)
    {
        LOG_SEPARATOR
        Log(0, "CPU: %s %s", cpu.vendor().c_str(), cpu.modelName().c_str());
        Log(0, "Physical cores: %i", cpu.numPhysicalCores());
        Log(0, "Logical cores: %i", cpu.numLogicalCores());
        LOG_SEPARATOR
    }
    
    isBenchmarking = true;
    RunBenchmarks();
}

void Benchmarker::RunBenchmarks()
{
    // Sand.
    {
        uint32_t colorA = avl::utils::pack_RGBA(0.99f, 0.97f, 0.0f, 1.0f);
        uint32_t colorB = avl::utils::pack_RGBA(0.6f, 0.46f, 0.05f, 1.0f);
        avl::utils::register_cell(2, colorA, colorB, *avalancheInitializer.world, OnUpdateSand);
    }

    {
        avalancheInitializer.world->reset_world();
        avalancheInitializer.world->plot_rectangle(0, 0, 499, 99, 2);
        Benchmark("Sand cells (500x100)", 1.0f / 60.0f, *avalancheInitializer.world);
    }

    {
        avalancheInitializer.world->reset_world();
        avalancheInitializer.world->plot_rectangle(0, 0, 499, 199, 2);
        Benchmark("Sand cells (500x200)", 1.0f / 60.0f, *avalancheInitializer.world);
    }

    {
        avalancheInitializer.world->reset_world();
        avalancheInitializer.world->plot_rectangle(0, 0, 499, 299, 2);
        Benchmark("Sand cells (500x300)", 1.0f / 60.0f, *avalancheInitializer.world);
    }

    {
        avalancheInitializer.world->reset_world();
        avalancheInitializer.world->plot_rectangle(0, 0, 499, 399, 2);
        Benchmark("Sand cells (500x400)", 1.0f / 60.0f, *avalancheInitializer.world);
    }

    {
        avalancheInitializer.world->reset_world();
        avalancheInitializer.world->plot_rectangle(0, 0, 499, 499, 2);
        Benchmark("Sand cells (500x500)", 1.0f / 60.0f, *avalancheInitializer.world);
    }

    {
        avalancheInitializer.world->reset_world();
        
        for (int x = 0; x < 500; x += 10)
            avalancheInitializer.world->plot_rectangle(x, 0, x + 5, 399, 2);
        
        Benchmark("Vertical sand collums cells (500x300)", 1.0f / 60.0f, *avalancheInitializer.world);
    }

    {
        avalancheInitializer.world->reset_world();

        for (int x = 0; x < 500; x += 10) {
            for (int y = 0; y < 300; y += 10) {
                avalancheInitializer.world->plot_rectangle(x, y, x + 2, y + 2, 2);
            }
        }

        Benchmark("Sparse sand (500x300, 10% density)", 1.0f / 60.0f, *avalancheInitializer.world);
    }
}

void Benchmarker::Benchmark(std::string testName, const float fixedTimeStep, avl::World& world)
{
    std::vector<double> worldTimeSteps;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    auto time = startTime;
    
    int stepCount = 0;
    
    Log(0, "Running benchmark: %s at %f: s", testName.c_str(), fixedTimeStep);

    while (!world.is_sleeping())
    {
        stepCount++;

        auto ctime = std::chrono::high_resolution_clock::now();
        auto elapsed = ctime - time;
        
        avalancheInitializer.world->run_world_one_tick();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::milli> duration = endTime - ctime;
        worldTimeSteps.push_back(duration.count());
        
        LogInPlace(0, "Steps: %d | Current: %.4f ms", stepCount, worldTimeSteps.back());
        
        time = ctime;
    }
    
    printf("\n");

    auto finalTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> totalDuration = finalTime - startTime;

    double sum = 0;
    for (double sample : worldTimeSteps) sum += sample;

    LOG_SEPARATOR_TEXT("results", testName.c_str())
    Log(0, (LOG_INDENTATION + std::string("Total steps: %d in %.4f s")).c_str(), stepCount, totalDuration.count());
    Log(0, (LOG_INDENTATION + std::string("Average time step: %.4f ms")).c_str(), sum / stepCount);
    LOG_SEPARATOR
}

const char* Benchmarker::GetLogColor(const int level)
{
    switch (level)
    {
        case 0:  return "\033[32m";   // Green.
        case 1:  return "\033[1;33m"; // Bright Yellow.
        case 2:  return "\033[1;31m"; // Bright Red.
        default: return "\033[0m";    // Reset.
    }
}

void Benchmarker::Log(const int level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    LogImplementation(level, true, format, args);
    va_end(args);
}

void Benchmarker::LogInPlace(const int level, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    LogImplementation(level, false, format, args);
    va_end(args);
}

void Benchmarker::LogImplementation(const int level, bool newline, const char* format, va_list args)
{
    const char* level_str = (level == 0) ? "info" : (level == 1) ? "warn" : "error";

    printf("%s[%s%s%s] ", newline ? "" : "\r", GetLogColor(level), level_str, "\033[0m");
    vprintf(format, args);
    
    if (newline) printf("\n");
    else printf("   ");
    
    fflush(stdout);
}

std::string Benchmarker::CreateCenteredSeparator(const char* format, ...)
{
    const int totalWidth = 71;

    va_list args;
    va_start(args, format);

    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    std::string text(buffer);
    const int textLength = static_cast<int>(text.length());

    // If text is too long, return it with minimal dashes.
    if (textLength >= totalWidth - 2) {
        return "--" + text + "--";
    }

    const int remainingWidth = totalWidth - textLength;
    const int leftDashes = remainingWidth / 2;
    const int rightDashes = remainingWidth - leftDashes;

    return std::string(leftDashes, '-') + text + std::string(rightDashes, '-');
}

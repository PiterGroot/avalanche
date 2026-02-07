#pragma once
#include <avalanche.hpp>
#include <vector>
#include <memory>

class AvalancheInitializer
{
public:
	AvalancheInitializer();

public:
	std::unique_ptr<avl::World> world;

private:
	const int sectorChunkSize = 50;
	const int sectorSize = 500;
};
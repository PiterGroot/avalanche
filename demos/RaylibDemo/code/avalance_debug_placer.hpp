#pragma once
#include <raylib.h>

namespace avl
{
	class World;
}

class AvalancheDebugPlacer
{
public:
	void Update(avl::World& world, Vector2 mousePosition, int currentCellIndex) const;

public:
	float currentPlaceFillChance = 0.0f;
	int currentPlaceSize = 10;
};


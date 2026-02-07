#pragma once
#include <avalanche.hpp>

#include "demo_imgui_interface.hpp"
#include "avalanche_raylib_debug_drawer.hpp"
#include "clock.hpp"

#include <vector>
#include <raylib.h>

#include <DemoCore/include/avalanche_initializer.hpp>

class AvalancheController
{
public:
	struct CreatedSector
	{
		int sectorId;
		Texture2D texture = {};
	};

public:
	AvalancheController();

	void Update(float deltaTime, DemoImguiInterface::DEMO_MODE currentMode);
	void Draw(Camera2D& camera);
	void ForceUpdateAllTextures() const;

public:
	AvalancheInitializer avalancheInitializer;
	Clock clock;
	RaylibAvalancheDebugDrawer avalancheDebugDrawer;
	float currentFixedDeltaTime = 0;
	bool isStepping = false;
	static std::vector<CreatedSector> createdSectors;

private:
	std::vector<std::pair<avl::SimulationSector*, Texture2D*>> activeDrawList;

	float targetFrameRate = 60.0f;
	const int sectorChunkSize = 50;
	const int sectorSize = 500;
};
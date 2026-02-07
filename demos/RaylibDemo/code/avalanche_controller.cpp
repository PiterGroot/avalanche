#define AVALANCHE_DEBUG_DRAWER

#include "avalanche_controller.hpp"

std::vector<AvalancheController::CreatedSector> AvalancheController::createdSectors;

static void OnSectorCreated(const avl::SimulationSector* sector)
{
	Image sectorImage = GenImageColor(sector->width, sector->height, WHITE);
	Texture2D newSectorTexture = LoadTextureFromImage(sectorImage);
	UnloadImage(sectorImage);

	AvalancheController::createdSectors.push_back({ sector->id, newSectorTexture });
}

static void OnSectorUpdated(const avl::SimulationSector* sector)
{
	UpdateTexture(AvalancheController::createdSectors[sector->id].texture, sector->_activeCellColors);
}

static void OnWorldReset()
{
	for (auto& createdSector : AvalancheController::createdSectors)
	{
		UnloadTexture(createdSector.texture);
	}

	AvalancheController::createdSectors.clear();
}

AvalancheController::AvalancheController()
{
	avalancheInitializer.world->set_on_sector_created_listener(OnSectorCreated);
	avalancheInitializer.world->set_on_sector_updated_listener(OnSectorUpdated);
	avalancheInitializer.world->set_on_world_reset_listener(OnWorldReset);
	avalancheInitializer.world->set_debug_drawer(&avalancheDebugDrawer);
	avalancheInitializer.world->create_sector(0, 0); // Create default sector.
}

void AvalancheController::Update(float deltaTime, DemoImguiInterface::DEMO_MODE currentMode)
{
	// Start clock stopwatch to measure frametimes.
	clock.StartClock();

	if (!isStepping)
	{
		// Update the world at a fixed time step using a target frame rate.
		if (currentMode == DemoImguiInterface::DEMO_MODE::ACTIVE)
		{
			avalancheInitializer.world->step_world(deltaTime, currentFixedDeltaTime);
		}
	}
	else if (currentFixedDeltaTime > 0) // Force update the world if the simulation is stepped.
	{
		avalancheInitializer.world->run_world_one_tick();
	}

	// Calculate and save average duration in milliseconds.
	clock.EndClock();
	clock.CalculateAverage();

	avalancheInitializer.world->debug_draw();
}

void AvalancheController::Draw(Camera2D& camera)
{
	const Vector2 windowStart = GetScreenToWorld2D({ 0,0 }, camera);
	const Vector2 windowEnd = GetScreenToWorld2D(Vector2{ (float)GetScreenWidth(), (float)GetScreenHeight() }, camera);
	
	activeDrawList.clear();

	for (auto& createdSector : createdSectors)
	{
		avl::SimulationSector* sector = avalancheInitializer.world->get_sector_direct(createdSector.sectorId);
		
		Vector2 pos = { (float)sector->worldX,  (float)sector->worldY };
		Vector2 size = { (float)sector->width, (float)sector->height };

		bool intersects = !(pos.x + size.x < windowStart.x ||
			pos.y + size.y < windowStart.y ||
			pos.x > windowEnd.x ||
			pos.y > windowEnd.y);

		if (intersects)
			activeDrawList.push_back(std::make_pair(sector, &createdSector.texture));
	}

	for (auto& sectorToDraw : activeDrawList)
	{
		DrawTexture(*sectorToDraw.second, sectorToDraw.first->worldX, sectorToDraw.first->worldY, WHITE);
	}
}

void AvalancheController::ForceUpdateAllTextures() const
{
	for (auto& createdSector : createdSectors)
	{
		avl::SimulationSector* sector = avalancheInitializer.world->get_sector_direct(createdSector.sectorId);
		UpdateTexture(createdSectors[createdSector.sectorId].texture, sector->_activeCellColors);
	}
}

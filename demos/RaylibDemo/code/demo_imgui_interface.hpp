#pragma once
#include <avalanche.hpp>
#include <raylib.h>

class DemoImguiInterface
{
public:
	enum class DEMO_MODE
	{
		ACTIVE,
		INACTIVE,
		PAUSED
	};

public:
	DemoImguiInterface();
	void DrawSimulationControls(bool& showDebugOverlay, bool& isStepping, float& currentFixedTimeStep, float& targetFramerate, int& currentPlaceSize, avl::World& world);
	void DrawDebugOverlay(double averageDuration, double maxDuration, const float targetFramerate, Vector2 worldMouse, size_t debugLineCount, avl::World& world);
	void ShowMaterialButtons();

public:
	DEMO_MODE mode = DemoImguiInterface::DEMO_MODE::INACTIVE;
	int selectedMaterialIndex = 2;

private:
	Texture2D stoneTexture;
	Texture2D woodTexture;
	Texture2D steelTexture;
	Texture2D magnetNorthTexture;
	Texture2D magnetSouthTexture;
	Texture2D sandTexture;
	Texture2D lavaTexture;
	Texture2D waterTexture;
	Texture2D acidTexture;
	Texture2D smokeTexture;
	Texture2D fireTexture;
	Texture2D gunPowderTexture;
	Texture2D fireworkTexture;
	Texture2D plantTexture;
	Texture2D sparkTexture;
	Texture2D unstableMatterTexture;
	Texture2D quantumMatterTextureTexture;
};
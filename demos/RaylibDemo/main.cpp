#include <raylib.h>

#define NO_FONT_AWESOME
#include "rlImGui/extras/IconsFontAwesome6.h"
#include "rlImGui/rlImGui.h"
#include "ImGui/imgui.h"

#include "code/demo_camera.hpp"
#include "code/demo_imgui_interface.hpp"
#include "code/avalance_debug_placer.hpp"
#include "code/avalanche_controller.hpp"

int main()
{
	InitWindow(900, 600, "Avalanche Raylib demo");
	SetWindowState(FLAG_WINDOW_ALWAYS_RUN | FLAG_WINDOW_MAXIMIZED | FLAG_WINDOW_RESIZABLE);
	rlImGuiSetup(true);
	SetTraceLogLevel(LOG_ERROR);

	bool isExclusiveFullscreen = false;

	DemoCamera cameraController;
	DemoImguiInterface demoInterface;

	AvalancheDebugPlacer avalancheDebugPlacer;
	AvalancheController avalancheController;

	avalancheController.ForceUpdateAllTextures();
	
	ImGuiIO& io = ImGui::GetIO();

	float targetFrameRate = 60.0f;
	bool showDebugOverlay = true;
	bool showAnyOverlay = true;

	while (!WindowShouldClose())
	{
		float deltaTime = GetFrameTime();
		auto screenMousePosition = GetMousePosition();
		auto mousePosition = GetScreenToWorld2D(screenMousePosition, cameraController.camera);
		
		cameraController.Update(deltaTime);
		avalancheDebugPlacer.Update(*avalancheController.avalancheInitializer.world, mousePosition, demoInterface.selectedMaterialIndex);
		avalancheController.Update(deltaTime, demoInterface.mode);
		
		if (IsKeyPressed(KEY_F1))
			showAnyOverlay = !showAnyOverlay;

		size_t prevDebugLines = avalancheController.avalancheDebugDrawer.lines.size();

		if (avalancheController.isStepping)
			avalancheController.currentFixedDeltaTime = 0;
		
		avalancheController.avalancheDebugDrawer.lineThickness 
			= cameraController.camera.zoom >= 1.5f ? 0.5f : 1.5f;

		BeginDrawing();
			ClearBackground(BLANK);
			BeginMode2D(cameraController.camera);
				avalancheController.Draw(cameraController.camera);
				if (showDebugOverlay) avalancheController.avalancheDebugDrawer.Render();
				else avalancheController.avalancheDebugDrawer.lines.clear();
				DrawPixel((int)mousePosition.x, (int)mousePosition.y, {(unsigned char)255.0f, (unsigned char)255.0f, (unsigned char)255.0f, (unsigned char)255.0f});
			EndMode2D();
			
			if (showAnyOverlay)
			{
				rlImGuiBegin();
					ImGui::PushFont(io.Fonts->Fonts[1]);
					demoInterface.DrawSimulationControls(showDebugOverlay, avalancheController.isStepping, avalancheController.currentFixedDeltaTime, targetFrameRate, avalancheDebugPlacer.currentPlaceSize, *avalancheController.avalancheInitializer.world);
					if (showDebugOverlay) demoInterface.DrawDebugOverlay(avalancheController.clock.avgUpdateTime, avalancheController.clock.maxUpdateTime, targetFrameRate, mousePosition, prevDebugLines, *avalancheController.avalancheInitializer.world);
					ImGui::PopFont();
				rlImGuiEnd();
			}
		EndDrawing();

		if (IsKeyPressed(KEY_F11))
		{
			if (!isExclusiveFullscreen)
			{
				int monitorID = GetCurrentMonitor();
				SetWindowSize(GetMonitorWidth(monitorID), GetMonitorHeight(monitorID));
				ToggleFullscreen();
				isExclusiveFullscreen = true;
			}
			else
			{
				ToggleFullscreen();
				SetWindowSize(900, 600);
				isExclusiveFullscreen = false;
			}
		}
	}
}
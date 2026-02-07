#include "avalance_debug_placer.hpp"
#include "../rlImGui/rlImGui.h"
#include "../ImGui/imgui.h"

#include <avalanche.hpp>

#define CLICK_VIEWPORT_DOWN(button) \
    (IsMouseButtonDown(button) && \
     !ImGui::GetIO().WantCaptureMouse)

#define CLICK_VIEWPORT_PRESS(button) \
    (IsMouseButtonPressed(button) && \
     !ImGui::GetIO().WantCaptureMouse)

void AvalancheDebugPlacer::Update(avl::World& world, Vector2 mousePosition, int currentCellIndex) const
{
	if (IsKeyDown(KEY_LEFT_CONTROL))
	{
		if (CLICK_VIEWPORT_PRESS(MOUSE_BUTTON_LEFT))
			world.plot_cell((int)mousePosition.x, (int)mousePosition.y, currentCellIndex);
	}
	else if (CLICK_VIEWPORT_DOWN(MOUSE_BUTTON_LEFT))
		world.plot_circle((int)mousePosition.x, (int)mousePosition.y, currentPlaceSize, currentCellIndex, currentPlaceFillChance);

	if (IsKeyDown(KEY_LEFT_CONTROL))
	{
		if (CLICK_VIEWPORT_PRESS(MOUSE_BUTTON_RIGHT))
			world.plot_cell((int)mousePosition.x, (int)mousePosition.y, 0);
	}
	else if (CLICK_VIEWPORT_DOWN(MOUSE_BUTTON_RIGHT))
		world.plot_circle((int)mousePosition.x, (int)mousePosition.y, currentPlaceSize, 0, 0.0f);

	if(IsKeyPressed(KEY_F5))
	{
		auto targetSector = world.try_get_sector((int)mousePosition.x, (int)mousePosition.y);
		world.plot_rectangle(targetSector->worldX, targetSector->worldY, targetSector->worldX + 499, targetSector->worldY + 499, currentCellIndex, 0.0f);
	}
}

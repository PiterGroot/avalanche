#pragma once
#include <avalanche.hpp>
#include "raylib.h"
#include <iostream>

/// <summary>
/// Custom debug drawer inherited from avl::SimulationDebugDrawer to implement the line drawing in our raylib context.
/// </summary>
class RaylibAvalancheDebugDrawer : public avl::SimulationDebugDrawer
{
public:
	void Render()
	{
		// Render all stored line data in the same BeginDrawing() call and clear the vector when done.
		for (int i = 0; i < lines.size(); i++)
		{
			LineData lineData = lines[i];
			DrawLineEx({ (float)lineData.fromX, (float)lineData.fromY }, { (float)lineData.toX, (float)lineData.toY }, lineThickness,
				{ lineData.color.r, lineData.color.g, lineData.color.b, lineData.color.a });
		}

		lines.clear();
	}

	// Automatically called from avalanche, only need to implement the actual line rendering.
	void DrawDebugLine(int fromX, int fromY, int toX, int toY, avl::SimulationDebugDrawer::DebugDrawColor color) override
	{
		// Add new line data to vector to render in the future.
		lines.push_back({ fromX, fromY, toX, toY, color });
	}

private:
	struct LineData // Basic line data structure for the line drawing.
	{
		int fromX; int fromY;
		int toX; int toY;

		avl::SimulationDebugDrawer::DebugDrawColor color;
	};

public:
	std::vector<LineData> lines;
	float lineThickness = 1.0f;
};
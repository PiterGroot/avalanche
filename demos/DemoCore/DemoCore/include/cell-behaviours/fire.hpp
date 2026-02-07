#pragma once
#include <avalanche.hpp>
#include "../common-cell-moves.hpp"
#include "../cell-flags.hpp"

inline bool OnUpdateFire(avl::CellUpdateContext& ctx)
{
	static uint32_t colors[5] =
	{
		avl::utils::pack_RGBA(0.612f, 0.169f, 0.067f, 1.0f),
		avl::utils::pack_RGBA(1.0f, 0.416f, 0.0f, 1.0f),
		avl::utils::pack_RGBA(0.498f, 0.0f, 0.0f, 1.0f),
		avl::utils::pack_RGBA(1.0f, 0.592f, 0.0f, 1.0f),
		avl::utils::pack_RGBA(0.498f, 0.2f, 0.0f, 1.0f)
	};

	ctx.notify_chunk();
	ctx.notify_sector();

	// Get current user data.
	auto data = ctx.get_user_data();

	// Update lifetime.
	int randValue = avl::utils::get_random_value(0, 1);
	data.value += (uint16_t)randValue;

	// Determine cell lifetime based on flags.
	int cellLifeTime = 75;

	if (data.has_flag(CellFlags::LONG_LIFE_TIME))
		cellLifeTime = 150;
	else if (data.has_flag(CellFlags::SHORT_LIFE_TIME))
		cellLifeTime = 40;

	// Randomly change color.
	if (randValue == 0 && avl::utils::get_chance(0.5f))
		ctx.sector->_activeCellColors[ctx.cellIndex] = colors[avl::utils::get_random_value(0, 4)];

	// Check if fire should go out.
	if (data.value == cellLifeTime)
	{
		if (!avl::utils::get_chance(.9f))
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY, 6); // Spawn smoke.
		else
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY, 0); // Go out.

		data.value = 0;
		data.clear_all_flags();
		
		ctx.set_user_data(data);
		return false;
	}

	ctx.set_user_data(data);

	// Movement logic.
	if (!data.has_flag(CellFlags::LONG_LIFE_TIME) && !data.has_flag(CellFlags::IMMOVABLE))
	{
		if (!FloatUp(ctx, 2))
			return MoveSide(ctx);
	}

	return true;
}

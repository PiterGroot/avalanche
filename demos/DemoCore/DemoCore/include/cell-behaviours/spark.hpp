#pragma once
#include <avalanche.hpp>
#include "../common-cell-moves.hpp"
#include "../cell-flags.hpp"

inline bool OnUpdateSpark(avl::CellUpdateContext& ctx)
{
	uint32_t colors[3] = {};
	auto data = ctx.get_user_data();

	if (data.has_flag(RANDOM_COLOR))
	{
		colors[0] = avl::utils::pack_RGBA(1.0f, 0.0f, 0.0f, 1.0f);
		colors[1] = avl::utils::pack_RGBA(0.0f, 1.0f, 0.0f, 1.0f);
		colors[2] = avl::utils::pack_RGBA(0.0f, 0.0f, 1.0f, 1.0f);
	}
	else 
	{
		colors[0] = avl::utils::pack_RGBA(1.0f, 1.0f, 0.8f, 1.0f);
		colors[1] = avl::utils::pack_RGBA(0.6f, 0.8f, 1.0f, 1.0f);
		colors[2] = avl::utils::pack_RGBA(1.0f, 1.0f, 1.0f, 1.0f);
	}

	ctx.notify_chunk();
	ctx.notify_sector();

	int randValue = avl::utils::get_random_value(0, 2);

	// Flicker
	if (data.has_flag(RANDOM_COLOR))
	{ 
		if(avl::utils::get_chance(.1f))
			ctx.sector->_activeCellColors[ctx.cellIndex] = colors[randValue];
	}
	else
	{
		ctx.sector->_activeCellColors[ctx.cellIndex] = colors[randValue];
	}

	data.value += (uint16_t)randValue;

	if (data.value >= 7)
	{
		ctx.world->plot_cell(ctx.x + ctx.sector->worldX,
			ctx.y + ctx.sector->worldY, 0);

		data.value = 0;
		data.clear_all_flags();
		ctx.set_user_data(data);

		return false;
	}

	// Randomly branch and spread.
	if (data.value < 3 && avl::utils::get_chance(.78f))
	{
		int direction = avl::utils::get_random_value(-1, 1);
		int targetX = ctx.x + direction;
		int targetY = ctx.y + avl::utils::get_random_value(-1, 1);

		if (!ctx.is_out_of_bounds(targetX, targetY) && ctx.is_empty(targetX, targetY))
		{
			ctx.world->plot_cell(targetX + ctx.sector->worldX,
				targetY + ctx.sector->worldY, ctx.cellID);
			
			if (data.has_flag(RANDOM_COLOR))
			{
				auto newData = ctx.get_user_data(targetX, targetY);
				newData.set_flag(RANDOM_COLOR);
				ctx.set_user_data(targetX, targetY, newData);
			}
		}
	}

	ctx.set_user_data(data);
	return true;
}
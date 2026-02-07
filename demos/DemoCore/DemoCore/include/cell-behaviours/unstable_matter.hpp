#pragma once
#include <avalanche.hpp>
#include "../common-cell-moves.hpp"
#include "../cell-flags.hpp"

inline bool OnUpdateUnstableMatter(avl::CellUpdateContext& ctx)
{
	static uint32_t colors[6] = 
	{
		avl::utils::pack_RGBA(0.8f, 0.0f, 0.8f, 1.0f),
		avl::utils::pack_RGBA(0.6f, 0.0f, 1.0f, 1.0f),
		avl::utils::pack_RGBA(1.0f, 0.0f, 0.6f, 1.0f),
		avl::utils::pack_RGBA(0.4f, 0.0f, 0.8f, 1.0f),
		avl::utils::pack_RGBA(0.9f, 0.0f, 1.0f, 1.0f),
		avl::utils::pack_RGBA(0.7f, 0.0f, 0.9f, 1.0f)
	};

	auto data = ctx.get_user_data();

	ctx.notify_chunk();
	ctx.notify_sector();

	if (avl::utils::get_chance(0.3f))
	{
		ctx.sector->_activeCellColors[ctx.cellIndex] =
			colors[avl::utils::get_random_value(0, 5)];
	}

	data.value++;

	if (data.value > 40)
	{
		uint8_t possibleTransforms[] = { 2, 3, 5, 6, 8, 9, 11, 12, 13, 14, 7 };
		uint8_t newCell = possibleTransforms[avl::utils::get_random_value(0, 11)];

		ctx.world->plot_cell(
			ctx.x + ctx.sector->worldX,
			ctx.y + ctx.sector->worldY,
			newCell);

		return false;
	}

	if (data.value % 10 == 0 && avl::utils::get_chance(0.3f))
	{
		int offsetX = avl::utils::get_random_value(-2, 2);
		int offsetY = avl::utils::get_random_value(-2, 2);
		int targetX = ctx.x + offsetX;
		int targetY = ctx.y + offsetY;

		if (!ctx.is_out_of_bounds(targetX, targetY) && ctx.is_empty(targetX, targetY))
		{
			ctx.world->plot_cell(
				targetX + ctx.sector->worldX,
				targetY + ctx.sector->worldY,
				6);
		}
	}

	ctx.set_user_data(data);

	int behavior = avl::utils::get_random_value(0, 2);
	
	if (behavior == 0)
		return MoveDown(ctx);
	else if (behavior == 1)
		return FloatUp(ctx);
	else
		return MoveSide(ctx);
}
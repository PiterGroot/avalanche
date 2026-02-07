#pragma once
#include <avalanche.hpp>
#include "../common-cell-moves.hpp"
#include "../cell-flags.hpp"

inline bool OnUpdateSmoke(avl::CellUpdateContext& ctx)
{
	auto data = ctx.get_user_data();

	// Increment lifetime.
	int randValue = avl::utils::get_random_value(0, 2);
	data.value += (uint16_t)randValue;

	// Check if smoke should disappear.
	if (data.value >= 100)
	{
		ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY, 0); // Disappear.
		data.value = 0;
		data.clear_all_flags();
		ctx.set_user_data(data);
		return false;
	}

	ctx.set_user_data(data);

	// Movement logic.
	if (!FloatUp(ctx))
	{
		return MoveSide(ctx);
	}

	return false;
}
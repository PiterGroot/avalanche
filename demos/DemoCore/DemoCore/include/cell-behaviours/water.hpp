#pragma once
#include <avalanche.hpp>
#include "../common-cell-moves.hpp"
#include "../cell-flags.hpp"

inline bool OnUpdateWater(avl::CellUpdateContext& ctx)
{
	if (!MoveDown(ctx))
	{
		return MoveSide(ctx);
	}

	return false;
}
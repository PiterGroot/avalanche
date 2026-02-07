#pragma once
#include <avalanche.hpp>
#include "../common-cell-moves.hpp"
#include "../cell-flags.hpp"

inline bool OnUpdateSand(avl::CellUpdateContext& ctx)
{
	if (!MoveDown(ctx))
	{
		return MoveSideDown(ctx);
	}

	return false;
}
#pragma once
#include <avalanche.hpp>
#include "../cell-flags.hpp"
#include "../common-cell-moves.hpp"

inline bool OnUpdateLava(avl::CellUpdateContext& ctx)
{
	if (!MoveDown(ctx))
	{
		return MoveSide(ctx);
	}

	return false;
}
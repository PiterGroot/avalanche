#pragma once
#include <avalanche.hpp>

//#define DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS

inline bool MoveDown(avl::CellUpdateContext& ctx)
{
	const int targetY = ctx.y + 1;
	if (!ctx.is_out_of_bounds(ctx.x, targetY))
	{
		uint8_t downCellID = 0;
		if (ctx.is_empty(ctx.x, targetY, downCellID))
		{
			ctx.move_cell(ctx.x, targetY);
			return true;
		}
		else if (ctx.cellID != 3 && downCellID == 3)
		{
			if (avl::utils::get_chance(.5f))
				ctx.swap_cell(ctx.x, targetY);
			else
				ctx.notify_chunk();

			return true;
		}
	}

#ifdef DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS
	else
	{
		if (ctx.world->is_postion_empty(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY + 1))
		{
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY, 0);
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY + 1, ctx.cellID);
		}
	}
#endif // DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS
	return false;
}

inline bool MoveSide(avl::CellUpdateContext& ctx)
{
	bool outBoundsLeft = ctx.is_out_of_bounds(ctx.x - 1, ctx.y);
	bool outBoundsRight = ctx.is_out_of_bounds(ctx.x + 1, ctx.y);

	bool left = (!outBoundsLeft && ctx.is_empty(ctx.x - 1, ctx.y))
#ifdef DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS
		|| (outBoundsLeft && ctx.world->is_postion_empty(ctx.x + ctx.sector->worldX - 1, ctx.y + ctx.sector->worldY))
#endif
		;

	bool right = (!outBoundsRight && ctx.is_empty(ctx.x + 1, ctx.y))
#ifdef DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS
		|| (outBoundsRight && ctx.world->is_postion_empty(ctx.x + ctx.sector->worldX + 1, ctx.y + ctx.sector->worldY))
#endif
		;

	if (left && right)
	{
		left = avl::utils::get_chance(.5f);
		right = !left;
	}
	if (left)
	{
		if (!outBoundsLeft)
			ctx.move_cell(ctx.x - 1, ctx.y);

#ifdef DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS
		else
		{
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY, 0);
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX - 1, ctx.y + ctx.sector->worldY, ctx.cellID);
		}
#endif
	}
	else if (right)
	{
		if (!outBoundsRight)
			ctx.move_cell(ctx.x + 1, ctx.y);

#ifdef DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS
		else
		{
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY, 0);
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX + 1, ctx.y + ctx.sector->worldY, ctx.cellID);
		}
#endif
	}
	return left || right;
}

inline bool MoveSideDown(avl::CellUpdateContext& ctx)
{
	bool outBoundsLeft = ctx.is_out_of_bounds(ctx.x - 1, ctx.y + 1);
	bool outBoundsRight = ctx.is_out_of_bounds(ctx.x + 1, ctx.y + 1);

	bool downLeft = (!outBoundsLeft && ctx.is_empty(ctx.x - 1, ctx.y + 1))
#ifdef DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS
		|| (outBoundsLeft && ctx.world->is_postion_empty(ctx.x + ctx.sector->worldX - 1, ctx.y + ctx.sector->worldY + 1))
#endif
		;

	bool downRight = (!outBoundsRight && ctx.is_empty(ctx.x + 1, ctx.y + 1))
#ifdef DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS
		|| (outBoundsRight && ctx.world->is_postion_empty(ctx.x + ctx.sector->worldX + 1, ctx.y + ctx.sector->worldY + 1))
#endif
		;

	if (downLeft && downRight) {
		downLeft = avl::utils::get_chance(0.5f);
		downRight = !downLeft;
	}

	if (downLeft) {
		if (!outBoundsLeft)
			ctx.move_cell(ctx.x - 1, ctx.y + 1);
#ifdef DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS
		else {
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY, 0);
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX - 1, ctx.y + ctx.sector->worldY + 1, ctx.cellID);
		}
#endif
		return true;
	}
	else if (downRight) {
		if (!outBoundsRight)
			ctx.move_cell(ctx.x + 1, ctx.y + 1);
#ifdef DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS
		else {
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY, 0);
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX + 1, ctx.y + ctx.sector->worldY + 1, ctx.cellID);
		}
#endif
		return true;
	}

	return false;
}

inline bool MoveUp(avl::CellUpdateContext& ctx)
{
	const int targetY = ctx.y - 1;
	bool canMoveUp = !ctx.is_out_of_bounds(ctx.x, targetY) && ctx.is_empty(ctx.x, targetY);
	if (canMoveUp)
		ctx.move_cell(ctx.x, targetY);
	else if (ctx.is_out_of_bounds(ctx.x, targetY))
	{
#ifdef DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS
		if (ctx.world->is_postion_empty(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY - 1))
		{
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY, 0);
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY - 1, ctx.cellID);
			canMoveUp = true;
		}
#endif
	}
	return canMoveUp;
}

inline bool FloatUp(avl::CellUpdateContext& ctx, int amount = 1)
{
	int randomValue = avl::utils::get_random_value(0, amount);
	const int targetY = ctx.y - randomValue;
	bool isEmpty = !ctx.is_out_of_bounds(ctx.x, targetY) && ctx.is_empty(ctx.x, targetY);

	if (isEmpty)
		ctx.move_cell(ctx.x, targetY);
	else if (ctx.is_out_of_bounds(ctx.x, targetY))
	{
#ifdef DEMO_CORE_DO_CELL_MOVEMENT_BETWEEN_SECTORS
		if (ctx.world->is_postion_empty(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY - randomValue))
		{
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY, 0);
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY - randomValue, ctx.cellID);
			isEmpty = true;
		}
#endif
	}
	return isEmpty;
}
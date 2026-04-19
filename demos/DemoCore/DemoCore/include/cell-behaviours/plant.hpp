#pragma once
#include <avalanche.hpp>
#include "../common-cell-moves.hpp"
#include "../cell-flags.hpp"

void PlotFlower(int worldX, int worldY, avl::CellUpdateContext& ctx);

inline bool OnUpdatePlant(avl::CellUpdateContext& ctx)
{
	auto data = ctx.get_user_data();

	if (!data.has_flag(CellFlags::PLANT_SEED) && !data.has_flag(CellFlags::GROWN_PLANT))
	{
		ctx.notify_chunk();
		ctx.notify_sector();

		// Increment lifetime.
		data.value += (uint16_t)(avl::utils::get_random_value(0, 1));

		if (data.value % 20 == 0)
		{
			if (data.value >= 75)
			{
				data.clear_flag(CellFlags::PLANT_SEED);
				data.set_flag(CellFlags::GROWN_PLANT);
				ctx.set_user_data(data);
				return false;
			}

			int targetX = ctx.x;
			int targetY = ctx.y - 1;

			if (avl::utils::get_chance(0.7f))
			{
				if (ctx.is_empty(targetX, targetY))
				{
					int randomHeight = avl::utils::get_random_value(3, 6);
					int randomDirection = avl::utils::get_random_value(-6, 6);
					ctx.world->plot_line(targetX + ctx.sector->worldX, targetY + ctx.sector->worldY,
						targetX + ctx.sector->worldX + randomDirection,
						targetY + ctx.sector->worldY - randomHeight, 16);
					ctx.world->plot_cell(targetX + ctx.sector->worldX, targetY + ctx.sector->worldY, 9);

					// Clear plant seed flag on target cell.
					auto targetData = ctx.get_user_data(targetX, targetY);
					targetData.clear_flag(CellFlags::PLANT_SEED);
					ctx.set_user_data(targetX, targetY, targetData);

					PlotFlower(targetX + ctx.sector->worldX + randomDirection,
						targetY + ctx.sector->worldY - randomHeight - 1, ctx);
				}
			}
			else if (avl::utils::get_chance(0.5f))
			{
				targetX = ctx.x - 1;
				if (ctx.is_empty(targetX, targetY))
				{
					ctx.world->plot_cell(targetX + ctx.sector->worldX, targetY + ctx.sector->worldY, 9);

					// Clear plant seed flag on target cell.
					auto targetData = ctx.get_user_data(targetX, targetY);
					targetData.clear_flag(CellFlags::PLANT_SEED);
					ctx.set_user_data(targetX, targetY, targetData);
				}
			}
			else
			{
				targetX = ctx.x + 1;
				if (ctx.is_empty(targetX, targetY))
				{
					ctx.world->plot_cell(targetX + ctx.sector->worldX, targetY + ctx.sector->worldY, 9);

					// Clear plant seed flag on target cell.
					auto targetData = ctx.get_user_data(targetX, targetY);
					targetData.clear_flag(CellFlags::PLANT_SEED);
					ctx.set_user_data(targetX, targetY, targetData);
				}
			}
		}

		ctx.set_user_data(data);
	}
	else if (!data.has_flag(CellFlags::GROWN_PLANT))
	{
		if (!MoveDown(ctx))
		{
			data.clear_flag(CellFlags::PLANT_SEED);
			ctx.set_user_data(data);
			ctx.notify_chunk();
			ctx.notify_sector();
		}
	}

	return false;
}

inline void PlotFlower(int worldX, int worldY, avl::CellUpdateContext& ctx)
{
	if (avl::utils::get_random_value01() > .75f)
	{
		float hue = avl::utils::get_random_value01();
		float saturation = .85f;
		float value = std::max<float>(avl::utils::get_random_value01(), 0.45f);
		uint32_t flowerColor = avl::utils::HSVtoRGB(hue, saturation, value);
		ctx.world->plot_cell(worldX, worldY, 16, flowerColor, false);
	}
}
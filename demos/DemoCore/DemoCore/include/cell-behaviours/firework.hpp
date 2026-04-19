#pragma once
#include <avalanche.hpp>
#include "../common-cell-moves.hpp"
#include "../cell-flags.hpp"

bool OnUpdateFireWork(avl::CellUpdateContext& ctx)
{
	ctx.notify_chunk();
	ctx.notify_sector();

	auto data = ctx.get_user_data();
	data.value++;

	if (data.value >= 100)
	{
		data.value = 0;
		data.clear_all_flags();

		int randomExplosionSize = avl::utils::get_random_value(8, 30);
		const int xPosition = ctx.x + ctx.sector->worldX;
		const int yPosition = ctx.y + ctx.sector->worldY;

		for (int x = -randomExplosionSize; x <= randomExplosionSize; x++)
		{
			for (int y = -randomExplosionSize; y <= randomExplosionSize; y++)
			{
				float dist = sqrt((float)(x * x + y * y));
				float edgeFactor = 1.0f - abs(dist - randomExplosionSize) / randomExplosionSize;

				if (edgeFactor < 0.0f) edgeFactor = 0.0f;
				if (edgeFactor > 1.0f) edgeFactor = 1.0f;

				float radiusVariation = avl::utils::get_random_value(-5, 5) * edgeFactor;
				float adjustedRadius = randomExplosionSize + radiusVariation;

				if (dist <= adjustedRadius)
				{
					int newXPosition = xPosition + x;
					int newYPosition = yPosition + y;

					if (ctx.get_cell_id(x, y) != 10)
					{
						ctx.world->plot_cell(newXPosition, newYPosition, 10);
						auto newData = ctx.get_user_data(newXPosition - ctx.sector->worldX, newYPosition - ctx.sector->worldY);
						newData.set_flag(RANDOM_COLOR);
						ctx.set_user_data(newXPosition - ctx.sector->worldX, newYPosition - ctx.sector->worldY, newData);
					}
				}
			}
		}
	}

	ctx.set_user_data(data);

	if (FloatUp(ctx, 4))
	{
		int dx = avl::utils::get_random_value(-1, 1);
		int newX = ctx.x + dx;
		int newY = ctx.y + 1;

		if (!ctx.is_out_of_bounds(newX, newY) && ctx.is_empty(newX, newY))
		{
			ctx.world->plot_cell(newX + ctx.sector->worldX, newY + ctx.sector->worldY, 4);
			auto newData = ctx.get_user_data(newX, newY);
			newData.set_flag(IMMOVABLE);
			ctx.set_user_data(newX, newY, newData);
		}
		else
		{
			ctx.world->plot_cell(ctx.x + ctx.sector->worldX, ctx.y + ctx.sector->worldY + 1, 4);
			auto newData = ctx.get_user_data(ctx.x, ctx.y + 1);
			newData.set_flag(IMMOVABLE);
			ctx.set_user_data(ctx.x, ctx.y + 1, newData);
		}
	}

	return true;
}
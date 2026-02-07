#pragma once
#include <avalanche.hpp>
#include "cell-flags.hpp"

void UpdateFireSpread(avl::CellUpdateContext& ctx, avl::CellUserData data)
{
	if (data.has_flag(CellFlags::FLAMMABLE)) // Fire spread behavior implementation.
	{
		// Convert local positions to world positions.
		int worldX = ctx.x + ctx.sector->worldX;
		int worldY = ctx.y + ctx.sector->worldY;

		avl::Vector2Int upWorld = { worldX, worldY - 1 };
		avl::Vector2Int downWorld = { worldX, worldY + 1 };
		avl::Vector2Int leftWorld = { worldX - 1, worldY };
		avl::Vector2Int rightWorld = { worldX + 1, worldY };

		// Helper to check HOT flag using world coordinates.
		auto hasHotFlag = [&ctx](int wx, int wy) -> bool 
		{
			avl::CellUserData neighborData = ctx.world->get_cell_user_data(wx, wy);
			return neighborData.has_flag(CellFlags::HOT);
		};

		// Check if any neighbor has the HOT flag using world coordinates.
		bool up = hasHotFlag(upWorld.x, upWorld.y);
		bool down = hasHotFlag(downWorld.x, downWorld.y);
		bool left = hasHotFlag(leftWorld.x, leftWorld.y);
		bool right = hasHotFlag(rightWorld.x, rightWorld.y);

		if (up || down || left || right)
		{
			// Increment lifetime.
			data.value += (uint16_t)avl::utils::get_random_value(0, 2);

			// Notify neighboring chunks using world coordinates.
			auto notifyChunkAtWorld = [&ctx](int wx, int wy) 
			{
				avl::SectorSimulationChunk* chunk 
					= ctx.world->try_get_chunk(wx, wy);
				
				if (chunk)
				{
					ctx.notify_chunk(chunk);

					avl::SimulationSector* sector 
						= ctx.world->try_get_sector(wx, wy);
					
					if (sector)
						sector->notify_sector();
				}
			};

			notifyChunkAtWorld(upWorld.x, upWorld.y);
			notifyChunkAtWorld(downWorld.x, downWorld.y);
			notifyChunkAtWorld(leftWorld.x, leftWorld.y);
			notifyChunkAtWorld(rightWorld.x, rightWorld.y);

			ctx.notify_chunk();
			ctx.notify_sector();

			if (data.value == 20)
			{
				data.value = 0;
				data.clear_flag(CellFlags::FLAMMABLE);

				// Check if up is empty using world coordinates.
				bool upIsEmpty = (ctx.world->get_cell_id_safe(upWorld.x, upWorld.y) == 0);

				if (upIsEmpty) // Spawn "ember" above if possible.
				{
					data.set_flag(CellFlags::HOT);
					ctx.set_user_data(data);
					ctx.world->plot_cell(worldX, worldY, 4);
				}
				else // Spawn stationary fire.
				{
					data.set_flag(CellFlags::HOT | CellFlags::LONG_LIFE_TIME);
					ctx.set_user_data(data);
					ctx.world->plot_cell(worldX, worldY, 4);
				}
			}
			else if (data.value > 45) // Reset if taking too long.
			{
				data.value = 0;
				ctx.set_user_data(data);
			}
			else // Save updated lifetime.
			{
				ctx.set_user_data(data);
			}
		}
	}
}

void UpdateSparkSpread(avl::CellUpdateContext& ctx, avl::CellUserData data)
{
	if (data.has_flag(CellFlags::CONDUCTIVE)) // Wire spark spread behavior implementation.
	{
		// Convert local position to world position.
		int worldX = ctx.x + ctx.sector->worldX;
		int worldY = ctx.y + ctx.sector->worldY;

		avl::Vector2Int neighborsWorld[4] = {
			{ worldX, worldY - 1 },
			{ worldX, worldY + 1 },
			{ worldX - 1, worldY },
			{ worldX + 1, worldY }
		};

		// Check if any neighbor is a spark using world coordinates.
		bool hasSparkNeighbor = false;
		for (const auto& neighbor : neighborsWorld)
		{
			uint8_t neighborID = ctx.world->get_cell_id_safe(neighbor.x, neighbor.y);
			if (neighborID == 13) // Spark ID.
			{
				hasSparkNeighbor = true;
				break;
			}
		}

		if (hasSparkNeighbor)
		{
			if (avl::utils::get_chance(0.65f))
			{
				// Convert wire to spark.
				ctx.world->plot_cell(worldX, worldY, 13);
				ctx.notify_chunk();
				ctx.notify_sector();

				// Notify neighboring chunks using world coordinates.
				auto notifyChunkAtWorld = [&ctx](int wx, int wy) 
				{
					avl::SectorSimulationChunk* chunk = ctx.world->try_get_chunk(wx, wy);
					if (chunk)
					{
						chunk->isSleeping = false;
						chunk->hasBeenUpdatedThisFrame = true;

						// Also notify the sector.
						avl::SimulationSector* sector 
							= ctx.world->try_get_sector(wx, wy);
						
						if (sector)
							sector->notify_sector();
					}
				};

				for (const auto& neighbor : neighborsWorld)
					notifyChunkAtWorld(neighbor.x, neighbor.y);
			}
			else // Didn't ignite this frame, keep checking.
			{
				ctx.notify_chunk();
				ctx.notify_sector();
			}
		}
	}
}

// A generic cell post-processor that gets called after each cell update.
void CellPostProcessor(avl::CellUpdateContext& ctx)
{
	auto data = ctx.get_user_data();
	UpdateFireSpread(ctx, data);
	UpdateSparkSpread(ctx, data);
}
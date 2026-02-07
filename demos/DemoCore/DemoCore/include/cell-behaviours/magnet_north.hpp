#pragma once
#include <avalanche.hpp>
#include "../common-cell-moves.hpp"
#include "../cell-flags.hpp"

inline bool OnUpdateMagnetNorth(avl::CellUpdateContext& ctx)
{
    ctx.notify_chunk();
    ctx.notify_sector();

    bool isAttractor = true;
    constexpr int range = 32;
    constexpr int rangeSquared = range * range;

    constexpr int step = 2; // Check every 2nd cell.

    for (int dy = -range; dy <= range; dy += step)
    {
        for (int dx = -range; dx <= range; dx += step)
        {
            if (dx == 0 && dy == 0) continue;

            // Early distance check using squared distance.
            int distSquared = dx * dx + dy * dy;
            if (distSquared > rangeSquared || distSquared < 4) continue;

            int targetX = ctx.x + dx;
            int targetY = ctx.y + dy;

            if (ctx.is_out_of_bounds(targetX, targetY))
                continue;

            uint8_t targetID = ctx.get_cell_id(targetX, targetY);

            if (targetID != 2 && targetID == 0) continue;

            auto targetData = ctx.get_user_data(targetX, targetY);

            if (!targetData.has_flag(CellFlags::CONDUCTIVE) && targetID != 2)
                continue;

            // Use fast inverse square root approximation.
            float invDist = avl::utils::fast_inv_sqrt((float)distSquared);
            float distance = 1.0f / invDist;

            float strength = 1.0f - (distance / range);

            // Reduce probability check frequency.
            if (avl::utils::get_chance(strength * 0.15f))
            {
                int dirX = (dx > 0) - (dx < 0);
                int dirY = (dy > 0) - (dy < 0);

                if (!isAttractor)
                {
                    dirX = -dirX;
                    dirY = -dirY;
                }

                int newX = targetX - dirX;
                int newY = targetY - dirY;

                if (newX == ctx.x && newY == ctx.y)
                    continue;

                if (!ctx.is_out_of_bounds(newX, newY) && ctx.is_empty(newX, newY))
                {
                    uint32_t color = ctx.sector->_activeCellColors[
                        targetY * ctx.sector->width + targetX];

                    ctx.world->plot_cell(
                        newX + ctx.sector->worldX,
                        newY + ctx.sector->worldY,
                        targetID, color, false);

                    ctx.world->plot_cell(
                        targetX + ctx.sector->worldX,
                        targetY + ctx.sector->worldY,
                        0);
                }
            }
        }
    }

    return false;
}
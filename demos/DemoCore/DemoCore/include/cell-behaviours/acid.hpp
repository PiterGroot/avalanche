#pragma once
#include <avalanche.hpp>
#include "../common-cell-moves.hpp"
#include "../cell-flags.hpp"

inline bool OnUpdateAcid(avl::CellUpdateContext& ctx)
{
    static uint32_t colors[5] =
    {
        avl::utils::pack_RGBA(0.2f, 1.0f, 0.2f, 1.0f),
        avl::utils::pack_RGBA(0.3f, 0.9f, 0.3f, 1.0f),
        avl::utils::pack_RGBA(0.15f, 1.0f, 0.25f, 1.0f),
        avl::utils::pack_RGBA(0.25f, 0.95f, 0.15f, 1.0f),
        avl::utils::pack_RGBA(0.1f, 1.0f, 0.3f, 1.0f)
    };

    ctx.notify_chunk();
    ctx.notify_sector();

    auto data = ctx.get_user_data();
    data.value++;

    if (avl::utils::get_chance(0.3f))
        ctx.sector->_activeCellColors[ctx.cellIndex] = colors[avl::utils::get_random_value(0, 4)];

    avl::Vector2Int neighbors[4] =
    {
        { ctx.x, ctx.y - 1 },
        { ctx.x, ctx.y + 1 },
        { ctx.x - 1, ctx.y },
        { ctx.x + 1, ctx.y }
    };

    for (const auto& neighbor : neighbors)
    {
        if (!ctx.is_out_of_bounds(neighbor.x, neighbor.y))
        {
            uint8_t neighborID = ctx.get_cell_id(neighbor.x, neighbor.y);

            if (neighborID != 0 && neighborID != 1 && neighborID != 18)
            {
                if (avl::utils::get_chance(0.05f))
                {
                    ctx.world->plot_cell(
                        neighbor.x + ctx.sector->worldX,
                        neighbor.y + ctx.sector->worldY,
                        0);

                    if (avl::utils::get_chance(0.5f))
                    {
                        avl::Vector2Int smokePositions[4] = {
                            { neighbor.x, neighbor.y - 1 },
                            { neighbor.x, neighbor.y + 1 },
                            { neighbor.x - 1, neighbor.y },
                            { neighbor.x + 1, neighbor.y }
                        };

                        for (const auto& smokePos : smokePositions)
                        {
                            if (!ctx.is_out_of_bounds(smokePos.x, smokePos.y) &&
                                ctx.is_empty(smokePos.x, smokePos.y))
                            {
                                ctx.world->plot_cell(
                                    smokePos.x + ctx.sector->worldX,
                                    smokePos.y + ctx.sector->worldY,
                                    6);
                                break;
                            }
                        }
                    }

                    ctx.notify_chunk();
                    ctx.notify_sector();
                }
            }
        }
    }

    ctx.set_user_data(data);

    if (!MoveDown(ctx))
    {
        return MoveSide(ctx);
    }

    return false;
}
#pragma once
#include <avalanche.hpp>
#include <unordered_set>
#include "../common-cell-moves.hpp"
#include "../cell-flags.hpp"

inline void ExplodeNearbyGunpowder(avl::CellUpdateContext& ctx)
{
    std::vector<avl::Vector2Int> gunpowderPositions;
    std::vector<avl::Vector2Int> toCheck;
    std::unordered_set<int> visited;

    auto encodePos = [](int x, int y) -> int { return (x << 16) | (y & 0xFFFF); };

    toCheck.reserve(128);
    gunpowderPositions.reserve(128);

    toCheck.push_back({ ctx.x, ctx.y });
    visited.insert(encodePos(ctx.x, ctx.y));
    gunpowderPositions.push_back({ ctx.x, ctx.y });

    while (!toCheck.empty() && gunpowderPositions.size())
    {
        avl::Vector2Int current = toCheck.back();
        toCheck.pop_back();

        avl::Vector2Int neighbors[4] =
        {
            {current.x, current.y - 1},
            {current.x, current.y + 1},
            {current.x - 1, current.y},
            {current.x + 1, current.y}
        };

        for (const auto& neighbor : neighbors)
        {
            if (ctx.is_out_of_bounds(neighbor.x, neighbor.y))
                continue;

            int encoded = encodePos(neighbor.x, neighbor.y);

            if (visited.find(encoded) != visited.end())
                continue;

            if (ctx.get_cell_id(neighbor.x, neighbor.y) == ctx.cellID)
            {
                visited.insert(encoded);
                toCheck.push_back(neighbor);
                gunpowderPositions.push_back(neighbor);
            }
        }
    }

    int explosionCount = std::min<int>(5, (int)gunpowderPositions.size() / 10);

    for (int i = 0; i < explosionCount; ++i)
    {
        int randomIndex = avl::utils::get_random_value(0, (int)gunpowderPositions.size() - 1);
        int explosionRadius = avl::utils::get_random_value(25, 45);

        const auto& pos = gunpowderPositions[randomIndex];
        const int xPosition = pos.x + ctx.sector->worldX;
        const int yPosition = pos.y + ctx.sector->worldY;

        for (int x = -explosionRadius; x <= explosionRadius; x++)
        {
            for (int y = -explosionRadius; y <= explosionRadius; y++)
            {
                float dist = sqrt((float)(x * x + y * y));

                // Add randomness to the radius check for jagged edges.
                float edgeFactor = 1.0f - abs(dist - explosionRadius) / explosionRadius;

                if (edgeFactor < 0.0f) edgeFactor = 0.0f;
                if (edgeFactor > 1.0f) edgeFactor = 1.0f;

                float radiusVariation = avl::utils::get_random_value(-10.0f, 10.0f) * edgeFactor;
                float adjustedRadius = explosionRadius + radiusVariation;

                bool inRadius = dist <= adjustedRadius;

                if (inRadius)
                {
                    int newXPosition = xPosition + x;
                    int newYPosition = yPosition + y;

                    bool chance = avl::utils::get_chance(0.5f);

                    if (!chance)
                    {
                        ctx.world->plot_cell(newXPosition, newYPosition, 0);
                    }
                    else
                    {
                        ctx.world->plot_cell(newXPosition, newYPosition, 4, avl::utils::pack_RGBA(1.0f, 1.0f, 1.0f, 1.0f));

                        auto targetData = ctx.world->get_cell_user_data(newXPosition, newYPosition);
                        targetData.set_flag(CellFlags::IMMOVABLE);
                        ctx.world->set_cell_user_data(newXPosition, newYPosition, targetData);
                    }
                }
            }
        }
    }
}

// TODO: Move neighbour cell query logic this to a helper function to easily reuse it.
inline bool OnUpdateGunPowder(avl::CellUpdateContext& ctx)
{
    avl::Vector2Int upIndex = { ctx.x, ctx.y - 1 };
    avl::Vector2Int downIndex = { ctx.x, ctx.y + 1 };
    avl::Vector2Int leftIndex = { ctx.x - 1, ctx.y };
    avl::Vector2Int rightIndex = { ctx.x + 1, ctx.y };

    // Check if any neighbor has the HOT flag.
    bool up = !ctx.is_out_of_bounds(upIndex.x, upIndex.y) &&
        ctx.get_user_data(upIndex.x, upIndex.y).has_flag(CellFlags::HOT);
    bool down = !ctx.is_out_of_bounds(downIndex.x, downIndex.y) &&
        ctx.get_user_data(downIndex.x, downIndex.y).has_flag(CellFlags::HOT);
    bool left = !ctx.is_out_of_bounds(leftIndex.x, leftIndex.y) &&
        ctx.get_user_data(leftIndex.x, leftIndex.y).has_flag(CellFlags::HOT);
    bool right = !ctx.is_out_of_bounds(rightIndex.x, rightIndex.y) &&
        ctx.get_user_data(rightIndex.x, rightIndex.y).has_flag(CellFlags::HOT);

    if (up || down || left || right)
    {
        avl::SectorSimulationChunk** chunkBuffer = nullptr;

        if (ctx.get_chunk_safe(upIndex.x, upIndex.y, chunkBuffer))
            ctx.notify_chunk(chunkBuffer[0]);

        if (ctx.get_chunk_safe(downIndex.x, downIndex.y, chunkBuffer))
            ctx.notify_chunk(chunkBuffer[0]);

        if (ctx.get_chunk_safe(leftIndex.x, leftIndex.y, chunkBuffer))
            ctx.notify_chunk(chunkBuffer[0]);

        if (ctx.get_chunk_safe(rightIndex.x, rightIndex.y, chunkBuffer))
            ctx.notify_chunk(chunkBuffer[0]);

        ctx.notify_chunk();
        ctx.notify_sector();

        ExplodeNearbyGunpowder(ctx);

        // Reset user data value.
        auto data = ctx.get_user_data();
        data.value = 0;
        ctx.set_user_data(data);

        return true;
    }

    MoveDown(ctx);

    return false;
}
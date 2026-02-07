#pragma once
#include <avalanche.hpp>
#include "../common-cell-moves.hpp"
#include "../cell-flags.hpp"

inline bool OnUpdateQuantumMatter(avl::CellUpdateContext& ctx)
{
    auto data = ctx.get_user_data();

    ctx.notify_chunk();
    ctx.notify_sector();

    data.value++;

    if (data.value % 3 == 0)
    {
        if (avl::utils::get_chance(0.5f))
        {
            ctx.sector->_activeCellColors[ctx.cellIndex] =
                avl::utils::pack_RGBA(0.0f, 0.0f, 0.0f, 0.0f);

            data.set_flag(CellFlags::CHARACTER_TRAVERSABLE);
        }
        else
        {
            float hue = avl::utils::get_random_value01();
            ctx.sector->_activeCellColors[ctx.cellIndex] =
                avl::utils::HSVtoRGB(hue, 1.0f, 1.0f);

            data.clear_flag(CellFlags::CHARACTER_TRAVERSABLE);
        }
    }

    if (data.value % 25 == 0 && avl::utils::get_chance(0.4f))
    {
        int jumpDist = avl::utils::get_random_value(3, 8);
        int angle = avl::utils::get_random_value(0, 7);

        int dx = (angle == 0 || angle == 1 || angle == 7) ? jumpDist :
            (angle == 3 || angle == 4 || angle == 5) ? -jumpDist :
            avl::utils::get_random_value(-jumpDist, jumpDist);
        
        int dy = (angle == 1 || angle == 2 || angle == 3) ? -jumpDist :
            (angle == 5 || angle == 6 || angle == 7) ? jumpDist :
            avl::utils::get_random_value(-jumpDist, jumpDist);

        int targetX = ctx.x + dx;
        int targetY = ctx.y + dy;

        if (!ctx.is_out_of_bounds(targetX, targetY) && ctx.is_empty(targetX, targetY))
        {
            uint32_t color = ctx.sector->_activeCellColors[ctx.cellIndex];

            ctx.world->plot_cell(
                targetX + ctx.sector->worldX,
                targetY + ctx.sector->worldY,
                ctx.cellID, color, false);

            ctx.set_user_data(targetX, targetY, data);

            ctx.world->plot_cell(
                ctx.x + ctx.sector->worldX,
                ctx.y + ctx.sector->worldY,
                0);

            if (avl::utils::get_chance(0.5f))
            {
                ctx.world->plot_cell(
                    ctx.x + ctx.sector->worldX,
                    ctx.y + ctx.sector->worldY,
                    ctx.cellID);

                auto echoData = avl::CellUserData(0, 90);
                ctx.set_user_data(ctx.x, ctx.y, echoData);
            }

            return false;
        }
    }

    if (data.value % 40 == 0 && avl::utils::get_chance(0.3f))
    {
        for (int attempt = 0; attempt < 5; ++attempt)
        {
            int dx = avl::utils::get_random_value(-5, 5);
            int dy = avl::utils::get_random_value(-5, 5);
            int targetX = ctx.x + dx;
            int targetY = ctx.y + dy;

            if (!ctx.is_out_of_bounds(targetX, targetY) && ctx.is_empty(targetX, targetY))
            {
                ctx.world->plot_cell(
                    targetX + ctx.sector->worldX,
                    targetY + ctx.sector->worldY,
                    ctx.cellID);

                int steps = std::max(abs(dx), abs(dy));
                for (int i = 0; i <= steps; ++i)
                {
                    int lineX = ctx.x + (dx * i) / steps;
                    int lineY = ctx.y + (dy * i) / steps;

                    if (!ctx.is_out_of_bounds(lineX, lineY) && ctx.is_empty(lineX, lineY))
                    {
                        if (avl::utils::get_chance(0.3f))
                        {
                            ctx.world->plot_cell(
                                lineX + ctx.sector->worldX,
                                lineY + ctx.sector->worldY,
                                13);
                        }
                    }
                }

                break;
            }
        }
    }

    if (avl::utils::get_chance(0.3f))
    {
        int moveType = avl::utils::get_random_value(0, 2);
        if (moveType == 0) MoveDown(ctx);
        else if (moveType == 1) FloatUp(ctx);
        else MoveSide(ctx);
    }

    if (data.value > 300)
    {
        if (avl::utils::get_chance(0.05f))
        {
            uint8_t collapseInto[] = { 0, 2, 3, 6, 13, 14, 15 };
            uint8_t result = collapseInto[avl::utils::get_random_value(0, 4)];

            ctx.world->plot_cell(
                ctx.x + ctx.sector->worldX,
                ctx.y + ctx.sector->worldY,
                result);

            return false;
        }
    }

    ctx.set_user_data(data);
    return true;
}
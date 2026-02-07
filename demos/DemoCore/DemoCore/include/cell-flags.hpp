#pragma once
#include <cstdint>

enum CellFlags : uint16_t
{
	HOT = 1 << 0,
	FLAMMABLE = 1 << 1,
	LONG_LIFE_TIME = 1 << 2,
	CHARACTER_TRAVERSABLE = 1 << 3,
	PLANT_SEED = 1 << 4,
	GROWN_PLANT = 1 << 5,
	SHORT_LIFE_TIME = 1 << 6,
	IMMOVABLE = 1 << 7,
	CONDUCTIVE = 1 << 8,
	CHARGED = 1 << 9,
	RANDOM_COLOR = 1 << 10,
};
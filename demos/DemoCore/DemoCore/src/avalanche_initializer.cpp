#define AVALANCHE_USE_CELL_POST_POSTPROCESSOR
#define AVALANCHE_IMPLEMENTATION
#define AVALANCHE_DEBUG_DRAWER

#include "../include/avalanche_initializer.hpp"

#include "../include/cell-behaviours/sand.hpp"
#include "../include/cell-behaviours/water.hpp"
#include "../include/cell-behaviours/fire.hpp"
#include "../include/cell-behaviours/smoke.hpp"
#include "../include/cell-behaviours/lava.hpp"
#include "../include/cell-behaviours/gun_powder.hpp"
#include "../include/cell-behaviours/plant.hpp"
#include "../include/cell-behaviours/spark.hpp"
#include "../include/cell-behaviours/unstable_matter.hpp"
#include "../include/cell-behaviours/quantum_matter.hpp"
#include "../include/cell-behaviours/acid.hpp"
#include "../include/cell-behaviours/firework.hpp"

#include "../include/cell-post-processor.hpp"

AvalancheInitializer::AvalancheInitializer()
{
	world = std::make_unique<avl::World>();

	// Rock.
	{
		uint32_t colorA = avl::utils::pack_RGBA(0.75f, 0.75f, 0.75f, 1.0f);
		uint32_t colorB = avl::utils::pack_RGBA(0.45f, 0.45f, 0.45f, 1.0f);
		avl::utils::register_cell(1, colorA, colorB, *world);
	}

	// Sand.
	{
		uint32_t colorA = avl::utils::pack_RGBA(0.99f, 0.97f, 0.0f, 1.0f);
		uint32_t colorB = avl::utils::pack_RGBA(0.6f, 0.46f, 0.05f, 1.0f);
		avl::utils::register_cell(2, colorA, colorB, *world, OnUpdateSand);
	}

	// Water.
	{
		uint32_t colorA = avl::utils::pack_RGBA(0.05f, 0.58f, 1.0f, 0.58f);
		uint32_t colorB = avl::utils::pack_RGBA(0.05f, 0.58f, 1.0f, 0.58f);
		avl::utils::register_cell(3, colorA, colorB, *world, OnUpdateWater);
	}

	// Fire.
	{
		avl::CellUserData fireData(HOT | SHORT_LIFE_TIME, 0);
		uint32_t colorA = avl::utils::pack_RGBA(0.99f, 0.97f, 0.0f, 1.0f);
		uint32_t colorB = avl::utils::pack_RGBA(0.99f, 0.97f, 0.0f, 1.0f);
		avl::utils::register_cell(4, colorA, colorB, *world, OnUpdateFire, fireData.pack());
	}

	// Wood.
	{
		avl::CellUserData woodData(FLAMMABLE | CHARACTER_TRAVERSABLE, 0);
		uint32_t colorA = avl::utils::pack_RGBA(0.392f, 0.254f, 0.09f, 1.0f);
		uint32_t colorB = avl::utils::pack_RGBA(0.11f, 0.06f, 0.00f, 1.0f);
		avl::utils::register_cell(5, colorA, colorB, *world, nullptr, woodData.pack());
	}

	// Smoke.
	{
		uint32_t colorA = avl::utils::pack_RGBA(.2f, .2f, .2f, 1.0f);
		avl::utils::register_cell(6, colorA, colorA, *world, OnUpdateSmoke);
	}

	// Lava.
	{
		avl::CellUserData lavaData(HOT, 0);
		uint32_t colorA = avl::utils::pack_RGBA(1, 0.369f, 0.0f, 1.0f);
		avl::utils::register_cell(7, colorA, colorA, *world, OnUpdateLava, lavaData.pack());
	}

	// Gunpowder.
	{
		uint32_t colorA = avl::utils::pack_RGBA(0.8f, 0.4f, 0.36f, 1.0f);
		uint32_t colorB = avl::utils::pack_RGBA(0.50f, 0.25f, 0.25f, 1.0f);
		avl::utils::register_cell(8, colorA, colorB, *world, OnUpdateGunPowder);
	}

	// Plant seed.
	{
		avl::CellUserData plantData(PLANT_SEED | FLAMMABLE, 0);
		uint32_t colorA = avl::utils::pack_RGBA(0.0f, 0.56f, 0.18f, 1.0f);
		avl::utils::register_cell(9, colorA, colorA, *world, OnUpdatePlant, plantData.pack());
	}

	// Spark.
	{
		avl::CellUserData sparkData(HOT, 0);
		uint32_t colorA = avl::utils::pack_RGBA(0.6f, 0.7f, 1.0f, 1.0f);
		uint32_t colorB = avl::utils::pack_RGBA(1.0f, 1.0f, 1.0f, 1.0f);
		avl::utils::register_cell(10, colorA, colorB, *world, OnUpdateSpark, sparkData.pack());
	}

	// Steel.
	{
		avl::CellUserData steelData(CONDUCTIVE, 0);
		uint32_t collorA = avl::utils::pack_RGBA(0.28f, 0.31f, 0.36f, 1.0f);
		avl::utils::register_cell(11, collorA, collorA, *world, nullptr, steelData.pack());
	}

	// Unstable matter.
	{
		uint32_t colorA = avl::utils::pack_RGBA(1.0f, 0.8f, 0.9f, 1.0f);
		avl::utils::register_cell(12, colorA, colorA, *world, OnUpdateUnstableMatter);
	}

	// Quantum matter.
	{
		uint32_t colorA = avl::utils::pack_RGBA(0.23f, 0.89f, 0.18f, 1.0f);
		avl::utils::register_cell(13, colorA, colorA, *world, OnUpdateQuantumMatter);
	}

	// Acid.
	{
		uint32_t colorA = avl::utils::pack_RGBA(0.0f, 0.59f, 0.18f, 1.0f);
		avl::utils::register_cell(14, colorA, colorA, *world, OnUpdateAcid);
	}

	// Firework.
	{
		uint32_t colorA = avl::utils::pack_RGBA(0.58f, 0.31f, 0.58f, 1.0f);
		avl::utils::register_cell(15, colorA, colorA, *world, OnUpdateFireWork);
	}

	// Plant leaves
	{
		avl::CellUserData leavesData(FLAMMABLE, 0);
		uint32_t colorA = avl::utils::pack_RGBA(0.3f, 0.47f, 0.25f, 1.0f);
		uint32_t colorB = avl::utils::pack_RGBA(0.29f, 0.73f, 0.09f, 1.0f);
		avl::utils::register_cell(16, colorA, colorB, *world, nullptr, leavesData.pack());
	}

	world->set_cell_post_processor(CellPostProcessor);
}
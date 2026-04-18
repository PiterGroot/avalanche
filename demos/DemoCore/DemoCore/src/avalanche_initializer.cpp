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
		uint32_t collorA = avl::utils::pack_RGBA(1.0f, 1.0f, 1.0f, 1.0f);
		uint32_t collorB = avl::utils::pack_RGBA(0.29f, 0.29f, 0.29f, 1.0f);
		avl::utils::register_cell(1, collorA, collorB, *world);
	}

	// Sand.
	{
		uint32_t collorA = avl::utils::pack_RGBA(0.99f, 0.97f, 0.0f, 1.0f);
		uint32_t collorB = avl::utils::pack_RGBA(0.6f, 0.46f, 0.05f, 1.0f);
		avl::utils::register_cell(2, collorA, collorB, *world, OnUpdateSand);
	}

	// Water.
	{
		uint32_t collorA = avl::utils::pack_RGBA(0.05f, 0.58f, 1.0f, 0.58f);
		uint32_t collorB = avl::utils::pack_RGBA(0.05f, 0.58f, 1.0f, 0.58f);
		avl::utils::register_cell(3, collorA, collorB, *world, OnUpdateWater);
	}

	// Fire.
	{
		avl::CellUserData fireData(HOT | SHORT_LIFE_TIME, 0);
		uint32_t collorA = avl::utils::pack_RGBA(0.99f, 0.97f, 0.0f, 1.0f);
		uint32_t collorB = avl::utils::pack_RGBA(0.99f, 0.97f, 0.0f, 1.0f);

		avl::utils::register_cell(4, collorA, collorB, *world, OnUpdateFire, fireData.pack());
	}

	// Wood.
	{
		avl::CellUserData woodData(FLAMMABLE | CHARACTER_TRAVERSABLE, 0);
		uint32_t collorA = avl::utils::pack_RGBA(0.392f, 0.254f, 0.09f, 1.0f);
		uint32_t collorB = avl::utils::pack_RGBA(0.11f, 0.06f, 0.00f, 1.0f);
		avl::utils::register_cell(5, collorA, collorB, *world, nullptr, woodData.pack());
	}

	// Smoke.
	{
		uint32_t collorA = avl::utils::pack_RGBA(.2f, .2f, .2f, 1.0f);
		avl::utils::register_cell(6, collorA, collorA, *world, OnUpdateSmoke);
	}

	// Lava.
	{
		avl::CellUserData lavaData(HOT, 0);
		uint32_t collorA = avl::utils::pack_RGBA(1, 0.369f, 0.0f, 1.0f);
		avl::utils::register_cell(7, collorA, collorA, *world, OnUpdateLava, lavaData.pack());
	}

	// Gunpowder.
	{
		uint32_t collorA = avl::utils::pack_RGBA(0.8f, 0.4f, 0.36f, 1.0f);
		avl::utils::register_cell(8, collorA, collorA, *world, OnUpdateGunPowder);
	}

	// Plant seed.
	{
		avl::CellUserData plantData(PLANT_SEED | FLAMMABLE, 0);
		uint32_t collorA = avl::utils::pack_RGBA(0.0f, 0.56f, 0.18f, 1.0f);
		avl::utils::register_cell(9, collorA, collorA, *world, OnUpdatePlant, plantData.pack());
	}

	// Spark.
	{
		avl::CellUserData sparkData(HOT, 0);
		uint32_t collorA = avl::utils::pack_RGBA(0.0f, 0.56f, 0.18f, 1.0f);
		avl::utils::register_cell(10, collorA, collorA, *world, OnUpdateSpark, sparkData.pack());
	}

	// Steel.
	{
		avl::CellUserData steelData(CONDUCTIVE, 0);
		uint32_t collorA = avl::utils::pack_RGBA(0.28f, 0.31f, 0.36f, 1.0f);
		avl::utils::register_cell(11, collorA, collorA, *world, nullptr, steelData.pack());
	}

	// Unstable matter.
	{
		uint32_t collorA = avl::utils::pack_RGBA(1.0f, 0.8f, 0.9f, 1.0f);
		avl::utils::register_cell(12, collorA, collorA, *world, OnUpdateUnstableMatter);
	}

	// Quantum matter.
	{
		uint32_t collorA = avl::utils::pack_RGBA(0.23f, 0.89f, 0.18f, 1.0f);
		avl::utils::register_cell(13, collorA, collorA, *world, OnUpdateQuantumMatter);
	}

	// Acid.
	{
		uint32_t collorA = avl::utils::pack_RGBA(0.0f, 0.59f, 0.18f, 1.0f);
		avl::utils::register_cell(14, collorA, collorA, *world, OnUpdateAcid);
	}

	// Firework.
	{
		uint32_t collorA = avl::utils::pack_RGBA(0.58f, 0.31f, 0.58f, 1.0f);
		avl::utils::register_cell(15, collorA, collorA, *world, OnUpdateFireWork);
	}

	world->set_cell_post_processor(CellPostProcessor);
}
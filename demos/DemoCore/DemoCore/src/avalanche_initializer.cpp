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
#include "../include/cell-behaviours/magnet_north.hpp"
#include "../include/cell-behaviours/magnet_south.hpp"
#include "../include/cell-behaviours/unstable_matter.hpp"
#include "../include/cell-behaviours/quantum_matter.hpp"
#include "../include/cell-behaviours/acid.hpp"
#include "../include/cell-behaviours/firework.hpp"

#include "../include/cell-post-processor.hpp"

AvalancheInitializer::AvalancheInitializer()
{
	world = std::make_unique<avl::World>();

	// Rock.
	avl::utils::register_cell(1, avl::utils::pack_RGBA(1.0f, 1.0f, 1.0f, 1.0f), *world);

	// Sand.
	avl::utils::register_cell(2, avl::utils::pack_RGBA(1.0f, 1.0f, 0.0f, 1.0f), *world, OnUpdateSand);

	// Water.
	avl::utils::register_cell(3, avl::utils::pack_RGBA(0.0f, 0.0f, 1.0f, 1.0f), *world, OnUpdateWater);

	// Fire.
	avl::CellUserData fireData(HOT | SHORT_LIFE_TIME, 0);
	avl::utils::register_cell(4, avl::utils::pack_RGBA(1, 0.369f, 0.0f, 1.0f), *world, OnUpdateFire, fireData.pack());

	// Wood.
	avl::CellUserData woodData(FLAMMABLE | CHARACTER_TRAVERSABLE, 0);
	avl::utils::register_cell(5, avl::utils::pack_RGBA(0.392f, 0.254f, 0.09f, 1.0f), *world, nullptr, woodData.pack());

	// Smoke.
	avl::utils::register_cell(6, avl::utils::pack_RGBA(.2f, .2f, .2f, 1.0f), *world, OnUpdateSmoke);

	// Lava.
	avl::CellUserData lavaData(HOT, 0);
	avl::utils::register_cell(7, avl::utils::pack_RGBA(1, 0.369f, 0.0f, 1.0f), *world, OnUpdateLava, lavaData.pack());

	// Stone.
	avl::utils::register_cell(8, avl::utils::pack_RGBA(0.5f, 0.5f, 0.5f, 1.0f), *world, nullptr);

	// Dirt.
	avl::utils::register_cell(9, avl::utils::pack_RGBA(0.49f, 0.41f, 0.3f, 1.0f), *world, nullptr);

	// Leaves.
	avl::CellUserData leavesData(FLAMMABLE, 0);
	avl::utils::register_cell(10, avl::utils::pack_RGBA(0.0f, 0.89f, 0.18f, 1.0f), *world, nullptr, leavesData.pack());

	// Gunpowder.
	avl::utils::register_cell(11, avl::utils::pack_RGBA(0.8f, 0.4f, 0.36f, 1.0f), *world, OnUpdateGunPowder);

	// Plant seed.
	avl::CellUserData plantData(PLANT_SEED | FLAMMABLE, 0);
	avl::utils::register_cell(12, avl::utils::pack_RGBA(0.0f, 0.56f, 0.18f, 1.0f), *world, OnUpdatePlant, plantData.pack());
	
	// Spark.
	avl::CellUserData sparkData(HOT, 0);
	avl::utils::register_cell(13, avl::utils::pack_RGBA(1.0f, 1.0f, 0.8f, 1.0f), *world, OnUpdateSpark, sparkData.pack());
	
	// Steel.
	avl::CellUserData steelData(CONDUCTIVE, 0);
	avl::utils::register_cell(14, avl::utils::pack_RGBA(0.28f, 0.31f, 0.36f, 1.0f), *world, nullptr, steelData.pack());

	// Magnet north.
	avl::utils::register_cell(15, avl::utils::pack_RGBA(0.28f, 0.31f, 0.65f, 1.0f), *world, OnUpdateMagnetNorth);

	// Unstable matter.
	avl::utils::register_cell(16, avl::utils::pack_RGBA(1.0f, 0.8f, 0.9f, 1.0f), *world, OnUpdateUnstableMatter);
	
	// Quantum matter.
	avl::utils::register_cell(17, avl::utils::pack_RGBA(0.23f, 0.89f, 0.18f, 1.0f), *world, OnUpdateQuantumMatter);

	// Acid.
	avl::utils::register_cell(18, avl::utils::pack_RGBA(0.0f, 0.59f, 0.18f, 1.0f), *world, OnUpdateAcid);

	// Magnet south.
	avl::utils::register_cell(19, avl::utils::pack_RGBA(0.58f, 0.31f, 0.25f, 1.0f), *world, OnUpdateMagnetSouth);

	// Firework.
	avl::utils::register_cell(20, avl::utils::pack_RGBA(0.58f, 0.31f, 0.58f, 1.0f), *world, OnUpdateFireWork);

	world->set_cell_post_processor(CellPostProcessor);
}
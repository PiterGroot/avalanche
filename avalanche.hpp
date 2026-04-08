/*
	   /$$$$$$                      /$$                               /$$
	  /$$__  $$                    | $$                              | $$
	 | $$  \ $$ /$$    /$$ /$$$$$$ | $$  /$$$$$$  /$$$$$$$   /$$$$$$$| $$$$$$$   /$$$$$$
	 | $$$$$$$$|  $$  /$$/|____  $$| $$ |____  $$| $$__  $$ /$$_____/| $$__  $$ /$$__  $$
	 | $$__  $$ \  $$/$$/  /$$$$$$$| $$  /$$$$$$$| $$  \ $$| $$      | $$  \ $$| $$$$$$$$
	 | $$  | $$  \  $$$/  /$$__  $$| $$ /$$__  $$| $$  | $$| $$      | $$  | $$| $$_____/
	 | $$  | $$   \  $/  |  $$$$$$$| $$|  $$$$$$$| $$  | $$|  $$$$$$$| $$  | $$|  $$$$$$$
	 |__/  |__/    \_/    \_______/|__/ \_______/|__/  |__/ \_______/|__/  |__/ \_______/

*/

#pragma once

#ifndef AVALANCHE
#define AVALANCHE

//--------------------------------------------
// LIBRARY PRELUDES
//--------------------------------------------

// Library version:
#define AVALANCHE_VERSION_MAJOR 0
#define AVALANCHE_VERSION_MINOR 1
#define AVALANCHE_VERSION_PATCH 0

#ifndef AVALANCHE_MAX_THREADS
#define AVALANCHE_MAX_THREADS -1
#endif

#pragma warning(push)
#pragma warning(disable: 4267)

#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <limits>
#include <deque>

#pragma warning(pop)

namespace avl
{
	//--------------------------------------------
	// FORWARD DECLARATIONS
	//--------------------------------------------
	class World;
	class CellUpdateContext;
	class SimulationSector;

	using CellUpdateCallback = bool(*)(CellUpdateContext& ctx);
	using CellPostProcessor = void(*)(CellUpdateContext& ctx);
	using OnSectorCreated = void(*)(const SimulationSector* sector);
	using OnSectorUpdated = void(*)(const SimulationSector* sector);
	using OnWorldReset = void(*)();

	//--------------------------------------------
	// LIBRARY PUBLIC INTERFACE
	//--------------------------------------------

	struct SectorSimulationChunk
	{
		int chunkX = 0;
		int chunkY = 0;
		bool isSleeping = true;
		bool hasBeenUpdatedThisFrame = false;
	};

	struct Vector2Int
	{
		int x;
		int y;
	};

	struct RegisteredCelllPrefab
	{
		uint8_t type = 0;
		uint32_t colorA = 0;
		uint32_t colorB = 0;
		uint32_t userData = 0;
		CellUpdateCallback updateCallback = {};
	};

	// Global utils namespace functions.
	//-------------------------------------------
	namespace utils
	{
		void register_cell(uint8_t cellID, uint32_t cellColorA, uint32_t cellColorB, World& world, CellUpdateCallback callback = nullptr, uint32_t cellUserData = 0);
		int get_random_value(int min, int max);
		float get_random_value(float min, float max);
		float get_random_value01();
		bool get_chance(float input);
		uint32_t lerp_packed_RGBA(uint32_t a, uint32_t b, uint32_t alpha);
		uint32_t lerp_packed_RGBA(uint32_t a, uint32_t b, float alpha);
		uint32_t pack_RGBA(float r, float g, float b, float a);
		void unpack_RGBA(uint32_t packedColor, float& r, float& g, float& b, float& a);
		uint32_t HSVtoRGB(float h, float s, float v);
	} // namespace utils

	class Logging
	{
	public:
		friend class World;
		friend class SimulationSector;

	private:
		enum class LogType
		{
			LOG_INFO,
			LOG_WARNING,
			LOG_ERROR
		};

	private:
		static void _log_info(const char* msg) { _log(LogType::LOG_INFO, msg); }
		static void _log_warning(const char* msg) { _log(LogType::LOG_WARNING, msg); }
		static void _log_error(const char* msg) { _log(LogType::LOG_ERROR, msg); }
		static std::string _get_version_string();
		static void _log(LogType type, const char* msg);
		static const char* _get_log_color(const LogType type);
	};

	class CellUserData
	{
	public:
		CellUserData() : flags(0), value(0) {}
		CellUserData(uint16_t f, uint16_t v) : flags(f), value(v) {}

		uint32_t pack() const;
		static CellUserData unpack(uint32_t packed);
		void set_flag(uint16_t flag);
		bool has_flag(uint16_t flag) const;
		void clear_flag(uint16_t flag);
		void clear_all_flags();

	public:
		uint16_t flags;
		uint16_t value;
	};

	class CellUpdateContext
	{
	public:
		void move_cell(int toX, int toY);
		void swap_cell(int toX, int toY);
		bool is_empty(int checkX, int checkY) const;
		bool is_empty(int checkX, int checkY, uint8_t& outCellID) const;
		bool is_out_of_bounds(int checkX, int checkY) const;
		uint8_t get_cell_id(int checkX, int checkY) const;
		void notify_chunk() const;
		void notify_chunk(SectorSimulationChunk* newChunk) const;
		void notify_sector() const;
		bool get_chunk_safe(int sectorX, int sectorY, SectorSimulationChunk**& outChunk);
		CellUserData get_user_data() const;
		CellUserData get_user_data(int localX, int localY) const;
		uint16_t get_user_flags() const;
		uint16_t get_user_value() const;
		bool has_user_flag(uint16_t flag) const;
		void set_user_data(const CellUserData& data);
		void set_user_data(int localX, int localY, const CellUserData& data);

	public:
		int x;                          // Cell X position in sector.
		int y;                          // Cell Y position in sector.
		int cellIndex;                  // Grid index of cell.
		uint8_t cellID;                 // Cell type ID.
		uint32_t userData;              // User data.
		uint32_t color;                 // Cell color.
		World* world;					// Pointer to the world.
		SimulationSector* sector;       // Pointer to current sector.
		SectorSimulationChunk* chunk;   // Pointer to current chunk.
		int threadId = 0;				// ID of the thread executing the update.
	};

	class SimulationDebugDrawer
	{
	public:
		struct DebugDrawColor
		{
			unsigned char r; // Color red value.
			unsigned char g; // Color green value.
			unsigned char b; // Color blue value.
			unsigned char a; // Color alpha value.
		};
	public:
		virtual void DrawDebugLine(int fromX, int fromY, int toX, int toY, DebugDrawColor color) = 0;

		void DrawBox(int x, int y, int width, int height, DebugDrawColor color)
		{
			DrawDebugLine(x, y, x + width, y, color);                   // Draw the top side of the box.
			DrawDebugLine(x + width, y, x + width, y + height, color);  // Draw the right side of the box.
			DrawDebugLine(x + width, y + height, x, y + height, color); // Draw the bottom side of the box.
			DrawDebugLine(x, y + height, x, y, color);                  // Draw the left side of the box.
		}
	};

	class SimulationSector
	{
	public:
		friend class World;
		friend class CellUpdateContext;

		SimulationSector(int x, int y, int width, int height, int id, World* worldPtr);
		~SimulationSector();

		int calculate_index(int x, int y) const;
		void notify_sector();

	private:
		void set_cell(int x, int y, RegisteredCelllPrefab* cell);
		void set_cell(int x, int y, float cellColorAlphaMin, float cellColorAlphaMax, RegisteredCelllPrefab* cell);
		void set_cell(int index, RegisteredCelllPrefab* cell);
		void set_cell(int index, float cellColorAlphaMin, float cellColorAlphaMax, RegisteredCelllPrefab* cell);
		void _update();
		void _process_chunks(std::vector<SectorSimulationChunk*>& chunks);
		void _process_chunks_sigle_threaded(std::vector<SectorSimulationChunk*>& chunks);
#ifdef AVALANCHE_DEBUG_DRAWER
		void _debug_draw();
#endif // AVALANCHE_DEBUG_DRAWER
		void _reset_chunks();
		void _commit_cells();
		void _move_cell(int x, int y, int fromIndex, int toX, int toY, SectorSimulationChunk* currentChunk, int threadId);
		void _swap_cell(int fromX, int fromY, int toX, int toY, SectorSimulationChunk* currentChunk, int threadId);
		void _create_chunk(int chunkX, int chunkY, int size);
		void _notify_chunk(SectorSimulationChunk* chunk);
		int _get_chunk_lookup_key(int chunkX, int chunkY);
		bool _get_chunk_safe(int x, int y, SectorSimulationChunk**& outChunk);
		SectorSimulationChunk* _get_chunk_unsafe(int x, int y);
		void _set_cell_info(size_t index, uint8_t cellID, uint32_t cellColor, uint32_t userData);
		bool _is_out_of_bounds(int x, int y) const;
		bool _is_out_of_bounds(int index) const;
		uint8_t _get_cell_id(int index);
		uint32_t _get_cell_color(int index);
		CellUserData _get_cell_user_data(int x, int y) const;
		void _set_cell_user_data(int x, int y, const CellUserData& data);
		bool _is_empty(int index);
		bool _is_empty(int index, uint8_t& outCellID);
		bool _is_empty(int x, int y);
		bool _is_empty(int x, int y, uint8_t& outCellID);

	public:
		bool isSleeping = true;
		bool hasBeenUpdatedThisFrame = false;
		int width = 0;
		int height = 0;
		int worldX = 0;
		int worldY = 0;
		int simulationSize = -1;
		int id = -1;

		uint8_t* _activeCellIDs = nullptr;
		uint32_t* _activeCellColors = nullptr;
		uint32_t* _activeCellsUserData = nullptr;

	private:
		World* _world = nullptr;

		std::vector<SectorSimulationChunk*> _allChunks;
		std::vector<SectorSimulationChunk*> _activeChunks;
		std::unordered_map<int, SectorSimulationChunk*> _chunkLookup;

		std::vector<std::vector<std::pair<int, int>>> _threadCellChanges;
		std::vector<std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>>> _threadCellSwaps;
		std::vector<std::pair<int, int>> _cellChanges;
		std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> _cellSwaps;

		int _numThreads = 1;
	};

	class World
	{
	public:
		World();
		void step_world(const float deltaTime, const float fixedTimeStep);
		void step_world_without_callback(const float deltaTime, const float fixedTimeStep);
		void run_world_one_tick();
		void reset_world();
		bool is_sleeping() const;
		bool is_postion_empty(int worldX, int worldY);
		bool is_position_out_of_bounds(int worldX, int worldY);
		RegisteredCelllPrefab& get_registered_cell_prefab(int index);
		void set_on_sector_created_listener(OnSectorCreated callback);
		void set_on_sector_updated_listener(OnSectorUpdated callback);
		void set_on_world_reset_listener(OnWorldReset callback);
#ifdef AVALANCHE_DEBUG_DRAWER
		void debug_draw();
#endif // AVALANCHE_DEBUG_DRAWER
		SectorSimulationChunk* try_get_chunk(int worldX, int worldY);
		uint8_t get_cell_id_safe(const int worldX, const int worldY);
		uint8_t get_cell_id_unsafe(const int worldX, const int worldY);
		uint32_t get_cell_color(const int worldX, const int worldY);
		CellUserData get_cell_user_data(int worldX, int worldY);
		void set_cell_user_data(int worldX, int worldY, const CellUserData& data);
		void plot_line(int x0, int y0, int x1, int y1, int registeredCellID, float placeChance = 1.0f);
		void plot_line_carefull(int x0, int y0, int x1, int y1, int registeredCellID, float placeChance = 1.0f);
		void plot_rectangle(int x0, int y0, int x1, int y1, int registeredCellID, float placeChance = 1.0f);
		void plot_circle(int xPosition, int yPosition, int size, int registeredCellID, float placeChance = 1.0f);
		void plot_cell(int x, int y, int registeredCellID, bool notify = true);
		void plot_cell(int x, int y, int registeredCellID, float alphaMin, float alphaMax, bool notify = true);
		void plot_cell(int x, int y, int registeredCellID, uint32_t overrideColor, bool notify = true);
		void plot_cell_if_empty(int x, int y, int registeredCellID, bool notify = true);
		SimulationSector* create_sector(int worldX, int worldY);
		SimulationSector* get_or_create_sector(int worldX, int worldY);
		SimulationSector* try_get_sector(int worldX, int worldY);
		SimulationSector* get_sector_direct(int sectorIndex);
		int get_sector_count();
		static void set_debug_drawer(SimulationDebugDrawer* debugDrawer);
		static SimulationDebugDrawer* get_debug_drawer();
		friend void utils::register_cell(uint8_t cellID, uint32_t cellColorA, uint32_t cellColorB, World& world, CellUpdateCallback callback, uint32_t cellUserData);
#ifdef AVALANCHE_USE_CELL_POST_POSTPROCESSOR
		CellPostProcessor get_cell_post_processor() const;
		void set_cell_post_processor(CellPostProcessor cellPostProcessor);
#endif // AVALANCHE_USE_CELL_POST_POSTPROCESSOR

	private:
		void _fill_active_sector_buffer();
		int _get_sector_key(int x, int y);

	private:
		float _updateSectorInterval = 0;
		bool _isSleeping = true;
		static SimulationDebugDrawer* _debugDrawer;
		int _sectorCounter = 0;
		int _sectorSize = 500;

		std::vector<RegisteredCelllPrefab> _registeredCells;
		std::vector<SimulationSector*> _activeSimulationSectors;
		std::vector<SimulationSector*> _allSimulationSectors;
		std::unordered_map<int, SimulationSector*> _sectorLookup = {};

		CellPostProcessor _cellPostProcessor = nullptr;
		OnSectorCreated _onSectorCreatedCallback = nullptr;
		OnSectorUpdated _onSectorUpdatedCallback = nullptr;
		OnWorldReset _onWorldReset = nullptr;
	};

#endif
} // namespace avl

//--------------------------------------------
// LIBRARY IMPLEMENTATION
//--------------------------------------------
#ifdef AVALANCHE_IMPLEMENTATION

#include <algorithm>
#include <time.h>
#include <execution>
#include <omp.h>

#ifdef AVALANCHE_PROFILE
#include <chrono>
#include <iostream>
#include "profiler.h"
#endif

namespace avl {

	// Utils global implementations.
	//----------------------------------------
	void utils::register_cell(uint8_t cellID, uint32_t cellColorA, uint32_t cellColorB, World& world, CellUpdateCallback callback, uint32_t cellUserData)
	{
		RegisteredCelllPrefab newCellPrefab = RegisteredCelllPrefab();

		newCellPrefab.type = cellID;
		newCellPrefab.colorA = cellColorA;
		newCellPrefab.colorB = cellColorB;
		newCellPrefab.userData = cellUserData;
		newCellPrefab.updateCallback = callback;

		world._registeredCells.push_back(newCellPrefab);
	}

	int utils::get_random_value(int min, int max)
	{
		return min + (rand() % (max - min + 1));
	}

	float utils::get_random_value(float min, float max)
	{
		return min + (max - min) * ((float)rand() / (float)RAND_MAX);
	}

	float utils::get_random_value01()
	{
		return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
	}

	bool utils::get_chance(float input)
	{
		return ((float)rand() / RAND_MAX) < input;
	}

	uint32_t utils::lerp_packed_RGBA(uint32_t a, uint32_t b, uint32_t alpha)
	{
		uint32_t rb = (((b & 0xFF00FF) - (a & 0xFF00FF)) * alpha >> 8) + (a & 0xFF00FF);
		uint32_t ag = (((b >> 8 & 0xFF00FF) - (a >> 8 & 0xFF00FF)) * alpha >> 8) + (a >> 8 & 0xFF00FF);
		return (rb & 0xFF00FF) | (ag & 0xFF00FF) << 8;
	}

	uint32_t utils::lerp_packed_RGBA(uint32_t a, uint32_t b, float alpha)
	{
		return lerp_packed_RGBA(a, b, static_cast<uint32_t>(alpha * 256.0f));
	}

	uint32_t utils::pack_RGBA(float r, float g, float b, float a)
	{
		uint8_t ur = (uint8_t)(r * 255.0f);
		uint8_t ug = (uint8_t)(g * 255.0f);
		uint8_t ub = (uint8_t)(b * 255.0f);
		uint8_t ua = (uint8_t)(a * 255.0f);
		return ur | (ug << 8) | (ub << 16) | (ua << 24);
	}

	void utils::unpack_RGBA(uint32_t packedColor, float& r, float& g, float& b, float& a)
	{
		r = ((packedColor >> 0) & 0xFF) / 255.0f;
		g = ((packedColor >> 8) & 0xFF) / 255.0f;
		b = ((packedColor >> 16) & 0xFF) / 255.0f;
		a = ((packedColor >> 24) & 0xFF) / 255.0f;
	}

	uint32_t utils::HSVtoRGB(float h, float s, float v)
	{
		float r = 0, g = 0, b = 0;
		int i = int(h * 6.0f);
		float f = h * 6.0f - i;
		float p = v * (1.0f - s);
		float q = v * (1.0f - f * s);
		float t = v * (1.0f - (1.0f - f) * s);

		switch (i % 6)
		{
		case 0: r = v, g = t, b = p; break;
		case 1: r = q, g = v, b = p; break;
		case 2: r = p, g = v, b = t; break;
		case 3: r = p, g = q, b = v; break;
		case 4: r = t, g = p, b = v; break;
		case 5: r = v, g = p, b = q; break;
		}

		return utils::pack_RGBA(r, g, b, 1.0f);
	}

#ifndef AVALANCHE_DISABLE_LOGGING

	// Logging class implementations
	//----------------------------------------
	std::string Logging::_get_version_string()
	{
		return std::to_string(AVALANCHE_VERSION_MAJOR) + "." +
			std::to_string(AVALANCHE_VERSION_MINOR) + "." +
			std::to_string(AVALANCHE_VERSION_PATCH);
	}

	void Logging::_log(LogType type, const char* msg)
	{
		const char* level_str = (type == LogType::LOG_INFO) ? "info" :
			(type == LogType::LOG_WARNING) ? "warn" : "error";

		printf("[avl:%s%s%s] %s\n",
			_get_log_color(type),
			level_str,
			"\033[0m", // Reset
			msg);
	}

	const char* Logging::_get_log_color(const LogType type)
	{
		switch (type)
		{
		case LogType::LOG_INFO:    return "\033[32m";   // Green.
		case LogType::LOG_WARNING: return "\033[1;33m"; // Bright Yellow.
		case LogType::LOG_ERROR:   return "\033[1;31m"; // Bright Red.
		default:				   return "\033[0m";    // Reset.
		}
	}
#else 
	void logging::set_logging_level(LoggingLevels /*loggingLevel*/) {};
	std::string Logging::_get_version_string()
	{
		return std::to_string(AVALANCHE_VERSION_MAJOR) + "." +
			std::to_string(AVALANCHE_VERSION_MINOR) + "." +
			std::to_string(AVALANCHE_VERSION_PATCH);
	}
	void Logging::_log(LogType type, const char* msg) {};
	const char* Logging::_get_log_color(const LogType type) { return nullptr; }
#endif

	// CellUserData class implementations
	//----------------------------------------
	uint32_t CellUserData::pack() const { return flags | (static_cast<uint32_t>(value) << 16); }

	CellUserData CellUserData::unpack(uint32_t packed)
	{
		return CellUserData(
			static_cast<uint16_t>(packed & 0xFFFF),
			static_cast<uint16_t>((packed >> 16) & 0xFFFF)
		);
	}

	void CellUserData::set_flag(uint16_t flag) { flags |= flag; }

	void CellUserData::clear_flag(uint16_t flag) { flags &= ~flag; }

	void CellUserData::clear_all_flags() { flags = 0; }

	bool CellUserData::has_flag(uint16_t flag) const { return (flags & flag) != 0; }

	// CellUpdateContext implementations.
	//----------------------------------------

	void CellUpdateContext::move_cell(int toX, int toY) { sector->_move_cell(x, y, cellIndex, toX, toY, chunk, threadId); }

	void CellUpdateContext::swap_cell(int toX, int toY) { sector->_swap_cell(x, y, toX, toY, chunk, threadId); }

	bool CellUpdateContext::is_empty(int checkX, int checkY) const { return sector->_is_empty(checkX, checkY); }

	bool CellUpdateContext::is_empty(int checkX, int checkY, uint8_t& outCellID) const { return sector->_is_empty(checkX, checkY, outCellID); }

	bool CellUpdateContext::is_out_of_bounds(int checkX, int checkY) const { return sector->_is_out_of_bounds(checkX, checkY); }

	uint8_t CellUpdateContext::get_cell_id(int checkX, int checkY) const { return sector->_get_cell_id(sector->calculate_index(checkX, checkY)); }

	void CellUpdateContext::notify_chunk() const { sector->_notify_chunk(chunk); }

	void CellUpdateContext::notify_chunk(SectorSimulationChunk* newChunk) const { sector->_notify_chunk(newChunk); }

	void CellUpdateContext::notify_sector() const { sector->notify_sector(); }

	bool CellUpdateContext::get_chunk_safe(int sectorX, int sectorY, SectorSimulationChunk**& outChunk) { return sector->_get_chunk_safe(sectorX, sectorY, outChunk); }

	CellUserData CellUpdateContext::get_user_data() const { return CellUserData::unpack(userData); }

	CellUserData CellUpdateContext::get_user_data(int localX, int localY) const { return sector->_get_cell_user_data(localX, localY); }

	uint16_t CellUpdateContext::get_user_flags() const { return get_user_data().flags; }

	uint16_t CellUpdateContext::get_user_value() const { return get_user_data().value; }

	bool CellUpdateContext::has_user_flag(uint16_t flag) const { return get_user_data().has_flag(flag); }

	void CellUpdateContext::set_user_data(const CellUserData& data) { userData = data.pack(); }

	void CellUpdateContext::set_user_data(int localX, int localY, const CellUserData& data) { sector->_set_cell_user_data(localX, localY, data); }

	// SimulationSector implementations.
	//----------------------------------------

	SimulationSector::SimulationSector(int x, int y, int width, int height, int id, World* worldPtr)
	{
		worldX = x;
		worldY = y;

		simulationSize = width * height;

		this->id = id;

		this->width = width;
		this->height = height;

		_activeCellIDs = new uint8_t[simulationSize];
		_activeCellColors = new uint32_t[simulationSize];
		_activeCellsUserData = new uint32_t[simulationSize];

		for (int i = 0; i < simulationSize; i++)
		{
			_activeCellIDs[i] = 0;
			_activeCellColors[i] = 0;
			_activeCellsUserData[i] = 0;
		}

		const int chunkSize = 50;

		int numChunksWidth = this->width / chunkSize;
		int numChunksHeight = this->height / chunkSize;

		_allChunks.reserve(numChunksWidth * numChunksHeight);
		_chunkLookup.reserve(numChunksWidth * numChunksHeight);

		for (int i = 0; i < numChunksWidth; i++)
		{
			for (int j = 0; j < numChunksHeight; j++)
			{
				_create_chunk(i, j, chunkSize);
			}
		}

#ifdef _OPENMP
		int maxThreads = omp_get_max_threads();

#if AVALANCHE_MAX_THREADS == -1
		_numThreads = maxThreads;
#else
		_numThreads = (maxThreads < AVALANCHE_MAX_THREADS) ? maxThreads : AVALANCHE_MAX_THREADS;
#endif

		omp_set_num_threads(_numThreads);
#else
		_numThreads = 1;
#endif

		_threadCellChanges.resize(_numThreads);
		_threadCellSwaps.resize(_numThreads);

		for (int i = 0; i < _numThreads; i++)
		{
			_threadCellChanges[i].reserve(1000);
			_threadCellSwaps[i].reserve(1000);
		}

		_world = worldPtr;
	}

	SimulationSector::~SimulationSector()
	{
		delete[] _activeCellIDs;
		delete[] _activeCellColors;
		delete[] _activeCellsUserData;

		for (auto chunk : _allChunks) delete chunk;

		_allChunks.clear();
		_activeChunks.clear();
	}

	void SimulationSector::set_cell(int x, int y, RegisteredCelllPrefab* cell)
	{
		int index = calculate_index(x, y);
		if (_is_out_of_bounds(index))
			return;

		set_cell(index, cell);
	}

	void SimulationSector::set_cell(int x, int y, float cellColorAlphaMin, float cellColorAlphaMax, RegisteredCelllPrefab* cell)
	{
		int index = calculate_index(x, y);
		if (_is_out_of_bounds(index))
			return;

		set_cell(index, cellColorAlphaMin, cellColorAlphaMax, cell);
	}

	void SimulationSector::set_cell(int index, RegisteredCelllPrefab* cell)
	{
		_activeCellIDs[index] = cell->type;
		float lerpAlpha = utils::get_random_value01();
		_activeCellColors[index] = utils::lerp_packed_RGBA(cell->colorA, cell->colorB, lerpAlpha);
		_activeCellsUserData[index] = cell->userData;
	}

	void SimulationSector::set_cell(int index, float cellColorAlphaMin, float cellColorAlphaMax, RegisteredCelllPrefab* cell)
	{
		_activeCellIDs[index] = cell->type;
		float lerpAlpha = utils::get_random_value(cellColorAlphaMin, cellColorAlphaMax);
		_activeCellColors[index] = utils::lerp_packed_RGBA(cell->colorA, cell->colorB, lerpAlpha);
		_activeCellsUserData[index] = cell->userData;
	}

	void SimulationSector::notify_sector()
	{
		isSleeping = false;
		hasBeenUpdatedThisFrame = true;
	}

	void SimulationSector::_update()
	{
		_activeChunks.clear();

		for (int i = 0; i < _allChunks.size(); i++)
		{
			if (!_allChunks[i]->isSleeping)
				_activeChunks.push_back(_allChunks[i]);
		}

		if (_activeChunks.empty())
		{
			if (!hasBeenUpdatedThisFrame)
				isSleeping = true;
			hasBeenUpdatedThisFrame = false;
			return;
		}

		static const int MIN_CHUNKS_FOR_THREADING = 4;

		if (_activeChunks.size() >= MIN_CHUNKS_FOR_THREADING)
		{
			_process_chunks(_activeChunks);
		}
		else
		{
			_process_chunks_sigle_threaded(_activeChunks);
		}

		_commit_cells();
		_reset_chunks();

		if (!hasBeenUpdatedThisFrame)
			isSleeping = true;

		hasBeenUpdatedThisFrame = false;
	}

	void SimulationSector::_process_chunks(std::vector<SectorSimulationChunk*>& chunks)
	{
		for (int i = 0; i < _numThreads; i++)
		{
			_threadCellChanges[i].clear();
			_threadCellSwaps[i].clear();
		}

#pragma omp parallel for schedule(static)
		for (int i = 0; i < chunks.size(); i++)
		{
#ifdef _OPENMP
			int threadId = omp_get_thread_num();
#else
			int threadId = 0;
#endif

			SectorSimulationChunk* activeChunk = chunks[i];

			for (int y = 0; y < 50; y++)
			{
				for (int x = 0; x < 50; x++)
				{
					const int cellX = x + activeChunk->chunkX;
					const int cellY = y + activeChunk->chunkY;
					const int index = calculate_index(cellX, cellY);
					const uint8_t cellID = _get_cell_id(index);

					if (cellID == 0)
						continue;

					RegisteredCelllPrefab& prefab = _world->get_registered_cell_prefab(cellID);

					CellUpdateContext ctx = {};
					ctx.x = cellX;
					ctx.y = cellY;
					ctx.cellIndex = index;
					ctx.cellID = cellID;
					ctx.userData = _activeCellsUserData[index];
					ctx.color = _get_cell_color(index);
					ctx.sector = this;
					ctx.chunk = activeChunk;
					ctx.world = _world;
					ctx.threadId = threadId;

					if (prefab.updateCallback != nullptr)
						prefab.updateCallback(ctx);

#ifdef AVALANCHE_USE_CELL_POST_POSTPROCESSOR
					_world->get_cell_post_processor()(ctx);
#endif

					_activeCellsUserData[index] = ctx.userData;
				}
			}
		}
	}

	void SimulationSector::_process_chunks_sigle_threaded(std::vector<SectorSimulationChunk*>& chunks)
	{
		// Clear thread-local buffer 0.
		_threadCellChanges[0].clear();
		_threadCellSwaps[0].clear();

		for (int i = 0; i < chunks.size(); i++)
		{
			SectorSimulationChunk* activeChunk = chunks[i];

			for (int x = 0; x < 50; x++)
			{
				for (int y = 0; y < 50; y++)
				{
					const int cellX = x + activeChunk->chunkX;
					const int cellY = y + activeChunk->chunkY;
					const int index = calculate_index(cellX, cellY);
					const uint8_t cellID = _get_cell_id(index);

					if (cellID == 0)
						continue;

					RegisteredCelllPrefab& prefab = _world->get_registered_cell_prefab(cellID);

					CellUpdateContext ctx = {};
					ctx.x = cellX;
					ctx.y = cellY;
					ctx.cellIndex = index;
					ctx.cellID = cellID;
					ctx.userData = _activeCellsUserData[index];
					ctx.color = _get_cell_color(index);
					ctx.sector = this;
					ctx.chunk = activeChunk;
					ctx.world = _world;
					ctx.threadId = 0;

					if (prefab.updateCallback != nullptr)
						prefab.updateCallback(ctx);

#ifdef AVALANCHE_USE_CELL_POST_POSTPROCESSOR
					_world->get_cell_post_processor()(ctx);
#endif

					_activeCellsUserData[index] = ctx.userData;
				}
			}
		}
	}

#ifdef AVALANCHE_DEBUG_DRAWER
	void SimulationSector::_debug_draw()
	{
		static const SimulationDebugDrawer::DebugDrawColor red = { 230, 41, 55, 255 };
		static const SimulationDebugDrawer::DebugDrawColor green = { 0, 228, 48, 255 };
		static const SimulationDebugDrawer::DebugDrawColor purple = { 200, 122, 255, 255 };

		static SimulationDebugDrawer* debugDrawer = World::get_debug_drawer();
		debugDrawer->DrawBox(worldX, worldY, width, height, isSleeping ? purple : green);

		for (int i = 0; i < _allChunks.size(); i++)
		{
			SectorSimulationChunk* chunk = _allChunks[i];

			if (chunk->isSleeping)
				continue;

			debugDrawer->DrawBox(chunk->chunkX + worldX, chunk->chunkY + worldY, 50, 50, red);
		}
	}
#endif // AVALANCHE_DEBUG_DRAWER

	void SimulationSector::_reset_chunks()
	{
		_activeChunks.clear();

		for (int i = 0; i < _allChunks.size(); i++)
		{
			SectorSimulationChunk* chunk = _allChunks[i];

			if (!chunk->hasBeenUpdatedThisFrame)
				chunk->isSleeping = true;

			chunk->hasBeenUpdatedThisFrame = false;
		}
	}

	void SimulationSector::_commit_cells()
	{
		// Merge all thread-local buffers into main buffer.
		_cellChanges.clear();
		_cellSwaps.clear();

		size_t totalChanges = 0;
		size_t totalSwaps = 0;

		for (int i = 0; i < _numThreads; i++)
		{
			totalChanges += _threadCellChanges[i].size();
			totalSwaps += _threadCellSwaps[i].size();
		}

		_cellChanges.reserve(totalChanges);
		_cellSwaps.reserve(totalSwaps);

		// Merge cell changes.
		for (int i = 0; i < _numThreads; i++)
		{
			if (!_threadCellChanges[i].empty())
			{
				_cellChanges.insert(_cellChanges.end(),
					std::make_move_iterator(_threadCellChanges[i].begin()),
					std::make_move_iterator(_threadCellChanges[i].end()));
			}
		}

		// Merge cell swaps.
		for (int i = 0; i < _numThreads; i++)
		{
			if (!_threadCellSwaps[i].empty())
			{
				_cellSwaps.insert(_cellSwaps.end(),
					std::make_move_iterator(_threadCellSwaps[i].begin()),
					std::make_move_iterator(_threadCellSwaps[i].end()));
			}
		}

		if (_cellChanges.empty() && _cellSwaps.empty())
			return;

		if (!_cellChanges.empty())
		{
#ifdef AVALANCHE_PROFILE
			auto start = std::chrono::high_resolution_clock::now();
#endif

			std::sort(std::execution::par, _cellChanges.begin(), _cellChanges.end(),
				[](auto& a, auto& b) { return a.first < b.first; });

#ifdef AVALANCHE_PROFILE
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
			std::cout << "std sort took: " << duration.count() << " ns" << std::endl;
#endif

			size_t iprev = 0;
			_cellChanges.emplace_back(-1, -1);

			for (size_t i = 0; i < _cellChanges.size() - 1; i++)
			{
				if (_cellChanges[i].first != _cellChanges[i + 1].first)
				{
					size_t rand = iprev + avl::utils::get_random_value(0, (int)(i - iprev));

					size_t dst = _cellChanges[rand].first;
					size_t src = _cellChanges[rand].second;

					_set_cell_info(dst, _activeCellIDs[src], _activeCellColors[src], _activeCellsUserData[src]);
					_set_cell_info(src, 0, 0, 0);

					iprev = i + 1;
				}
			}
		}

		// Process swaps.
		for (const auto& swap : _cellSwaps)
		{
			const int fromIndex = calculate_index(swap.first.first, swap.first.second);
			const int toIndex = calculate_index(swap.second.first, swap.second.second);

			std::swap(_activeCellIDs[fromIndex], _activeCellIDs[toIndex]);
			std::swap(_activeCellColors[fromIndex], _activeCellColors[toIndex]);
			std::swap(_activeCellsUserData[fromIndex], _activeCellsUserData[toIndex]);
		}
	}

	void SimulationSector::_move_cell(int x, int y, int fromIndex, int toX, int toY, SectorSimulationChunk* currentChunk, int threadId)
	{
		int atLeftEdge = (x == currentChunk->chunkX);
		int atRightEdge = (x == currentChunk->chunkX + 49);
		int atTopEdge = (y == currentChunk->chunkY);
		int atBottomEdge = (y == currentChunk->chunkY + 49);

		int pingX = atRightEdge - atLeftEdge;
		int pingY = atBottomEdge - atTopEdge;

		SectorSimulationChunk** chunkBuffer = nullptr;

		if (pingX != 0 && _get_chunk_safe(x + pingX, y, chunkBuffer))
			_notify_chunk(chunkBuffer[0]);

		if (pingY != 0 && _get_chunk_safe(x, y + pingY, chunkBuffer))
			_notify_chunk(chunkBuffer[0]);

		if ((pingX != 0) && (pingY != 0) && _get_chunk_safe(x + pingX, y + pingY, chunkBuffer))
			_notify_chunk(chunkBuffer[0]);

		_notify_chunk(currentChunk);
		_notify_chunk(_get_chunk_unsafe(toX, toY));

		notify_sector();
		_threadCellChanges[threadId].emplace_back(calculate_index(toX, toY), fromIndex);
	}

	void SimulationSector::_swap_cell(int fromX, int fromY, int toX, int toY, SectorSimulationChunk* currentChunk, int threadId)
	{
		_notify_chunk(currentChunk);
		_notify_chunk(_get_chunk_unsafe(toX, toY));

		notify_sector();
		_threadCellSwaps[threadId].emplace_back(std::make_pair(fromX, fromY), std::make_pair(toX, toY));
	}

	void SimulationSector::_create_chunk(int chunkX, int chunkY, int size)
	{
		// Create new chunk.
		SectorSimulationChunk* newChunk = new SectorSimulationChunk
		{
			chunkX * size,
			chunkY * size,
			true,
			true
		};

		_allChunks.push_back(newChunk);
		_chunkLookup.emplace(_get_chunk_lookup_key(chunkX, chunkY), newChunk);
	}

	void SimulationSector::_notify_chunk(SectorSimulationChunk* chunk)
	{
		chunk->isSleeping = false;
		chunk->hasBeenUpdatedThisFrame = true;
	}

	int SimulationSector::_get_chunk_lookup_key(int chunkX, int chunkY)
	{
		return chunkX * 10 + chunkY;
	}

	SectorSimulationChunk* SimulationSector::_get_chunk_unsafe(int x, int y)
	{
		return _chunkLookup[_get_chunk_lookup_key(x / 50, y / 50)];
	}

	bool SimulationSector::_get_chunk_safe(int x, int y, SectorSimulationChunk**& outChunk)
	{
		outChunk = nullptr;
		int chunkKey = _get_chunk_lookup_key(x / 50, y / 50);

		if (chunkKey >= _chunkLookup.size() || chunkKey < 0) return false;
		else
		{
			outChunk = &_chunkLookup[chunkKey];
			return true;
		}
	}

	void SimulationSector::_set_cell_info(size_t index, uint8_t cellID, uint32_t cellColor, uint32_t userData)
	{
		_activeCellIDs[index] = cellID;
		_activeCellColors[index] = cellColor;
		_activeCellsUserData[index] = userData;
	}

	bool SimulationSector::_is_out_of_bounds(int x, int y) const { return !((x >= 0 && x < width) && (y >= 0 && y < height)); }

	bool SimulationSector::_is_out_of_bounds(int index) const { return index >= simulationSize; }

	int SimulationSector::calculate_index(int x, int y) const { return x + y * width; }

	uint8_t SimulationSector::_get_cell_id(int index) { return _activeCellIDs[index]; }

	uint32_t SimulationSector::_get_cell_color(int index) { return _activeCellColors[index]; }

	CellUserData SimulationSector::_get_cell_user_data(int x, int y) const { return CellUserData::unpack(_activeCellsUserData[calculate_index(x, y)]); }

	void SimulationSector::_set_cell_user_data(int x, int y, const CellUserData& data) { _activeCellsUserData[calculate_index(x, y)] = data.pack(); }

	bool SimulationSector::_is_empty(int index) { return _get_cell_id(index) == 0; }

	bool SimulationSector::_is_empty(int index, uint8_t& outCellID) { outCellID = _get_cell_id(index); return outCellID == 0; }

	bool SimulationSector::_is_empty(int x, int y) { return _is_empty(calculate_index(x, y)); }

	bool SimulationSector::_is_empty(int x, int y, uint8_t& outCellID) { return _is_empty(calculate_index(x, y), outCellID); }

	// World class implementations.
	//----------------------------------------
	/*static*/ SimulationDebugDrawer* World::_debugDrawer = nullptr;

	World::World()
	{
		srand((unsigned)time(nullptr));
		std::string version_msg = "Avalanche version [v" + Logging::_get_version_string() + "] initialized";
		Logging::_log_info(version_msg.c_str());

		_registeredCells.push_back({ 0, 0, 0 });
	}

	void World::step_world(const float deltaTime, const float fixedTimeStep)
	{
		static float accumulator = 0.0f;
		accumulator += deltaTime;

		while (accumulator >= fixedTimeStep)
		{
			_fill_active_sector_buffer();

#pragma omp parallel for schedule(dynamic)
			for (int i = 0; i < _activeSimulationSectors.size(); i++)
			{
				_activeSimulationSectors[i]->_update();
			}

			if (_onSectorUpdatedCallback)
			{
				for (SimulationSector* sector : _activeSimulationSectors)
				{
					_onSectorUpdatedCallback(sector);
				}
			}

			accumulator -= fixedTimeStep;
		}
	}

	void World::step_world_without_callback(const float deltaTime, const float fixedTimeStep)
	{
		static float accumulator = 0.0f;
		accumulator += deltaTime;

		while (accumulator >= fixedTimeStep)
		{
			_fill_active_sector_buffer();

			for (SimulationSector* sector : _activeSimulationSectors)
			{
				sector->_update();
			}

			accumulator -= fixedTimeStep;
		}
	}

	void World::run_world_one_tick()
	{
		_fill_active_sector_buffer();

		for (auto* sector : _activeSimulationSectors)
			sector->_update();
	}

	void World::reset_world()
	{
		_allSimulationSectors.clear();
		_activeSimulationSectors.clear();
		_sectorLookup.clear();

		if (_onWorldReset)
			_onWorldReset();

		_sectorCounter = 0;
		Logging::_log_info("World reset complete");
	}

	bool World::is_sleeping() const
	{
		return _isSleeping;
	}

	bool World::is_postion_empty(int worldX, int worldY)
	{
		SimulationSector* sector = try_get_sector(worldX, worldY);

		int localX = worldX - sector->worldX;
		int localY = worldY - sector->worldY;

		return sector->_is_empty(localX, localY);
	}

	bool World::is_position_out_of_bounds(int worldX, int worldY)
	{
		SimulationSector* sector = try_get_sector(worldX, worldY);

		if (sector == nullptr)
			return true;

		int localX = worldX - sector->worldX;
		int localY = worldY - sector->worldY;

		return sector->_is_out_of_bounds(localX, localY);
	}

	RegisteredCelllPrefab& World::get_registered_cell_prefab(int index)
	{
		return _registeredCells[index];
	}

	void World::set_on_sector_created_listener(OnSectorCreated callback)
	{
		_onSectorCreatedCallback = callback;
	}

	void World::set_on_sector_updated_listener(OnSectorUpdated callback)
	{
		_onSectorUpdatedCallback = callback;
	}

	void World::set_on_world_reset_listener(OnWorldReset callback)
	{
		_onWorldReset = callback;
	}

#ifdef AVALANCHE_DEBUG_DRAWER
	void World::debug_draw()
	{
		for (auto* activeSector : _allSimulationSectors)
			activeSector->_debug_draw();
	}
#endif // AVALANCHE_DEBUG_DRAWER

	SectorSimulationChunk* World::try_get_chunk(int worldX, int worldY)
	{
		SimulationSector* sector = try_get_sector(worldX, worldY);
		return sector->_get_chunk_unsafe(worldX - sector->worldX, worldY - sector->worldY);
	}

	uint8_t World::get_cell_id_safe(const int worldX, const int worldY)
	{
		SimulationSector* sector = get_or_create_sector(worldX, worldY);

		int localX = worldX - sector->worldX;
		int localY = worldY - sector->worldY;

		return sector->_get_cell_id(sector->calculate_index(localX, localY));
	}

	uint8_t World::get_cell_id_unsafe(const int worldX, const int worldY)
	{
		SimulationSector* sector = try_get_sector(worldX, worldY);

		int localX = worldX - sector->worldX;
		int localY = worldY - sector->worldY;

		return sector->_get_cell_id(sector->calculate_index(localX, localY));
	}

	uint32_t World::get_cell_color(const int worldX, const int worldY)
	{
		SimulationSector* sector = try_get_sector(worldX, worldY);

		int localX = worldX - sector->worldX;
		int localY = worldY - sector->worldY;

		return sector->_get_cell_color(sector->calculate_index(localX, localY));
	}

	CellUserData World::get_cell_user_data(int worldX, int worldY)
	{
		SimulationSector* sector = try_get_sector(worldX, worldY);

		int localX = worldX - sector->worldX;
		int localY = worldY - sector->worldY;

		return sector->_get_cell_user_data(localX, localY);
	}

	void World::set_cell_user_data(int worldX, int worldY, const CellUserData& data)
	{
		SimulationSector* sector = try_get_sector(worldX, worldY);

		int localX = worldX - sector->worldX;
		int localY = worldY - sector->worldY;

		sector->_set_cell_user_data(localX, localY, data);
	}

	void World::plot_line(int x0, int y0, int x1, int y1, int registeredCellID, float /*placeChance*/)
	{
		int dx = abs(x1 - x0);
		int	sx = x0 < x1 ? 1 : -1;
		int	dy = -abs(y1 - y0);
		int	sy = y0 < y1 ? 1 : -1;
		int error = dx + dy;

		while (true)
		{
			plot_cell(x0, y0, registeredCellID);
			int e2 = 2 * error;

			if (e2 >= dy)
			{
				if (x0 == x1)
					break;

				error = error + dy;
				x0 = x0 + sx;
			}

			if (e2 <= dx)
			{
				if (y0 == y1)
					break;

				error = error + dx;
				y0 = y0 + sy;
			}
		}
	}

	void World::plot_line_carefull(int x0, int y0, int x1, int y1, int registeredCellID, float /*placeChance*/)
	{
		int dx = abs(x1 - x0);
		int	sx = x0 < x1 ? 1 : -1;
		int	dy = -abs(y1 - y0);
		int	sy = y0 < y1 ? 1 : -1;
		int error = dx + dy;

		while (true)
		{
			if (get_cell_id_safe(x0, y0) == 0)
				plot_cell(x0, y0, registeredCellID);
			else
				return;

			int e2 = 2 * error;

			if (e2 >= dy)
			{
				if (x0 == x1)
					break;

				error = error + dy;
				x0 = x0 + sx;
			}

			if (e2 <= dx)
			{
				if (y0 == y1)
					break;

				error = error + dx;
				y0 = y0 + sy;
			}
		}
	}

	void World::plot_rectangle(int x0, int y0, int x1, int y1, int registeredCellID, float /*placeChance*/)
	{
		if (x0 > x1) std::swap(x0, x1);
		if (y0 > y1) std::swap(y0, y1);

		for (int y = y0; y <= y1; y++)
			plot_line(x0, y, x1, y, registeredCellID);
	}

	void World::plot_circle(int xPosition, int yPosition, int size, int registeredCellID, float placeChance)
	{
		for (int x = -size; x <= size; x++)
		{
			for (int y = -size; y <= size; y++)
			{
				if (!avl::utils::get_chance(placeChance) && sqrt(x * x + y * y) <= size)
				{
					int newXPosition = xPosition + x;
					int newYPosition = yPosition + y;

					plot_cell(newXPosition, newYPosition, registeredCellID);
				}
			}
		}
	}

	void World::plot_cell(int x, int y, int registeredCellID, bool notify)
	{
		SimulationSector* sector = get_or_create_sector(x, y);
		auto* cell = &_registeredCells[registeredCellID];
		sector->set_cell(x - sector->worldX, y - sector->worldY, cell);

		if (notify)
		{
			sector->_notify_chunk(sector->_get_chunk_unsafe(x - sector->worldX, y - sector->worldY));
			sector->notify_sector();
			_isSleeping = false;
		}
	}

	void World::plot_cell(int x, int y, int registeredCellID, float alphaMin, float alphaMax, bool notify)
	{
		SimulationSector* sector = get_or_create_sector(x, y);
		auto* cell = &_registeredCells[registeredCellID];
		sector->set_cell(x - sector->worldX, y - sector->worldY, alphaMin, alphaMax, cell);

		if (notify)
		{
			sector->_notify_chunk(sector->_get_chunk_unsafe(x - sector->worldX, y - sector->worldY));
			sector->notify_sector();
			_isSleeping = false;
		}
	}

	void World::plot_cell(int x, int y, int registeredCellID, uint32_t overrideColor, bool notify)
	{
		SimulationSector* sector = get_or_create_sector(x, y);
		auto* cell = &_registeredCells[registeredCellID];
		int cellIndex = sector->calculate_index(x - sector->worldX, y - sector->worldY);
		sector->set_cell(cellIndex, cell);
		sector->_activeCellColors[cellIndex] = overrideColor;

		if (notify)
		{
			sector->_notify_chunk(sector->_get_chunk_unsafe(x - sector->worldX, y - sector->worldY));
			sector->notify_sector();
			_isSleeping = false;
		}
	}

	void World::plot_cell_if_empty(int x, int y, int registeredCellID, bool notify)
	{
		SimulationSector* sector = get_or_create_sector(x, y);

		if (!sector->_is_empty(x - sector->worldX, y - sector->worldY))
			return;

		auto* cell = &_registeredCells[registeredCellID];
		sector->set_cell(x - sector->worldX, y - sector->worldY, cell);

		if (notify)
		{
			sector->_notify_chunk(sector->_get_chunk_unsafe(x - sector->worldX, y - sector->worldY));
			sector->notify_sector();
			_isSleeping = false;
		}
	}

	void World::_fill_active_sector_buffer()
	{
		bool foundActiveSector = false;
		_activeSimulationSectors.clear();

		for (int i = 0; i < _allSimulationSectors.size(); i++)
		{
			SimulationSector* sector = _allSimulationSectors[i];

			if (!sector->isSleeping)
			{
				_activeSimulationSectors.push_back(sector);
				foundActiveSector = true;
				_isSleeping = false;
			}
		}

		if (!foundActiveSector)
			_isSleeping = true;
	}

	int World::_get_sector_key(int worldX, int worldY)
	{
		return worldX * 10 + worldY;
	}

	SimulationSector* World::create_sector(int worldX, int worldY)
	{
		SimulationSector* newSector = new SimulationSector(worldX, worldY, _sectorSize, _sectorSize, _sectorCounter, this);
		_sectorCounter++;

		_allSimulationSectors.push_back(newSector);
		_sectorLookup.emplace(_get_sector_key(worldX, worldY), newSector);

		if (_onSectorCreatedCallback != nullptr)
			_onSectorCreatedCallback(newSector);

		return newSector;
	}

	SimulationSector* World::get_or_create_sector(int worldX, int worldY)
	{
		int sectorX = (worldX < 0 ? (worldX - _sectorSize + 1) : worldX) / _sectorSize * _sectorSize;
		int sectorY = (worldY < 0 ? (worldY - _sectorSize + 1) : worldY) / _sectorSize * _sectorSize;

		for (int i = 0; i < _allSimulationSectors.size(); i++)
		{
			SimulationSector* sandSector = _allSimulationSectors[i];

			if (sandSector->worldX == sectorX && sandSector->worldY == sectorY)
				return sandSector;
		}

		return create_sector(sectorX, sectorY);
	}

	SimulationSector* World::try_get_sector(int worldX, int worldY)
	{
		int sectorX = (worldX < 0 ? (worldX - _sectorSize + 1) : worldX) / _sectorSize * _sectorSize;
		int sectorY = (worldY < 0 ? (worldY - _sectorSize + 1) : worldY) / _sectorSize * _sectorSize;

		for (int i = 0; i < _allSimulationSectors.size(); i++)
		{
			SimulationSector* sandSector = _allSimulationSectors[i];

			if (sandSector->worldX == sectorX && sandSector->worldY == sectorY)
				return sandSector;
		}

		return nullptr;
	}

	SimulationSector* World::get_sector_direct(int sectorIndex)
	{
		return _allSimulationSectors[sectorIndex];
	}

	int World::get_sector_count()
	{
		return (int)_allSimulationSectors.size();
	}

	void World::set_debug_drawer(SimulationDebugDrawer* debugDrawer)
	{
		_debugDrawer = debugDrawer;
	}

	SimulationDebugDrawer* World::get_debug_drawer()
	{
		return _debugDrawer;
	}

#ifdef AVALANCHE_USE_CELL_POST_POSTPROCESSOR
	CellPostProcessor World::get_cell_post_processor() const
	{
		return _cellPostProcessor;
	}

	void World::set_cell_post_processor(CellPostProcessor cellPostProcessor)
	{
		_cellPostProcessor = cellPostProcessor;
	}
#endif // AVALANCHE_USE_CELL_POST_POSTPROCESSOR

} // namespace avl

#endif

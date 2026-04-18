#include "demo_imgui_interface.hpp"
#include "../rlImGui/rlImGui.h"
#include "../ImGui/imgui.h"
#include <DemoCore/include/cell-flags.hpp>

DemoImguiInterface::DemoImguiInterface()
{
	ImGuiIO& io = ImGui::GetIO();

	ImFontConfig font_config;
	font_config.SizePixels = 23.0f;
	io.Fonts->AddFontDefault(&font_config);

	// Configure Font Awesome merge.
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = 20.0f;

	static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

	io.Fonts->AddFontFromFileTTF("fa-solid-900.ttf", 20.0f, &icons_config, icon_ranges);
	io.Fonts->Build();
	rlImGuiReloadFonts();

	stoneTexture = LoadTexture("assets/stone_button.png");
	woodTexture = LoadTexture("assets/wood_button.png");
	steelTexture = LoadTexture("assets/steel_button.png");
	magnetNorthTexture = LoadTexture("assets/magnet_north_button.png");
	magnetSouthTexture = LoadTexture("assets/magnet_south_button.png");
	sandTexture = LoadTexture("assets/sand_button.png");
	lavaTexture = LoadTexture("assets/lava_button.png");
	waterTexture = LoadTexture("assets/water_button.png");
	acidTexture = LoadTexture("assets/acid_button.png");
	smokeTexture = LoadTexture("assets/smoke_button.png");
	fireTexture = LoadTexture("assets/fire_button.png");
	gunPowderTexture = LoadTexture("assets/gun_powder_button.png");
	fireworkTexture = LoadTexture("assets/firework_button.png");
	plantTexture = LoadTexture("assets/plant_button.png");
	sparkTexture = LoadTexture("assets/spark_button.png");
	unstableMatterTexture = LoadTexture("assets/unstable_matter_button.png");
	quantumMatterTextureTexture = LoadTexture("assets/quantum_matter_button.png");
}

void DemoImguiInterface::DrawSimulationControls(bool& showDebugOverlay, bool& isStepping, float& currentFixedTimeStep, float& targetFramerate, int& currentPlaceSize, avl::World& world)
{
	if (ImGui::Begin("Buttons", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove))
	{
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImGuiIO& io = ImGui::GetIO();
		float displayWidth = io.DisplaySize.x;
		ImVec2 pos = { (displayWidth - windowSize.x) * 0.5f, ImGui::GetWindowHeight() };

		ImGui::SetWindowPos(pos);

		if (ImGui::Button(ICON_FA_MAGNIFYING_GLASS))
		{
			showDebugOverlay = !showDebugOverlay;
		}

		ImGui::SameLine();

		if (ImGui::Button(ICON_FA_PLAY))
		{
			mode = DEMO_MODE::ACTIVE;
			isStepping = false;
			HideCursor();

			currentFixedTimeStep = 1.0f / targetFramerate;
		}

		if (mode == DEMO_MODE::ACTIVE || mode == DEMO_MODE::PAUSED)
		{
			ImGui::SameLine();

			if (ImGui::Button(ICON_FA_PAUSE))
			{
				mode = DEMO_MODE::PAUSED;
			}

			ImGui::SameLine();

			if (ImGui::Button(ICON_FA_FORWARD_STEP))
			{
				isStepping = true;
				currentFixedTimeStep = 1.0f / targetFramerate;
			}

			ImGui::SameLine();

			if (ImGui::Button(ICON_FA_STOP))
			{
				mode = DEMO_MODE::INACTIVE;
				world.reset_world();
				world.create_sector(0, 0);
			}
		}

		ImVec2 currentWindowPosition = ImGui::GetWindowPos();
		ImVec2 currentWindowSize = ImGui::GetWindowSize();

		const int horizontalPadding = 10;
		static int newTargetFrameRate = (int)targetFramerate;

		ImGui::SetNextWindowPos({ currentWindowPosition.x + currentWindowSize.x + horizontalPadding, currentWindowPosition.y });
		ImGui::SetNextItemWidth(200.0f);
		ImGui::Begin("## world update slider", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar);

		if (ImGui::DragInt("hz", &newTargetFrameRate, 0.3f, 1, 512, "%d", ImGuiSliderFlags_AlwaysClamp))
		{
			targetFramerate = (float)newTargetFrameRate;

			if (mode == DEMO_MODE::ACTIVE && !isStepping)
				currentFixedTimeStep = 1.0f / targetFramerate;
		}
		auto hzSliderSize = ImGui::GetWindowSize();

		ImGui::End();

		ImGui::SetNextWindowPos({ currentWindowPosition.x + currentWindowSize.x + hzSliderSize.x + 20, currentWindowPosition.y });
		ImGui::SetNextItemWidth(200.0f);
		ImGui::Begin("## debug place size", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar);
		ImGui::DragInt("px", &currentPlaceSize, 0.3f, 1, 50, "%d", ImGuiSliderFlags_AlwaysClamp);
		ImVec2 sliderPosition = ImGui::GetWindowPos();
		ImVec2 sliderSize = ImGui::GetWindowSize();
		ImGui::End();

		/*ImGui::SetNextWindowPos({ sliderPosition.x + sliderSize.x + 10, sliderPosition.y });
		ImGui::SetNextItemWidth(100.0f);
		ImGui::Begin("## cell id", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoTitleBar);
		ImGui::DragInt("id", &currentCellID, 0.3f, 1, 50, "%d", ImGuiSliderFlags_AlwaysClamp);
		ImGui::End();*/
	}
	ImGui::End();

	ShowMaterialButtons();
}

void DemoImguiInterface::DrawDebugOverlay(double averageDuration, double maxDuration, const float targetFramerate, Vector2 worldMouse, size_t debugLineCount, avl::World& world)
{
	ImGui::SetNextWindowBgAlpha(0.35f);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
		ImGuiWindowFlags_NoNav;

	if (ImGui::Begin("Debug overlay", nullptr, window_flags))
	{
		ImVec2 size = ImGui::GetWindowSize();
		ImGuiIO& guiIO = ImGui::GetIO();

		ImGui::SeparatorText("Info");
		ImGui::Text("Resolution: (%.1f, %.1f)", guiIO.DisplaySize.x, guiIO.DisplaySize.y);

		if (ImGui::IsMousePosValid())
		{
			ImGui::Text("Screen Mouse position: x(%.1f), y(%.1f)", guiIO.MousePos.x, guiIO.MousePos.y);
			ImGui::Text("World mouse position: x(%.1f), y(%.1f)", worldMouse.x, worldMouse.y);
		}

		ImGui::Text("Debug line count: %i", (int)debugLineCount);
		ImGui::SeparatorText("Performance");
		ImGui::Text("Raw framerate: (%.1f)", guiIO.Framerate);
		ImGui::Text("Simulation target: (%.1f hz | %.4f s)", targetFramerate, 1.0f / targetFramerate);
		ImGui::Text("Average world update time: (%.4f) ms", averageDuration);
		ImGui::Text("Max average world update time: (%.4f) ms", maxDuration);

		ImVec2 currentWindowPosition = ImGui::GetWindowPos();
		ImVec2 currentWindowSize = ImGui::GetWindowSize();
		const int heightPadding = 10;

		if (IsKeyDown(KEY_LEFT_CONTROL))
		{
			ImGui::SetNextWindowPos({ currentWindowPosition.x, currentWindowPosition.y + currentWindowSize.y + heightPadding });

			ImGui::SetNextWindowBgAlpha(0.35f);
			ImGui::Begin("CELL INSPECTOR", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoInputs);

			ImGuiIO io = ImGui::GetIO();
			Vector2 mousePosition = { floor(worldMouse.x), floor(worldMouse.y) + 1 };

			int sectorID = world.get_or_create_sector((int)mousePosition.x, (int)mousePosition.y)->id;
			avl::SimulationSector* sector = world.try_get_sector((int)mousePosition.x, (int)mousePosition.y);

			if (sector != nullptr)
			{
				uint8_t cellID = world.get_cell_id_unsafe((int)mousePosition.x, (int)mousePosition.y);
				avl::CellUserData cellData = world.get_cell_user_data((int)mousePosition.x, (int)mousePosition.y);

				ImGui::Text("Selected cell position: x %i, y %i", (int)mousePosition.x, (int)mousePosition.y);
				ImGui::Text("Selected cell type: %i", cellID);
				ImGui::Text("Selected cell raw flags: 0x%04X", cellData.flags);
				ImGui::Text("Selected cell raw value: %u", cellData.value);
				ImGui::Text("Selected cell sector ID: %i", sectorID);

				ImGui::SeparatorText("Cell property flags:");

				if (cellData.flags != 0)
				{
					if (cellData.has_flag(CellFlags::HOT))							ImGui::BulletText("HOT");
					if (cellData.has_flag(CellFlags::FLAMMABLE))					ImGui::BulletText("FLAMMABLE");
					if (cellData.has_flag(CellFlags::LONG_LIFE_TIME))				ImGui::BulletText("LONG_LIFE_TIME");
					if (cellData.has_flag(CellFlags::CHARACTER_TRAVERSABLE))		ImGui::BulletText("CHARACTER_TRAVERSABLE");
					if (cellData.has_flag(CellFlags::PLANT_SEED))					ImGui::BulletText("PLANT_SEED");
					if (cellData.has_flag(CellFlags::GROWN_PLANT))					ImGui::BulletText("GROWN_PLANT");
					if (cellData.has_flag(CellFlags::SHORT_LIFE_TIME))				ImGui::BulletText("SHORT_LIFE_TIME");
					if (cellData.has_flag(CellFlags::IMMOVABLE))					ImGui::BulletText("IMMOVABLE");
					if (cellData.has_flag(CellFlags::CONDUCTIVE))					ImGui::BulletText("CONDUCTIVE");
					if (cellData.has_flag(CellFlags::CHARGED))						ImGui::BulletText("CHARGED");
					if (cellData.has_flag(CellFlags::RANDOM_COLOR))					ImGui::BulletText("RANDOM_COLOR");
				}
				else
				{
					ImGui::BulletText("NONE");
				}
			}
			ImGui::End();
		}
	}
	ImGui::End();
}

void DemoImguiInterface::ShowMaterialButtons()
{
	ImGui::Begin("Materials", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove);

	ImVec2 windowSize = ImGui::GetWindowSize();
	ImGuiIO& io = ImGui::GetIO();
	float displayWidth = io.DisplaySize.x;
	const float downPadding = 20.0f;
	ImVec2 pos = { (displayWidth - windowSize.x) * 0.5f, io.DisplaySize.y - ImGui::GetWindowSize().y - downPadding };

	ImGui::SetWindowPos(pos);

	// Helper lambda to draw a selectable material button.
	auto DrawMaterialButton = [this](const char* id, Texture2D* texture, int index) 
	{
		if (selectedMaterialIndex == index) 
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.6f, 1.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.4f, 0.7f, 1.0f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.5f, 0.9f, 1.0f));
		}

		bool clicked = rlImGuiImageButtonSize(id, texture, {95, 45});

		if (selectedMaterialIndex == index) 
			ImGui::PopStyleColor(3);

		if (clicked)
			selectedMaterialIndex = index;

		return clicked;
	};

	DrawMaterialButton("stone_button", &stoneTexture, 1);
	ImGui::SameLine();
	DrawMaterialButton("wood_button", &woodTexture, 5);
	ImGui::SameLine();
	DrawMaterialButton("steel_button", &steelTexture, 11);
	ImGui::SameLine();
	DrawMaterialButton("sand_button", &sandTexture, 2);
	ImGui::SameLine();
	DrawMaterialButton("lava_button", &lavaTexture, 7);
	ImGui::SameLine();
	DrawMaterialButton("water_button", &waterTexture, 3);
	ImGui::SameLine();
	DrawMaterialButton("acid_button", &acidTexture, 14);
	ImGui::SameLine();
	DrawMaterialButton("smoke_button", &smokeTexture, 6);
	ImGui::SameLine();
	DrawMaterialButton("fire_button", &fireTexture, 4);
	ImGui::SameLine();
	DrawMaterialButton("gun_powder_button", &gunPowderTexture, 8);
	ImGui::SameLine();
	DrawMaterialButton("firework_button", &fireworkTexture, 15);
	ImGui::SameLine();
	DrawMaterialButton("plant_button", &plantTexture, 9);
	ImGui::SameLine();
	DrawMaterialButton("spark_button", &sparkTexture, 10);
	ImGui::SameLine();
	DrawMaterialButton("unstable_matter_button", &unstableMatterTexture, 12);
	ImGui::SameLine();
	DrawMaterialButton("quantum_matter_button", &quantumMatterTextureTexture, 13);

	ImGui::End();
}


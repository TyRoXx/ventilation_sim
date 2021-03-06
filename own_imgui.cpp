#include "own_imgui.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include <array>

constexpr std::array<char*, 5> materialNames { "Air", "Snow", "Wall", "Sand", "Eraser" };

void menuBar(World& world, bool& isDemoVisible)
{
    if (!ImGui::BeginMainMenuBar()) {
        return;
    }

    if (ImGui::BeginMenu("File")) {
        if (ImGui::MenuItem("New", "Ctrl+N")) {
            clearWorld(world);
        }
        if (ImGui::MenuItem("Save", "Ctrl+S")) {
            saveWorldToFile(world, "world.dat");
        }
        if (ImGui::MenuItem("Load", "Ctrl+O")) {
            loadWorldFromFile(world, "world.dat");
        }
        ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("ImGui demo")) {
            isDemoVisible = !isDemoVisible;
        }
        ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
}

void addBrushTreeNode(SimulationSettings& settings)
{
    for (size_t i = 0; i < materialNames.size(); i++) {
        if (ImGui::RadioButton(materialNames[i], (settings.currentMaterial == static_cast<Cell>(i)))) {
            settings.currentMaterial = static_cast<Cell>(i);
        }
        if ((i + 1) < materialNames.size()) {
            ImGui::SameLine();
        }
    }

    ImGui::SliderInt("Size", &settings.brushSize, 1, 100);
    ImGui::SliderFloat("Strength", &settings.brushStrength, 0.0, 1.0);
}

void addSimulationSettingsNode(SimulationSettings& settings)
{
    ImGui::SliderInt("Time between steps (ms)", &settings.timeBetweenStepsInMilliseconds, 0, 1000);
    ImGui::Checkbox("Pause", &settings.isPaused);
}

void addProfilingNode(const ProfilingInfo& profilingInfo)
{
    if (!ImGui::TreeNode("Profiling")) {
        return;
    }
    ImGui::Text(("Cells filled: " + std::to_string(profilingInfo.nonEmptyCells)).c_str());
    ImGui::Text(("Cells changed: " + std::to_string(profilingInfo.cellsChanged)).c_str());
    ImGui::Text(("Simulation time: " + std::to_string(profilingInfo.simulationTime.count()) + " ms").c_str());
    ImGui::Text(("Render time: " + std::to_string(profilingInfo.renderTime.count()) + " ms").c_str());
    ImGui::TreePop();
}

void renderUI(World& world, SimulationSettings& settings, const ProfilingInfo& profilingInfo, bool& isDemoVisible)
{
    menuBar(world, isDemoVisible);

    ImGui::Begin("Toolbox");
    addBrushTreeNode(settings);
    addSimulationSettingsNode(settings);
    addProfilingNode(profilingInfo);
    ImGui::End();

    if (isDemoVisible) {
        ImGui::ShowDemoWindow(&isDemoVisible);
    }
}

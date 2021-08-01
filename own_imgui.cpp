#include "own_imgui.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include "main.hpp"
#include "world.hpp"
#include <vector>

void menuBar(std::vector<Cell>& world)
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
    ImGui::EndMainMenuBar();
}

void addBrushTreeNode(SimulationSettings& settings)
{
    if (!ImGui::TreeNode("Brush Settings")) {
        return;
    }
    ImGui::SliderInt("Size", &settings.brushSize, 1, 100);

    const char* itemLabels[] = { "Air", "Snow", "Wall" };
    const char* currentLabel = itemLabels[static_cast<size_t>(settings.currentTool)];

    if (ImGui::BeginCombo("Material", currentLabel)) {
        for (size_t i = 0; i < 3; i++) {
            bool isSelected = itemLabels[i] == currentLabel;
            if (ImGui::Selectable(itemLabels[i], isSelected)) {
                currentLabel = itemLabels[i];
                settings.currentTool = static_cast<Cell>(i);
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    ImGui::TreePop();
}

void addsimulationSettingsNode(SimulationSettings& settings)
{
    if (!ImGui::TreeNode("Speed")) {
        return;
    }
    ImGui::SliderInt("Time between steps (ms)", &settings.timeBetweenStepsInMilliseconds, 0, 1000);
    ImGui::Checkbox("Pause", &settings.isPaused);
    ImGui::TreePop();
}

void renderUI(std::vector<Cell>& world, SimulationSettings& settings)
{
    menuBar(world);

    ImGui::Begin("Toolbox");
    addBrushTreeNode(settings);
    addsimulationSettingsNode(settings);
    ImGui::End();
}

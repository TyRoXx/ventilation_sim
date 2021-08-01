#pragma once

#include "world.hpp"
#include <string>
#include <vector>

struct SimulationSettings {
    int timeBetweenStepsInMilliseconds;
    bool isPaused;
    int brushSize;
    Cell currentTool;
};

void clearWorld(std::vector<Cell>& world);
void saveWorldToFile(const std::vector<Cell>& world, const std::string& fileName);
void loadWorldFromFile(std::vector<Cell>& world, const std::string& fileName);

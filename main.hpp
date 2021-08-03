#pragma once

#include <string>
#include <vector>
#include "simulation.hpp"

struct SimulationSettings {
    int timeBetweenStepsInMilliseconds;
    bool isPaused;
    int brushSize;
    Cell currentTool;
};

void clearWorld(World& world);
void saveWorldToFile(const World& world, const std::string& fileName);
void loadWorldFromFile(World& world, const std::string& fileName);

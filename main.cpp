#include "main.hpp"
#include "imgui-SFML.h"
#include "imgui.h"
#include "own_imgui.hpp"
#include "simulation.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <array>
#include <chrono>
#include <fstream>
#include <iostream>

sf::Color renderCell(const Cell& cell)
{
    switch (cell) {
    case Cell::Air:
        return sf::Color(0, 0, 0, 255);
    case Cell::Snow:
        return sf::Color(255, 255, 255, 255);
    case Cell::Wall:
        return sf::Color(128, 128, 128, 255);
    case Cell::Sand:
        return sf::Color(180, 110, 0, 255);
    case Cell::Eraser:
        return sf::Color(255, 0, 0, 255);
    }
    VENT_UNREACHABLE();
}

void renderWorld(sf::Image& into, const Cell& front, const Point& worldSize)
{
    assert(static_cast<ptrdiff_t>(into.getSize().x) == worldSize.x);
    assert(static_cast<ptrdiff_t>(into.getSize().y) == worldSize.y);

    // pixels are marked const for some reason
    sf::Uint8* const pixels = const_cast<sf::Uint8*>(into.getPixelsPtr());

    const size_t numberOfCells = (worldSize.x * worldSize.y);
    for (size_t i = 0; i < numberOfCells; ++i) {
        const sf::Color cellColor = renderCell((&front)[i]);
        sf::Uint8* const cellPixel = pixels + (i * 4);
        cellPixel[0] = cellColor.r;
        cellPixel[1] = cellColor.g;
        cellPixel[2] = cellColor.b;
        cellPixel[3] = cellColor.a;
    }
}

void clearWorld(World& world)
{
    std::fill(world.Cells.begin(), world.Cells.end(), Cell::Air);
}

void saveWorldToFile(const World& world, const std::string& fileName)
{
    std::ofstream file(fileName, std::ofstream::binary);
    file.write(reinterpret_cast<const char*>(world.Cells.data()), sizeof(Cell) * world.Cells.size());
}

void loadWorldFromFile(World& world, const std::string& fileName)
{
    std::ifstream file(fileName, std::ifstream::binary);
    file.read(reinterpret_cast<char*>(world.Cells.data()), sizeof(Cell) * world.Cells.size());
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 800), "Ventilation Simulator 2021");
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);

    const Point worldSize(window.getSize().x, window.getSize().y);

    World world(worldSize, Cell::Air);

    sf::Image worldImage;
    worldImage.create(static_cast<unsigned>(worldSize.x), static_cast<unsigned>(worldSize.y));

    bool isMouseLeftDown = false;
    sf::Vector2u mousePosition;

    sf::Clock worldStepClock;
    sf::Time nextWorldStep = worldStepClock.getElapsedTime();

    SimulationSettings settings;
    ProfilingInfo profiling;

    bool isDemoVisible = false;

    sf::Clock deltaClock;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            ImGui::SFML::ProcessEvent(event);

            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (ImGui::IsAnyItemHovered() || ImGui::IsAnyItemActive() || ImGui::IsAnyItemFocused()) {
                continue;
            }

            if (!ImGui::GetIO().WantCaptureMouse) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (event.mouseButton.button == sf::Mouse::Button::Left) {
                        isMouseLeftDown = true;
                    }
                    mousePosition = sf::Vector2u(event.mouseButton.x, event.mouseButton.y);
                }

                if (event.type == sf::Event::MouseButtonReleased) {
                    if (event.mouseButton.button == sf::Mouse::Button::Left) {
                        isMouseLeftDown = false;
                    }
                }

                if (event.type == sf::Event::MouseMoved) {
                    mousePosition = sf::Vector2u(event.mouseMove.x, event.mouseMove.y);
                }
            }
        }

        if (isMouseLeftDown) {
            const Point mouse = Point(mousePosition.x, mousePosition.y);
            setRectangle(world, mouse, worldSize, settings);
        }

        const sf::Time startedStepping = worldStepClock.getElapsedTime();
        const sf::Time stopStepping = (startedStepping + sf::milliseconds(15));
        for (;;) {
            const sf::Time elapsed = worldStepClock.getElapsedTime();
            if (elapsed >= stopStepping) {
                break;
            }
            if (elapsed < nextWorldStep) {
                break;
            }

            const std::chrono::time_point start = std::chrono::high_resolution_clock::now();
            if (!settings.isPaused) {
                auto [cellsChanged, newWorld] = simulateStep(world);
                profiling.cellsChanged = cellsChanged;
                world = std::move(newWorld);
            }
            const std::chrono::time_point stop = std::chrono::high_resolution_clock::now();
            profiling.simulationTime = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

            profiling.nonEmptyCells = world.Cells.size() - world.getEmptyCells();
            nextWorldStep += sf::milliseconds(settings.timeBetweenStepsInMilliseconds);
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        const std::chrono::time_point start = std::chrono::high_resolution_clock::now();
        renderUI(world, settings, profiling, isDemoVisible);

        window.clear();

        renderWorld(worldImage, world.Cells.front(), worldSize);

        sf::Texture worldTexture;
        if (!worldTexture.loadFromImage(worldImage)) {
            throw std::runtime_error("Could not load image into texture");
        }

        sf::Sprite worldSprite(worldTexture);
        window.draw(worldSprite);

        ImGui::SFML::Render(window);
        window.display();
        const std::chrono::time_point stop = std::chrono::high_resolution_clock::now();
        profiling.renderTime = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    }

    ImGui::SFML::Shutdown();
}

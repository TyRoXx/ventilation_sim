#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "simulation.hpp"

TEST_CASE("empty world")
{
    Cell dummyWorld;
    const std::vector<Cell> result = simulateStep(dummyWorld, sf::Vector2u(0, 0));
    const std::vector<Cell> expected;
    REQUIRE(expected == result);
}

TEST_CASE("simplest non-empty world")
{
    std::vector<Cell> world = { Cell::Air };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 1));
    const std::vector<Cell> expected = { Cell::Air };
    REQUIRE(expected == result);
}

TEST_CASE("wall does not fall")
{
    std::vector<Cell> world = { Cell::Wall, Cell::Air };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 2));
    const std::vector<Cell> expected = { Cell::Wall, Cell::Air };
    REQUIRE(expected == result);
}

TEST_CASE("falling materials")
{
    const Cell material = GENERATE(Cell::Snow, Cell::Water);
    std::vector<Cell> world = { material, Cell::Air };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 2));
    const std::vector<Cell> expected = { Cell::Air, material };
    REQUIRE(expected == result);
}

TEST_CASE("falling materials leave gaps")
{
    const Cell material = GENERATE(Cell::Snow, Cell::Water);
    std::vector<Cell> world = { material, material, Cell::Air };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 3));
    const std::vector<Cell> expected = { material, Cell::Air, material };
    REQUIRE(expected == result);
}

TEST_CASE("materials collect at the bottom")
{
    const Cell material = GENERATE(Cell::Snow, Cell::Water);
    std::vector<Cell> world = { material };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 1));
    const std::vector<Cell> expected = { material };
    REQUIRE(expected == result);
}

TEST_CASE("wall stops falling materials")
{
    const Cell material = GENERATE(Cell::Snow, Cell::Water);
    std::vector<Cell> world = { material, Cell::Wall };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 2));
    const std::vector<Cell> expected = { material, Cell::Wall };
    REQUIRE(expected == result);
}

TEST_CASE("snow stops snow")
{
    std::vector<Cell> world = { Cell::Snow, Cell::Snow };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(1, 2));
    const std::vector<Cell> expected = { Cell::Snow, Cell::Snow };
    REQUIRE(expected == result);
}

TEST_CASE("water goes sideways")
{
    std::vector<Cell> world = {
        Cell::Water, Cell::Air,
        // below:
        Cell::Water, Cell::Air
    };
    const std::vector<Cell> result = simulateStep(world.front(), sf::Vector2u(2, 2));
    const std::vector<Cell> expected = {
        Cell::Air, Cell::Air,
        // below:
        Cell::Water, Cell::Water
    };
    REQUIRE(expected == result);
}

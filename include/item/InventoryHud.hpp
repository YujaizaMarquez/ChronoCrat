#pragma once

#include "item/Inventory.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

void drawInventoryHud(
    sf::RenderWindow& window,
    const Inventory& inventory,
    std::size_t selectedSlot);

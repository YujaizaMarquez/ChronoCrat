#include "item/InventoryHud.hpp"
#include "GameConfig.hpp"
#include "item/Item.hpp"
#include <algorithm>
#include <array>

namespace
{

void drawDigit(
    sf::RenderWindow& window,
    unsigned int digit,
    const sf::Vector2f& position,
    float scale,
    const sf::Color& color)
{
    constexpr std::array<unsigned char, 10> DigitSegments{
        0b1111110, 0b0110000, 0b1101101, 0b1111001, 0b0110011,
        0b1011011, 0b1011111, 0b1110000, 0b1111111, 0b1111011
    };
    constexpr std::array<sf::FloatRect, 7> SegmentBounds{
        sf::FloatRect({3.f, 0.f}, {8.f, GameConfig::InventoryHud::DigitThickness}),
        sf::FloatRect({11.f, 2.f}, {GameConfig::InventoryHud::DigitThickness, 7.f}),
        sf::FloatRect({11.f, 11.f}, {GameConfig::InventoryHud::DigitThickness, 7.f}),
        sf::FloatRect({3.f, 18.f}, {8.f, GameConfig::InventoryHud::DigitThickness}),
        sf::FloatRect({0.f, 11.f}, {GameConfig::InventoryHud::DigitThickness, 7.f}),
        sf::FloatRect({0.f, 2.f}, {GameConfig::InventoryHud::DigitThickness, 7.f}),
        sf::FloatRect({3.f, 9.f}, {8.f, GameConfig::InventoryHud::DigitThickness})
    };

    if (digit >= DigitSegments.size())
    {
        return;
    }

    const unsigned char mask = DigitSegments[digit];
    for (std::size_t segmentIndex = 0;
         segmentIndex < SegmentBounds.size();
         ++segmentIndex)
    {
        const unsigned char bit =
            static_cast<unsigned char>(1U << (6U - segmentIndex));
        if ((mask & bit) == 0)
        {
            continue;
        }

        const sf::FloatRect& segment = SegmentBounds[segmentIndex];
        sf::RectangleShape shape({
            segment.size.x * scale,
            segment.size.y * scale
        });
        shape.setPosition({
            position.x + segment.position.x * scale,
            position.y + segment.position.y * scale
        });
        shape.setFillColor(color);
        window.draw(shape);
    }
}

}

void drawInventoryHud(
    sf::RenderWindow& window,
    const Inventory& inventory,
    std::size_t selectedSlot)
{
    const sf::Vector2u windowSize = window.getSize();
    const float width = static_cast<float>(windowSize.x);
    const float height = static_cast<float>(windowSize.y);
    constexpr float slotSize = GameConfig::InventoryHud::SlotSize;
    constexpr float slotGap = GameConfig::InventoryHud::SlotGap;
    constexpr float bottomMargin = GameConfig::InventoryHud::BottomMargin;
    const float unscaledWidth =
        Inventory::Capacity * slotSize +
        (Inventory::Capacity - 1) * slotGap;
    const float scale = std::min(1.f, width / (unscaledWidth + 24.f));
    const float totalWidth = unscaledWidth * scale;
    const float startX = (width - totalWidth) / 2.f;
    const float top = height - (slotSize + bottomMargin) * scale;

    for (std::size_t index = 0; index < Inventory::Capacity; ++index)
    {
        const float left =
            startX + index * (slotSize + slotGap) * scale;
        const bool selected = index == selectedSlot;

        sf::RectangleShape slot({slotSize * scale, slotSize * scale});
        slot.setPosition({left, top});
        slot.setFillColor(
            selected ? GameConfig::InventoryHud::SelectedBackground
                     : GameConfig::InventoryHud::SlotBackground);
        slot.setOutlineColor(
            selected ? GameConfig::InventoryHud::SelectedOutline
                     : GameConfig::InventoryHud::SlotOutline);
        slot.setOutlineThickness((selected ? 3.f : 2.f) * scale);
        window.draw(slot);

        drawDigit(
            window,
            static_cast<unsigned int>(index + 1),
            {left + 7.f * scale, top + 5.f * scale},
            scale,
            sf::Color::White);

        const Item* item = inventory.get(index);
        if (item != nullptr)
        {
            item->drawInventoryIcon(
                window,
                sf::FloatRect(
                    {left + 18.f * scale, top + 22.f * scale},
                    {42.f * scale, 42.f * scale}));
        }
    }
}

#pragma once

#include "item/Item.hpp"
#include <array.hpp>
#include <cstddef>
#include <memory>

class Inventory
{
public:
    static constexpr std::size_t Capacity = 5;

    Inventory();
    Inventory(const Inventory&) = delete;
    Inventory& operator=(const Inventory&) = delete;

    bool addItem(std::unique_ptr<Item>&& item);
    std::unique_ptr<Item> removeAt(std::size_t index);

    Item* get(std::size_t index);
    const Item* get(std::size_t index) const;
    Item* findFirst(ItemType type);
    const Item* findFirst(ItemType type) const;
    bool contains(ItemType type) const;

    std::size_t size() const;
    std::size_t capacity() const;
    bool hasSpace() const;
    bool empty() const;
    void clear();

private:
    Designar::FixedArray<std::unique_ptr<Item>> items;
    std::size_t itemCount = 0;
};

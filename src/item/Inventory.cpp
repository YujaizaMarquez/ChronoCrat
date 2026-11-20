#include "item/Inventory.hpp"
#include <utility>

Inventory::Inventory()
    : items(Capacity)
{
}

bool Inventory::addItem(std::unique_ptr<Item>&& item)
{
    if (!item)
    {
        return false;
    }

    for (std::size_t index = 0; index < Capacity; ++index)
    {
        if (!items[index])
        {
            items[index] = std::move(item);
            ++itemCount;
            return true;
        }
    }

    return false;
}

std::unique_ptr<Item> Inventory::removeAt(std::size_t index)
{
    if (index >= Capacity || !items[index])
    {
        return nullptr;
    }

    std::unique_ptr<Item> item = std::move(items[index]);
    --itemCount;
    return item;
}

Item* Inventory::get(std::size_t index)
{
    return index < Capacity ? items[index].get() : nullptr;
}

const Item* Inventory::get(std::size_t index) const
{
    return index < Capacity ? items[index].get() : nullptr;
}

Item* Inventory::findFirst(ItemType type)
{
    for (std::size_t index = 0; index < Capacity; ++index)
    {
        const std::unique_ptr<Item>& item = items[index];
        if (item && item->getType() == type)
        {
            return item.get();
        }
    }

    return nullptr;
}

const Item* Inventory::findFirst(ItemType type) const
{
    for (std::size_t index = 0; index < Capacity; ++index)
    {
        const std::unique_ptr<Item>& item = items[index];
        if (item && item->getType() == type)
        {
            return item.get();
        }
    }

    return nullptr;
}

bool Inventory::contains(ItemType type) const
{
    return findFirst(type) != nullptr;
}

std::size_t Inventory::size() const
{
    return itemCount;
}

std::size_t Inventory::capacity() const
{
    return Capacity;
}

bool Inventory::hasSpace() const
{
    return itemCount < Capacity;
}

bool Inventory::empty() const
{
    return itemCount == 0;
}

void Inventory::clear()
{
    for (std::size_t index = 0; index < Capacity; ++index)
    {
        items[index].reset();
    }
    itemCount = 0;
}

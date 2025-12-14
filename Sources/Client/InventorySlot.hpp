#pragma once

#include <optional>

#include "Item.hpp"

struct InventorySlot {
    int index;
    std::optional<Item> item;
};
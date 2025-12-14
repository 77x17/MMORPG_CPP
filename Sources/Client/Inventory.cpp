#include "Inventory.hpp"

Inventory::Inventory(int size) {
    slots.reserve(size);
    for (int i = 0; i < size; ++i) {
        slots.push_back({ i, std::nullopt });
    }
}
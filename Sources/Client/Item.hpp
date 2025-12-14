#pragma once

#include <string>

struct Item {
    int id;             // unique id
    int typeId;         // unique id for type (wand: 0, sword: 1, ...)
    std::string name;      
};
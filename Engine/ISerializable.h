#pragma once
#include "json.hpp"

class ISerializable {
public:
    virtual ~ISerializable() = default;
    virtual json::JSON Serialize() const = 0;
};
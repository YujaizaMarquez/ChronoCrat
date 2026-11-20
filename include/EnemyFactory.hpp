#pragma once

#include "Enemy.hpp"
#include <array.hpp>
#include <cstddef>
#include <functional>
#include <memory>
#include <string>

class EnemyFactory
{
public:
    using Creator = std::function<std::unique_ptr<Enemy>()>;

    EnemyFactory();

    void registerEnemyType(const std::string& type, Creator creator);
    Designar::DynArray<std::unique_ptr<Enemy>> createEnemies(
        const std::string& type,
        std::size_t count) const;

private:
    struct RegisteredType
    {
        std::string type;
        Creator creator;
    };

    Designar::DynArray<RegisteredType> creators;
};

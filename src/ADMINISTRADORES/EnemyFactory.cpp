#include "EnemyFactory.hpp"
#include "Serpent.hpp"
#include "levels/Level_present/Scientist.hpp"
#include "Soldier.hpp"
#include "Robot.hpp"
#include <stdexcept>
#include <utility>

EnemyFactory::EnemyFactory()
{
    registerEnemyType("soldier", [] {
        return std::make_unique<Soldier>();
    });

    registerEnemyType("cientifico1", [] {
        return std::make_unique<Scientist>();
    });

    registerEnemyType("serpiente_murcielago", [] {
        return std::make_unique<Serpent>();
    });

    registerEnemyType("robot", [] {
        return std::make_unique<Robot>();
    });
}

void EnemyFactory::registerEnemyType(const std::string& type, Creator creator)
{
    if (type.empty() || !creator)
    {
        throw std::invalid_argument("El tipo y el creador del enemigo son obligatorios");
    }

    for (std::size_t index = 0; index < creators.size(); ++index)
    {
        if (creators[index].type == type)
        {
            throw std::logic_error("El tipo de enemigo ya está registrado: " + type);
        }
    }

    creators.append({type, std::move(creator)});
}

Designar::DynArray<std::unique_ptr<Enemy>> EnemyFactory::createEnemies(
    const std::string& type,
    std::size_t count) const
{
    const Creator* creator = nullptr;
    for (std::size_t index = 0; index < creators.size(); ++index)
    {
        if (creators[index].type == type)
        {
            creator = &creators[index].creator;
            break;
        }
    }

    if (creator == nullptr)
    {
        throw std::invalid_argument("Tipo de enemigo no registrado: " + type);
    }

    Designar::DynArray<std::unique_ptr<Enemy>> enemies;
    enemies.reserve(count);
    for (std::size_t index = 0; index < count; ++index)
    {
        std::unique_ptr<Enemy> enemy = (*creator)();
        if (!enemy)
        {
            throw std::logic_error("El creador devolvió un enemigo nulo: " + type);
        }
        enemies.append(std::move(enemy));
    }

    return enemies;
}

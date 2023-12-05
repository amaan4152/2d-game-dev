#ifndef WORLD_H
#define WORLD_H

#include <memory>
#include <vector>
#include <Entity.h>

using entity_sptr = std::shared_ptr<Entity>;
using entity_collection = std::vector<entity_sptr>;

class World
{
public:
    float ground_y;

    World() = default;

    World(float gravity);

    void operator>>(entity_sptr &entity);

    void update(float dt);

    void init();

private:
    entity_collection EntityCollection;

    float gravity;
};

#endif
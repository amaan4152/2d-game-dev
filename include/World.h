#ifndef WORLD_H
#define WORLD_H

#include <memory>
#include <vector>
#include "Entity.h"

template <typename T>
using uptr = std::unique_ptr<T>;
template <typename T>
using collection = std::vector<uptr<T>>;

class World
{
public:
    float ground_y;

    World() = default;

    World(float gravity);

    void operator>>(RigidBody::Entity* entity);

    void update(float dt);

    void init();

private:
    collection<RigidBody::Entity> EntityCollection;

    float gravity;
};

#endif
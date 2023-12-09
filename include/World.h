#ifndef WORLD_H
#define WORLD_H

#include <memory>
#include <vector>
#include "DynamicRB.h"
#include "Entity.h"

class World
{
public:
    float ground_y;

    World() = default;

    World(float gravity);

    template <class T>
    void operator>>(sptr<T> &entity)
    {
        bodyType type = entity->getType();
        if(type == DYNAMIC)
            this->EntityCollection.push_back(entity);
        else if(type == STATIC)
            this->EntityCollection.push_back(entity);
    }

    void update(float dt);

    void draw(sf::RenderTarget &window);

    void init();

private:
    collection<RigidBody::Entity> EntityCollection;
    state playerState;
    orientation playerOrient;
    float gravity;

    void playerController(sptr<RigidBody::Dynamic> &Player, float dt);
};

#endif
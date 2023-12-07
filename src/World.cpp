#include "DynamicRB.h"
#include "StaticRB.h"
#include "World.h"
#include <iostream>

#define WINDOW_WIDTH 1900
#define WINDOW_HEIGHT 900

const float GRAVITY = -250.f;

World::World(float gravity) : gravity{gravity}
{
    this->ground_y = WINDOW_HEIGHT;
}

void World::operator>>(RigidBody::Entity* entity)
{
    bodyType type = entity->getType();
    if(type == DYNAMIC)
        this->EntityCollection.push_back(std::make_unique<RigidBody::Dynamic>(entity));
    else if(type == STATIC)
        this->EntityCollection.push_back(std::make_unique<RigidBody::Static>(entity));
}

void World::init()
{
    // sample spawn point; each entity needs to have its own initialization vectors
    sf::Vector2f pos(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f), vel(75.f, 0.f), accel(0.f, GRAVITY);
    for (uptr<RigidBody::Entity> &entity : this->EntityCollection)
    {
        if (entity->getName() == "Player")
        {
            entity->grounded = false;
            entity->setState(true, pos, vel, accel);
        }
    }
}

void World::update(float dt)
{
    orientation gravity_dir = DOWN;
    sf::Vector2f pos, vel, accel;
    for (uptr<RigidBody::Entity> &entity : this->EntityCollection)
    {
        bodyType type = entity->getType();
        entity->grounded = false;
        entity->getState(pos, vel, accel);

        // in contact with ground!
        if (((pos.y + entity->getDims().y) > this->ground_y) && (vel.y <= 0.f))
        {
            entity->grounded = true;
            entity->setVel({vel.x, 0.f});
            continue;
        }
        if(type == DYNAMIC)
        {
            entity->update();
        }
    }
}


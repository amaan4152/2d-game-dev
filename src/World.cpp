#include "DynamicRB.h"
#include "StaticRB.h"
#include "World.h"
#include <iostream>
#include <SFML/Window/Keyboard.hpp>

#define WINDOW_WIDTH 1900
#define WINDOW_HEIGHT 900

const float GRAVITY = -250.f;

World::World(float gravity) : gravity{gravity}, playerState{IDLE}, playerOrient{RESET}
{
    this->ground_y = WINDOW_HEIGHT;
}

void World::init()
{
    // sample spawn point; each entity needs to have its own initialization vectors
    sf::Vector2f pos(WINDOW_WIDTH / 2.f, WINDOW_HEIGHT / 2.f), vel(75.f, 0.f), accel(0.f, GRAVITY);
    for (sptr<RigidBody::Entity> &entity : this->EntityCollection)
    {
        if (entity->getName() == "Player")
        {
            entity->grounded = false;
            entity->setState(true, pos, vel, accel);
        }
    }
}

void World::draw(sf::RenderTarget &window)
{
    for(sptr<RigidBody::Entity> &entity : this->EntityCollection)
        entity->draw(window);
}

void World::update(float dt)
{
    sf::Vector2f pos, vel, accel;
    for (sptr<RigidBody::Entity> &entity : this->EntityCollection)
    {
        bodyType type = entity->getType();
        std::string id = entity->getName();
        entity->grounded = false;
        entity->getState(pos, vel, accel);

        // in contact with ground!
        if (((pos.y + entity->getDims().y) > this->ground_y) && (vel.y <= 0.f))
        {
            entity->grounded = true;
            entity->setVel({vel.x, 0.f});
        }
        if(type == DYNAMIC)
        {
            sptr<RigidBody::Dynamic> dynamicEntity = std::static_pointer_cast<RigidBody::Dynamic>(entity);
            if(id == "Player")
                this->playerController(dynamicEntity, dt);
        }
    }
}

void World::playerController(sptr<RigidBody::Dynamic> &Player, float dt)
{
    if (Player->grounded && Player->jumped && Player->jumpCharge == -2)
    {
        Player->jumped = false;
        Player->jumpCharge = 3;
        this->playerState = IDLE;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || this->playerState == JUMP)
        this->playerState = JUMP;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        this->playerOrient = LEFT;
        if (Player->grounded && this->playerState != JUMP)
            this->playerState = WALK;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        this->playerOrient = RIGHT;
        if (Player->grounded && this->playerState != JUMP)
            this->playerState = WALK;
    }
    else if (Player->grounded && this->playerState != JUMP)
        this->playerState = IDLE;
    Player->update(this->playerState, this->playerOrient, dt);
}


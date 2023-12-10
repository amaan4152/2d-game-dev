#include "DynamicRB.h"
#include "StaticRB.h"
#include "Level.h"
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
    sf::Vector2f pos(WINDOW_WIDTH / 4.f, WINDOW_HEIGHT / 2.f), vel(90.f, 0.f), accel(0.f, GRAVITY);
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
    for (sptr<RigidBody::Entity> &entity : this->EntityCollection)
        entity->draw(window);
}

#define COLLISION_PAD 5.0f
collision_type World::detectCollision(sptr<RigidBody::Dynamic> &actor, levelObject &levelEntity)
{
    collision_type vertColl, horzColl, type;
    sf::Vector2f pos, vel, accel;
    actor->getState(pos, vel, accel);
    sf::FloatRect actorBBox = actor->getSprite()->getGlobalBounds();
    // actorBBox = sf::FloatRect(actorBBox.left - COLLISION_PAD, actorBBox.top - COLLISION_PAD, actorBBox.width + 2 * COLLISION_PAD, actorBBox.height + 2 * COLLISION_PAD);
    sf::FloatRect lvlObjBBox = levelEntity.self.getGlobalBounds();
    if (actorBBox.intersects(lvlObjBBox)) // check if collision occurs with bounding boxes
    {
        float deltaAbove = actorBBox.top + actorBBox.height - lvlObjBBox.top;
        float deltaBelow = lvlObjBBox.top + lvlObjBBox.height - actorBBox.top;
        float deltaFront = actorBBox.left + actorBBox.width - lvlObjBBox.left;
        float deltaBack = lvlObjBBox.left + lvlObjBBox.width - actorBBox.left;
        if ((actorBBox.top < lvlObjBBox.top) && (actorBBox.top + actorBBox.height) > lvlObjBBox.top) // collision under actor
            type = BOTTOM;
        else if ((actorBBox.top > lvlObjBBox.top) && (actorBBox.top) < (lvlObjBBox.top + lvlObjBBox.height)) // collision above actor
            type = ABOVE;
    }
    else
        type = NO_COLLISION;
    return type;
}

void World::update(Level &Stage, float dt)
{
    sf::Vector2f pos, vel, accel;
    for (sptr<RigidBody::Entity> &entity : this->EntityCollection)
    {
        bodyType type = entity->getType();
        std::string id = entity->getName();
        entity->grounded = false;
        entity->getState(pos, vel, accel);
        entity->setAccel({0.f, GRAVITY});

        if (type == DYNAMIC)
        {
            sptr<RigidBody::Dynamic> dynamicEntity = std::static_pointer_cast<RigidBody::Dynamic>(entity);
            if (id == "Player")
            {
                unsigned int levelSize = Stage.getLevelSize();

                // probably the most foulest approach to detecting collision due ineffecient configuration of level design :/
                // potential improvement: create a tiling construct to merge common tiled level objects into one vertex array entity -> generate texture -> regen as whole sprite
                for (unsigned int i = 0; i < levelSize; ++i)
                {
                    levelObject obj = Stage.getObject(i);
                    collision_type status = this->detectCollision(dynamicEntity, obj);
                    switch (status)
                    {
                    case NO_COLLISION:
                        break;
                    case BOTTOM:
                        dynamicEntity->grounded = true;
                        dynamicEntity->setAccel({0.f, 0.f});
                        dynamicEntity->setVel({vel.x, 0.f});
                        break;
                    case ABOVE:
                        dynamicEntity->setVel({vel.x, 0.f});
                        break;
                    }
                }
                this->playerController(dynamicEntity, dt);
                dynamicEntity->update(this->playerState, this->playerOrient, dt);
            }
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
}

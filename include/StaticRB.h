#ifndef STATIC_H
#define STATIC_H

#include "Entity.h"

namespace RigidBody
{
    class Static : public Entity
    {
    public:
        Static() = default;
        Static(std::string name, sf::Texture &texture, sf::Vector2i &udims, sf::Vector2f &scale);

        void update(sf::Vector2f pos, sf::Vector2f vel, sf::Vector2f accel, float dt);

    };
}
#endif
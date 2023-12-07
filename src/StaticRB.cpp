#include "StaticRB.h"

RigidBody::Static::Static(std::string name, sf::Texture &texture, sf::Vector2i &udims, sf::Vector2f &scale) : Entity{STATIC, name, texture, udims, scale}
{}

void RigidBody::Static::update(sf::Vector2f pos, sf::Vector2f vel, sf::Vector2f accel, float dt)
{
    sf::Vector2f offset{0.f, 0.f};
    this->setState(false, pos, vel, accel);
    this->vel += this->accel * dt;
    offset = this->vel * dt;
    
    this->pos += offset;
    this->self.move(offset);
}
#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "Entity.h"

RigidBody::Entity::Entity(bodyType type, std::string name, sf::Texture &texture, sf::Vector2i udims, sf::Vector2f &scale)
    : type{type}, id{name}, self{texture}, udims{udims}, t{0.f}
{
    // assumes sprite dimensions is set to full texture dimensions
    this->dims = this->self.getTexture()->getSize();
    this->self.setTextureRect({{0, 0}, {udims.x, udims.y}});
    this->self.setScale(scale);
}

void RigidBody::Entity::draw(sf::RenderTarget &target)
{
    target.draw(this->self);
}

void RigidBody::Entity::setState(bool apply, const sf::Vector2f &pos, const sf::Vector2f &vel, const sf::Vector2f &accel)
{
    if(apply)
        this->self.setPosition(pos);
    this->pos = pos;
    this->vel = vel;
    this->accel = accel;
}

void RigidBody::Entity::setPos(const sf::Vector2f pos)
{
    this->pos = pos;
}

void RigidBody::Entity::setVel(const sf::Vector2f vel)
{
    this->vel = vel;
}

void RigidBody::Entity::setAccel(const sf::Vector2f accel)
{
    this->accel = accel;
}

std::string RigidBody::Entity::getName()
{
    return this->id;
}

std::unique_ptr<sf::Sprite> RigidBody::Entity::getSprite()
{
    return std::make_unique<sf::Sprite>(this->self);
}

sf::Vector2f RigidBody::Entity::getDims()
{
    sf::Vector2f scale = this->self.getScale();
    return {scale.x * this->self.getLocalBounds().width, scale.y * this->self.getLocalBounds().height};
}

void RigidBody::Entity::getState(sf::Vector2f &pos, sf::Vector2f &vel, sf::Vector2f &accel)
{
    pos = this->pos;
    vel = this->vel;
    accel = this->accel;
}

bodyType RigidBody::Entity::getType()
{
    return this->type;
}
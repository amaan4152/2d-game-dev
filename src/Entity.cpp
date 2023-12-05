#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <Entity.h>

Entity::Entity(std::string name, sf::Texture &texture, sf::Vector2i udims, sf::Vector2f &scale)
    : id{name}, self{texture}, udims{udims}, faceForward{true}, jumped{false}, t{0.f}, spriteBox_ul{0, 0}, spriteID{0}
{
    // assumes sprite dimensions is set to full texture dimensions
    this->dims = this->self.getTexture()->getSize();
    this->self.setTextureRect({{0, 0}, {udims.x, udims.y}});
    this->self.setScale(scale);
}

void Entity::setState(sf::Vector2f pos, sf::Vector2f vel, sf::Vector2f accel, bool initial)
{

    if (initial)
        this->self.setPosition(pos);
    this->pos = pos;
    this->vel = vel;
    this->accel = accel;
}

void Entity::setVel(sf::Vector2f vel)
{
    this->vel = vel;
}

void Entity::setAccel(sf::Vector2f accel)
{
    this->accel = accel;
}

void Entity::draw(sf::RenderTarget &target)
{
    target.draw(this->self);
}

void Entity::animate(state action, sf::Vector2i window, sf::Vector2i padding, float dt)
{
    // assumes horizontal sprite sheet for entity
    this->t += dt;
    int start, end;
    float x_dt_bin, y_dt_bin;
    float _x_dt = (1. / this->vel.x) / 5;
    float _y_dt = -2 * this->MAX_Y_VELOCITY / (3 * this->accel.y); // each jump sprite triggers at evenly distributed accumulated dts throughout 1 jump cycle
    float t_anim;
    sf::Clock clk_anim;
    switch (action)
    {
    case IDLE:
        if (this->t > 0.2f)
        {
            this->updateAnimation(IDLE, IDLE + 3, window, padding);
            this->t = 0.f;
        }
        break;
    case WALK:
        if (this->t > 0.175f)
        {
            this->updateAnimation(WALK, WALK + 3, window, padding);
            this->t = 0.f;
        }
        break;
    case JUMP:
#define JUMP_CHARGE_DT 0.2
        y_dt_bin = (this->jumpCharge > 0 || this->jumpCharge < 0) ? JUMP_CHARGE_DT : _y_dt;
        if (this->t > y_dt_bin)
        {
            if (this->jumpCharge > 0 || (this->spriteID > 13 && this->jumpCharge > -2))
                --this->jumpCharge;
            this->updateAnimation(JUMP, JUMP + 6, window, padding);
            this->t = 0.f;
        }
    default:
        break;
    }
}

inline void Entity::updateAnimation(int start, int end, sf::Vector2i &window, sf::Vector2i &padding)
{
    if ((this->spriteID < start) || this->spriteID > end) // wrap around specific animation for continuous looping effect
    {
        this->spriteBox_ul = {start * window.x, 0};
        this->spriteID = start;
    }
    sf::IntRect spriteBox = {this->spriteBox_ul, window}; // get next sprite
    this->self.setTextureRect(spriteBox);                 // update sprite
    this->flip();                                         // flip based on sprite's forward vector
    this->spriteBox_ul.x += window.x + (2 * padding.x);   // acount for padding for next sprite block
    ++this->spriteID;                                     // update sprite ID
}

void Entity::move(orientation dir, float dt)
{
    sf::Vector2f offset(0.f, 0.f);

    // flip depending on horizontal dir
    // if ((!this->faceForward && dir == RIGHT) || (this->faceForward && dir == LEFT))
    //     this->flip();

    // move based on direction
    if (dir == RIGHT && (this->faceForward = true))
        offset.x = this->vel.x * dt;
    if (dir == LEFT && !(this->faceForward = false))
        offset.x = -1 * this->vel.x * dt;
    if (dir == DOWN) // for gravitational effect
    {
        this->vel.y += this->accel.y * dt;
        offset.y = -1 * this->vel.y * dt;
    }

    // apply offset
    this->pos += offset;
    this->self.move(offset);
}

void Entity::jump()
{
    if (!this->jumped && this->jumpCharge == 0)
    {
        this->jumped = true;
        this->vel.y = MAX_Y_VELOCITY;
    }
}

std::string Entity::getName()
{
    return this->id;
}

std::unique_ptr<sf::Sprite> Entity::getSprite()
{
    return std::make_unique<sf::Sprite>(this->self);
}

void Entity::getState(sf::Vector2f &pos, sf::Vector2f &vel, sf::Vector2f &accel)
{
    pos = this->pos;
    vel = this->vel;
    accel = this->accel;
}

sf::Vector2f Entity::getDims()
{
    sf::Vector2f scale = this->self.getScale();
    return {scale.x * this->self.getLocalBounds().width, scale.y * this->self.getLocalBounds().height};
}

void Entity::flip()
{
    sf::Vector2i bounds = {(int)this->self.getLocalBounds().width, (int)this->self.getLocalBounds().height};
    if (!this->faceForward)
        this->self.setTextureRect({{bounds.x + this->spriteBox_ul.x, 0}, {-1 * bounds.x, bounds.y}});
    else
        this->self.setTextureRect({this->spriteBox_ul, {bounds.x, bounds.y}});
}

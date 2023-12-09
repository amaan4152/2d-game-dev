#include <SFML/System/Clock.hpp>
#include "DynamicRB.h"

RigidBody::Dynamic::Dynamic(std::string name, sf::Texture &texture, sf::Vector2i spriteWindow, sf::Vector2i padding, sf::Vector2i &udims, sf::Vector2f &scale) : Entity{DYNAMIC, name, texture, udims, scale}, spriteWindow{spriteWindow}, padding{padding}
{}

void RigidBody::Dynamic::update(state &action, orientation &dir, float dt)
{
    if(action == JUMP) 
        this->jump();
    this->move(dir, dt);
    this->animate(action, dt);
    dir = RESET;
}

void RigidBody::Dynamic::animate(state action, float dt)
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
            this->updateAnimation(IDLE, IDLE + 3);
            this->t = 0.f;
        }
        break;
    case WALK:
        if (this->t > 0.175f)
        {
            this->updateAnimation(WALK, WALK + 3);
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
            this->updateAnimation(JUMP, JUMP + 6);
            this->t = 0.f;
        }
    default:
        break;
    }
}

inline void RigidBody::Dynamic::updateAnimation(int start, int end)
{
    if ((this->spriteID < start) || this->spriteID > end) // wrap around specific animation for continuous looping effect
    {
        this->spriteBox_ul = {start * this->spriteWindow.x, 0};
        this->spriteID = start;
    }
    sf::IntRect spriteBox = {this->spriteBox_ul, this->spriteWindow}; // get next sprite
    this->self.setTextureRect(spriteBox);                 // update sprite
    this->flip();                                         // flip based on sprite's forward vector
    this->spriteBox_ul.x += this->spriteWindow.x + (2 * this->padding.x);   // acount for padding for next sprite block
    ++this->spriteID;                                     // update sprite ID
}

void RigidBody::Dynamic::move(orientation dir, float dt) 
{
    sf::Vector2f offset(0.f, 0.f);
    
    // move based on direction
    if (dir == RIGHT && (this->faceForward = true))
        offset.x = this->vel.x * dt;
    if (dir == LEFT && !(this->faceForward = false))
        offset.x = -1 * this->vel.x * dt;

    this->vel.y += this->accel.y * dt;
    offset.y = -1 * this->vel.y * dt;

    // apply offset
    this->pos += offset;
    this->self.move(offset);
}

void RigidBody::Dynamic::jump()
{
    if (!this->jumped && this->jumpCharge == 0)
    {
        this->jumped = true;
        this->vel.y = MAX_Y_VELOCITY;
    }
}

void RigidBody::Dynamic::flip()
{
    sf::Vector2i bounds = {(int)this->self.getLocalBounds().width, (int)this->self.getLocalBounds().height};
    if (!this->faceForward)
        this->self.setTextureRect({{bounds.x + this->spriteBox_ul.x, 0}, {-1 * bounds.x, bounds.y}});
    else
        this->self.setTextureRect({this->spriteBox_ul, {bounds.x, bounds.y}});
}

#ifndef DYNAMIC_H
#define DYNAMIC_H

#include "Entity.h"



namespace RigidBody
{
    class Dynamic : public Entity
    {
    public:
        float MAX_Y_VELOCITY = 300.f;
        int jumpCharge = 3;

        bool jumped;
        bool faceForward;

    public:
        Dynamic() = default;
        Dynamic(std::string name, sf::Texture &texture, sf::Vector2i spriteWindow, sf::Vector2i padding, sf::Vector2i &udims, sf::Vector2f &scale);
        
        // --- update body --- //
        void update(state &action, orientation &dir, float dt);

        // --- actions --- //
        void animate(state action, float dt);
        void updateAnimation(int start, int end);
        void move(orientation dir, float dt);
        void jump();
        void flip();

    protected:
        unsigned int spriteID;
        sf::Vector2i spriteBox_ul;
        sf::Vector2i spriteWindow;
        sf::Vector2i padding;
    };
}

#endif
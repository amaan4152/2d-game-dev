#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>

enum orientation
{
    RESET,
    RIGHT,
    LEFT,
    UP,
    DOWN
};

enum state
{
    IDLE = 0,
    WALK = 5,
    JUMP = 9
};

class Entity
{
public:
    float MAX_Y_VELOCITY = 200.f;

    int jumpCharge = 3;
    bool jumped;
    bool grounded;
    bool faceForward;

    // --- constructors --- //
    Entity() = default;
    Entity(std::string name, sf::Texture &texture, sf::Vector2i udims, sf::Vector2f &scale);

    // --- actions --- //
    void draw(sf::RenderTarget &target);
    void animate(state action, sf::Vector2i window, sf::Vector2i padding, float dt);
    void move(orientation dir, float dt);
    void jump();
    void flip();

    // --- setters --- //
    void setState(sf::Vector2f pos, sf::Vector2f vel, sf::Vector2f accel, bool initial = true);
    void setVel(sf::Vector2f vel);
    void setAccel(sf::Vector2f accel);

    // --- getters --- //
    std::string getName();
    std::unique_ptr<sf::Sprite> getSprite();
    void getState(sf::Vector2f &pos, sf::Vector2f &vel, sf::Vector2f &accel);
    sf::Vector2f getDims();

protected:
    // entity name
    std::string id;

    // entity sprite
    sf::Sprite self;

    // entity states
    sf::Vector2f pos;
    sf::Vector2f vel;
    sf::Vector2f accel;

    // entity shape
    sf::Vector2u dims;
    sf::Vector2i udims;

    // entity animation state
    unsigned int spriteID;
    sf::Vector2i spriteBox_ul;

    // time
    float t;

    inline void updateAnimation(int start, int end, sf::Vector2i &window, sf::Vector2i &padding);
};

#endif
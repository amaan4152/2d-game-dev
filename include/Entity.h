#ifndef PLAYER_H
#define PLAYER_H

#include <memory>
#include <string>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Sprite.hpp>

template <typename T>
using sptr = std::shared_ptr<T>;
template <typename T>
using wptr = std::weak_ptr<T>;
template <typename T>
using collection = std::vector<sptr<T>>;

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

enum bodyType
{
    STATIC,
    DYNAMIC
};

namespace RigidBody
{
    class Entity
    {
    public:
        bool grounded;

    public:
        // --- constructors --- //
        Entity() = default;
        Entity(bodyType type, std::string name, sf::Texture &texture, sf::Vector2i udims, sf::Vector2f &scale);
        virtual ~Entity();

        // --- draw --- //
        void draw(sf::RenderTarget &target);

        // --- setters --- //
        void setState(bool apply, const sf::Vector2f &pos, const sf::Vector2f &vel, const sf::Vector2f &accel);
        void setPos(const sf::Vector2f pos);
        void setVel(const sf::Vector2f vel);
        void setAccel(const sf::Vector2f accel);

        // --- getters --- //
        std::string getName();
        sptr<sf::Sprite> getSprite();
        sf::Vector2f getDims();
        void getState(sf::Vector2f &pos, sf::Vector2f &vel, sf::Vector2f &accel);
        bodyType getType();

    protected:
        // entity name
        std::string id;
        bodyType type;

        // entity sprite
        sf::Sprite self;

        // entity shape
        sf::Vector2u dims;
        sf::Vector2i udims;

        // state variables
        sf::Vector2f pos;
        sf::Vector2f vel;
        sf::Vector2f accel;

        // time
        float t;
    };
}
#endif
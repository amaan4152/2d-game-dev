#include <cmath>
#include <iostream>
#include <Level.h>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics.hpp>

#define SCALE_FACTOR 0.25

Level::Level(std::string name, sf::Texture &texture, sf::Vector2f &gridConfig, std::vector<sf::Vector2i> &posConfig, std::vector<sf::Vector2i> &sizeConfig) : id{name}, objDragID{-1}, mouseHeld{false}, gridConfig{gridConfig}, posConfig{posConfig}, sizeConfig{sizeConfig}
{
    this->getLevelObjects(texture, posConfig, sizeConfig);
}

void Level::init(sf::RenderTarget &window)
{
    sf::Vector2f pos{0.f, 0.f};
    for (sprite_uptr &obj : this->objects)
    {
        obj->setPosition(pos);
        pos = {obj->getGlobalBounds().width, 0};
    }
}

void Level::draw(sf::RenderTarget &window)
{
    if (this->objects.empty())
    {
        std::cerr << "[ERROR]: Level has not been initialized!" << std::endl;
        exit(1);
    }
    for (sprite_uptr &obj : this->objects)
        window.draw(*obj);
}

void Level::editor(bool dev, EDITOR_STATE &state, std::string levelStateFile, sf::RenderWindow &window, sf::Event &event, float dt)
{
    this->t += dt;
    if (dev)
    {
        if (this->objDragID != -1) // if an object is draggable, lock onto object
        {
            float tElapsed = this->tHold - this->t;
            sprite_uptr &obj = this->objects[this->objDragID]; // get draggable object
            sf::Vector2i objSize = this->sizeConfig[this->objDragID];
            sprite_uptr objDup = nullptr;
            sf::Vector2f scale;

            // update mouse data
            sf::Vector2f mPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            sf::Vector2f delMousePos = mPos - this->mousePos;
            this->mouseDelta += delMousePos;
            sf::Vector2i dir = {(delMousePos.x > 0) - (delMousePos.x < 0), (delMousePos.y > 0) - (delMousePos.y < 0)};
            switch (state)
            {
            case SELECTED:
                this->tHold = this->t;
                // if (std::fabs(this->mouseDelta.x) >= this->gridConfig.x || std::fabs(this->mouseDelta.y) >= this->gridConfig.y)
                if (event.type == sf::Event::KeyPressed)
                {
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                    {
                        // std::cout << "g.x = " << this->gridConfig.x << " | m.x = " << this->mouseDelta.x << std::endl;
                        // this->mouseDelta = {0.f, 0.f};
                        // this->mousePos = mPos;
                        obj->move(this->gridConfig.x * 2, 0.f);
                        // obj->setPosition(mPos);
                    }
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                        obj->move(-1 * this->gridConfig.x * 2, 0.f);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                        obj->move(0.f, -1 * this->gridConfig.y * 2);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                        obj->move(0.f, this->gridConfig.y * 2);
                }
                break;
            case DUPLICATE:
                std::cout << "\t DUP" << std::endl;
                objDup = std::make_unique<sf::Sprite>(*obj);
                this->objects.push_back(std::move(objDup));
                this->sizeConfig.push_back(this->sizeConfig[this->objDragID]);
                break;
            case UNSELECTED:
                std::cout << "UNSELECT" << std::endl;
                this->objDragID = -1;
                break;
            case ZOOM_IN:
                scale = {1 - SCALE_FACTOR, 1 - SCALE_FACTOR};
                obj->scale(sf::Vector2f(scale));
                this->sizeConfig[this->objDragID] = sf::Vector2i(obj->getGlobalBounds().width, obj->getGlobalBounds().height);
                break;
            case ZOOM_OUT:
                scale = {1 + SCALE_FACTOR, 1 + SCALE_FACTOR};
                obj->scale(sf::Vector2f(scale));
                this->sizeConfig[this->objDragID] = sf::Vector2i(obj->getGlobalBounds().width, obj->getGlobalBounds().height);
                break;
            case NONE:
                break;
            }
        }
        else // if no draggable object, unlock permission to select an object
        {
            int i = 0;
            for (sprite_uptr &obj : this->objects)
            {
                if (this->isClickable(obj, window))
                {
                    this->tHold = this->t;
                    this->objDragID = i;
                    this->mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    break; // can only drag one object
                }
                ++i;
            }
        }
    }
}

void Level::getLevelObjects(sf::Texture &texture, std::vector<sf::Vector2i> &posConfig, std::vector<sf::Vector2i> &sizeConfig)
{
    if (posConfig.size() != sizeConfig.size())
    {
        std::cerr << "[ERROR]: Position and size configurations must be the same length as the numer of sprites in level sheet!" << std::endl;
        exit(1);
    }
    for (int i = 0; i < posConfig.size(); ++i)
    {
        sf::Vector2i pos = posConfig[i];
        sf::Vector2i size = sizeConfig[i];
        sf::IntRect objSpriteBox(pos, size);
        sprite_uptr obj = std::make_unique<sf::Sprite>(sf::Sprite{texture, objSpriteBox});
        obj->setScale(3.f, 3.f);
        this->objects.push_back(std::move(obj));
    }
}

bool Level::isClickable(sprite_uptr &obj, sf::RenderWindow &window)
{
    sf::FloatRect objBBox = obj->getGlobalBounds();
    sf::Vector2f mouseLoc = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    return objBBox.contains(mouseLoc);
}
#ifndef LEVEL_H
#define LEVEL_H

#include <vector>
#include <Entity.h>
#include <SFML/Graphics/RenderWindow.hpp>

using sprite_uptr = std::unique_ptr<sf::Sprite>;

enum EDITOR_STATE
{
    NONE,
    SELECTED,
    UNSELECTED,
    ZOOM_IN,
    ZOOM_OUT,
    DUPLICATE
};

class Level
{
public:
    Level() = default;

    Level(std::string name, sf::Texture &texture, sf::Vector2f &gridConfig, std::vector<sf::Vector2i> &posConfig, std::vector<sf::Vector2i> &sizeConfig);

    void init(sf::RenderTarget &window);

    void draw(sf::RenderTarget &window);

    void editor(bool dev, EDITOR_STATE &state, std::string levelStateFile, sf::RenderWindow &window, sf::Event &event, float dt);

private:
    int objDragID;
    bool mouseHeld;
    std::string id;
    std::vector<sprite_uptr> objects;
    std::vector<sf::Vector2i> posConfig;
    std::vector<sf::Vector2i> sizeConfig;

    sf::Vector2f gridConfig;
    sf::Vector2f mousePos;
    sf::Vector2f mouseDelta;

    float t;
    float tHold;

    void getLevelObjects(sf::Texture &texture, std::vector<sf::Vector2i> &posConfig, std::vector<sf::Vector2i> &sizeConfig);

    bool isClickable(sprite_uptr &obj, sf::RenderWindow &window);
};

#endif
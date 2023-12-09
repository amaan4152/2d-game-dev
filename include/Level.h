#ifndef LEVEL_H
#define LEVEL_H

#include <string>
#include <vector>
#include <Entity.h>
#include <SFML/Graphics/RenderWindow.hpp>


enum editor_state
{
    NONE,
    SELECTED,
    UNSELECTED,
    DELETE,
    ZOOM_IN,
    ZOOM_OUT,
    DUPLICATE
};

enum mouse_state
{
    LMB_CLICKED,
    LMB_RELEASED,
    RMB_CLICKED,
    RMB_RELEASED,
    VSCROLL_UP,
    VSCROLL_DOWN
};

struct levelObject
{
    unsigned int ogID;
    unsigned int duplicateID;
    sf::Sprite self;
};

template <typename T>
using uptr = std::unique_ptr<T>;

class Level
{
public:
    Level() = default;

    Level(std::string name, sf::Texture &texture, sf::Vector2f &gridConfig, std::vector<sf::Vector2i> &posConfig, std::vector<sf::Vector2i> &sizeConfig);

    void init(std::string levelFile);

    void draw(sf::RenderTarget &window);

    void editor(editor_state &estate, mouse_state &mstate, sf::RenderWindow &window, sf::Event &event, float dt);

    void serialize(std::string filename);

private:
    int objDragID;
    bool duplicateKeyPressed;
    bool deleteKeyPressed;
    std::string id;

    std::vector<uptr<levelObject>> objects;
    std::vector<sf::Vector2i> posConfig;
    std::vector<sf::Vector2i> sizeConfig;

    sf::Vector2f gridConfig;
    sf::Vector2f mousePos;
    sf::Vector2f mouseDelta;

    float t;
    float tHold;

    void getLevelObjects(sf::Texture &texture, std::vector<sf::Vector2i> &posConfig, std::vector<sf::Vector2i> &sizeConfig);

    bool isClickable(uptr<levelObject> &obj, sf::RenderWindow &window);
};

#endif
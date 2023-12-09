#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>


#include <World.h>
#include <DynamicRB.h>
#include <Entity.h>
#include <Level.h>
#include <StaticRB.h>

#define WINDOW_WIDTH 1900
#define WINDOW_HEIGHT 900

const float GRAVITY = -250.f;

int main()
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "2D Game");
    window.setFramerateLimit(144);
    window.setKeyRepeatEnabled(true);

    // need to figure out how to batch load :/
    sf::Texture playerTexture;
    sf::Texture levelTexture;
    if (!playerTexture.loadFromFile("assets/bob-sheet.png"))
    {
        std::cerr << "[ERROR]: Texture doesn't exist" << std::endl;
        exit(1);
    }
    if (!levelTexture.loadFromFile("assets/levelSheet.png"))
    {
        std::cerr << "[ERROR]: Texture doesn't exist" << std::endl;
        exit(1);
    }

    // world
    World Environment(GRAVITY);

    // level
    bool enableDev = false;
    std::vector<sf::Vector2i> posConfig = {
        {0, 0}, {32, 0}, {64, 0}, {0, 16}, {10, 16}};
    std::vector<sf::Vector2i> sizeConfig = {
        {32, 16}, {24, 16}, {16, 16}, {10, 16}, {7, 16}};
    sf::Vector2f gridConfig = {8, 8};
    Level Stage("lvl_00", levelTexture, gridConfig, posConfig, sizeConfig);
    std::string levelSaveFile = (enableDev) ? "" : "level00.txt";
    Stage.init(levelSaveFile);

    // entities
    sf::Vector2i spriteWindow = {32, 32};
    sf::Vector2f scale(2.f, 2.f);
    sptr<RigidBody::Dynamic> Player = std::make_shared<RigidBody::Dynamic>(RigidBody::Dynamic("Player", playerTexture, spriteWindow, {0, 0}, spriteWindow, scale));
    Environment >> Player;
    Environment.init();

    
    mouse_state mouseState;
    state playerState = IDLE;
    sf::Clock clock;
    editor_state editorState = NONE;
    while (window.isOpen())
    {
        sf::Event event = sf::Event{};
        for (; window.pollEvent(event);)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || event.type == sf::Event::Closed)
            {
                window.resetGLStates();
                window.clear();
                window.setActive(false);
                window.close();
            }
        }
        window.clear();

        float dt = clock.restart().asSeconds();
        orientation playerDir = RESET;
        // editor_state editorState = NONE;

        // update environment
        Environment.update(dt);

        if (enableDev)
        {
            mouseState = LMB_RELEASED;
            if(event.type == sf::Event::MouseButtonPressed && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
                mouseState = LMB_CLICKED;
            if(editorState == SELECTED)
            {    
                if (event.type == sf::Event::MouseWheelScrolled)
                    mouseState = (event.mouseWheelScroll.delta > 0) ? VSCROLL_UP : VSCROLL_DOWN;
            }
            Stage.editor(editorState, mouseState, window, event, dt);
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            {
                std::cout << "Saved!" << std::endl;
                Stage.serialize("level00.txt");
            }
        }
        

        // draw
        Stage.draw(window);
        Environment.draw(window);
        window.display();
    }
}

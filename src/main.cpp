#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>

#include <Entity.h>
#include <Level.h>
#include <World.h>

#define WINDOW_WIDTH 1900
#define WINDOW_HEIGHT 900

const float GRAVITY = -250.f;

void updatePlayerState(entity_sptr &Player, state &playerState, orientation &playerDir);

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
    std::vector<sf::Vector2i> posConfig = {
        {0, 0}, {32, 0}, {64, 0}, {0, 16}, {10, 16}};
    std::vector<sf::Vector2i> sizeConfig = {
        {32, 16}, {24, 16}, {16, 16}, {10, 16}, {7, 16}};
    sf::Vector2f gridConfig = window.mapPixelToCoords({8, 8});
    Level Stage("lvl_00", levelTexture, gridConfig, posConfig, sizeConfig);
    Stage.init(window);

    // entities
    sf::Vector2i spriteWindow = {32, 32};
    sf::Vector2f scale(2.f, 2.f);
    entity_sptr Player = std::make_shared<Entity>(Entity{"Player", playerTexture, spriteWindow, scale});
    Environment >> Player;
    Environment.init();

    bool playerLaunched = false;
    bool jumpKeyPress = false;
    bool dupObject = false;
    bool clicked = false;

    state playerState = IDLE;
    sf::Clock clock;
    EDITOR_STATE editorState = NONE;
    while (window.isOpen())
    {
        sf::Event event = sf::Event{};
        for (; window.pollEvent(event);)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) || event.type == sf::Event::Closed)
            {
                window.close();
            }
        }
        window.clear();

        float dt = clock.restart().asSeconds();
        orientation playerDir = RESET;
        // EDITOR_STATE editorState = NONE;

        // update environment
        Environment.update(dt);
        if (editorState != SELECTED && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
            editorState = SELECTED;
        else if (editorState == SELECTED && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
        {
            editorState = UNSELECTED;
            dupObject = false;
        }

        if (editorState == SELECTED && event.type == sf::Event::MouseWheelScrolled)
            editorState = (event.mouseWheelScroll.delta > 0) ? ZOOM_IN : ZOOM_OUT;
        else if (editorState == SELECTED && sf::Keyboard::isKeyPressed(sf::Keyboard::C) && !dupObject)
        {
            editorState = DUPLICATE;
            dupObject = true;
        }

        Stage.editor(true, editorState, "", window, event, dt);

        // updates entities
        updatePlayerState(Player, playerState, playerDir);
        Player->animate(playerState, spriteWindow, {0, 0}, dt);
        Player->move(playerDir, dt);

        // draw
        Stage.draw(window);
        Player->draw(window);
        window.display();
    }
}

void updatePlayerState(entity_sptr &Player, state &playerState, orientation &playerDir)
{
    if (Player->grounded && Player->jumped && Player->jumpCharge == -2)
    {
        Player->jumped = false;
        Player->jumpCharge = 3;
        playerState = IDLE;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Space) || playerState == JUMP)
    {
        playerState = JUMP;
        Player->jump();
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        playerDir = LEFT;
        if (Player->grounded && playerState != JUMP)
            playerState = WALK;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        playerDir = RIGHT;
        if (Player->grounded && playerState != JUMP)
            playerState = WALK;
    }
    else if (Player->grounded && playerState != JUMP)
        playerState = IDLE;
}
#include <cmath>
#include <fstream> 
#include <iostream>
#include <sstream>
#include <Level.h>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Mouse.hpp>
#include <SFML/Graphics.hpp>

#define SCALE_FACTOR 0.25

Level::Level(std::string name, sf::Texture &texture, sf::Vector2f &gridConfig, std::vector<sf::Vector2i> &posConfig, std::vector<sf::Vector2i> &sizeConfig) : id{name}, objDragID{-1}, gridConfig{gridConfig}, posConfig{posConfig}, sizeConfig{sizeConfig}
{
    this->getLevelObjects(texture, posConfig, sizeConfig);
}

void Level::init(std::string levelFile)
{
    sf::Vector2f pos{0.f, 0.f};
    if(levelFile != "")
    {
        unsigned int i = 0; 
        std::string line;
        unsigned int ogID, dupID;
        sf::Vector2f pos, scale;
        std::ifstream infile(levelFile);
        while(std::getline(infile, line))
        {
            std::stringstream ss(line);
            ss >> ogID >> dupID >> pos.x >> pos.y >> scale.x >> scale.y; 
            if(i < this->objects.size())
            {
                this->objects[i]->ogID = ogID;
                this->objects[i]->duplicateID = dupID;
                this->objects[i]->self.setPosition(pos);
            }
            else
            {
                sf::Sprite self = this->objects[ogID]->self;
                self.setPosition(pos);
                self.setScale(scale);
                uptr<levelObject> obj = std::make_unique<levelObject>(levelObject{ogID, dupID, self});
                this->objects.push_back(std::move(obj));
            }
            ++i;
        }
    }
    else
    {
        for (uptr<levelObject> &obj : this->objects)
        {
            obj->self.setPosition(pos);
            pos += {obj->self.getGlobalBounds().width, 0};
        }
    }
}

void Level::serialize(std::string filename)
{
    std::ofstream outfile(filename);
    for(uptr<levelObject> &obj : this->objects)
    {
        sf::Vector2f pos = obj->self.getPosition();
        sf::Vector2f scale = obj->self.getScale();
        outfile << obj->ogID << " " << obj->duplicateID << " " << pos.x << " " << pos.y << " " << scale.x << " " << scale.y << std::endl;
    }
    outfile.close();
}

void Level::draw(sf::RenderTarget &window)
{
    if (this->objects.empty())
    {
        std::cerr << "[ERROR]: Level has not been initialized!" << std::endl;
        exit(1);
    }
    for (uptr<levelObject> &obj : this->objects)
        window.draw((*obj).self);
}

void Level::editor(editor_state &estate, mouse_state &mstate, sf::RenderWindow &window, sf::Event &event, float dt)
{
    int i = 0;
    for (uptr<levelObject> &obj : this->objects)
    {
        if (mstate == LMB_CLICKED && this->isClickable(obj, window))
        {
            this->objDragID = i;
            estate = SELECTED;
            break; // can only drag one object
        }
        else if(!this->isClickable(obj, window) && mstate == LMB_CLICKED)
            estate = UNSELECTED; 
        ++i;
    }
    if (this->objDragID != -1) // if an object is draggable, lock onto object
    {
        levelObject objDup;
        uptr<levelObject> &obj = this->objects[this->objDragID]; // get draggable object
        sf::Vector2f scale;

        if (mstate == VSCROLL_UP)
            estate = ZOOM_IN;
        if (mstate == VSCROLL_DOWN)
            estate = ZOOM_OUT;
        if (!this->duplicateKeyPressed && estate == SELECTED && sf::Keyboard::isKeyPressed(sf::Keyboard::C))
            estate = DUPLICATE;
        else if (!this->deleteKeyPressed && estate == SELECTED && sf::Keyboard::isKeyPressed(sf::Keyboard::Delete))
            estate = DELETE;
        switch (estate)
        {
        case SELECTED:
            if(event.type == sf::Event::KeyPressed)
            {   
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                {
                    obj->self.move(this->gridConfig.x, 0.f);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                        obj->self.move(this->objects[this->objDragID]->self.getGlobalBounds().width, 0.f);
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                {
                    obj->self.move(-1 * this->gridConfig.x, 0.f);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                        obj->self.move(-1 * this->objects[this->objDragID]->self.getGlobalBounds().width, 0.f);
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                {
                    obj->self.move(0.f, -1 * this->gridConfig.y);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                        obj->self.move(0.f, -1 * this->objects[this->objDragID]->self.getGlobalBounds().height);
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                {
                    obj->self.move(0.f, this->gridConfig.y);
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
                        obj->self.move(0.f, this->objects[this->objDragID]->self.getGlobalBounds().height);
                }
                this->duplicateKeyPressed = false;
                this->deleteKeyPressed = false;
            }
            break;
        case DUPLICATE:
            this->duplicateKeyPressed = true;
            objDup = *obj;
            objDup.duplicateID = this->objects.size() - 1;
            this->objects.push_back(std::make_unique<levelObject>(objDup));
            this->sizeConfig.push_back(this->sizeConfig[this->objDragID]);
            this->objDragID = this->objects.size() - 1; 
            estate = SELECTED;
            break;
        case UNSELECTED:
            this->objDragID = -1;
            break;
        case DELETE:
            std::cout << "trig del - " << this->objects[this->objDragID]->ogID << " | " << this->objects[this->objDragID]->duplicateID << " !" << std::endl;
            this->deleteKeyPressed = true;
            if(this->objects[this->objDragID]->ogID != this->objects[this->objDragID]->duplicateID) // check if not a fundamental block, but a duplicate
            {    
                std::cout << "deteting..." << std::endl;
                this->objects.erase(this->objects.begin() + this->objDragID);
            }
            break;
        case ZOOM_IN:
            scale = {1 - SCALE_FACTOR, 1 - SCALE_FACTOR};
            obj->self.scale(sf::Vector2f(scale));
            this->sizeConfig[this->objDragID] = sf::Vector2i(obj->self.getGlobalBounds().width, obj->self.getGlobalBounds().height);
            estate = SELECTED;
            break;
        case ZOOM_OUT:
            scale = {1 + SCALE_FACTOR, 1 + SCALE_FACTOR};
            obj->self.scale(sf::Vector2f(scale));
            this->sizeConfig[this->objDragID] = sf::Vector2i(obj->self.getGlobalBounds().width, obj->self.getGlobalBounds().height);
            estate = SELECTED;
            break;
        case NONE:
            break;
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
    for (unsigned int i = 0; i < posConfig.size(); ++i)
    {
        sf::Vector2i pos = posConfig[i];
        sf::Vector2i size = sizeConfig[i];
        sf::IntRect objSpriteBox(pos, size);
        uptr<levelObject> obj = std::make_unique<levelObject>(levelObject{i, i, sf::Sprite{texture, objSpriteBox}});
        obj->self.setScale(3.f, 3.f);
        this->objects.push_back(std::move(obj));
    }
}

bool Level::isClickable(uptr<levelObject> &obj, sf::RenderWindow &window)
{
    sf::FloatRect objBBox = obj->self.getGlobalBounds();
    sf::Vector2f mouseLoc = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    return objBBox.contains(mouseLoc);
}
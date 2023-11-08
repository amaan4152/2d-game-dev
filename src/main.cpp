#include <memory>
#include <SFML/Graphics.hpp>

int main()
{
    std::unique_ptr<sf::RenderWindow> window = std::make_unique<sf::RenderWindow>(sf::RenderWindow{{1920u, 1080u}, "2D Game"});
    window->setFramerateLimit(144);

    while (window->isOpen())
    {
        for (sf::Event event = sf::Event{}; window->pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window->close();
            }
        }

        // all drawing goes here ...

        window->clear();
        window->display();
    }
}
#include <SFML/Graphics.hpp>

int main()
{
    auto window = new sf::RenderWindow{{1920u, 1080u}, "CMake SFML Project"};
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

        window->clear();
        window->display();
    }
}
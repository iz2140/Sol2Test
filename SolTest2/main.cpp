
//
// Disclamer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resource, use the helper
// method resourcePath() from ResourcePath.hpp
//

//#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include "sol.hpp"
// Here is a small helper for you ! Have a look.
#include "ResourcePath.hpp"
#include "girl.hpp"
#include "tampon.hpp"

int nextTampon = 0;

int main(int, char const**)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1600, 1200), "Window");
    window.setFramerateLimit(60);
    
    //setting up the bg
    sf::Vector2f bgVector(1600.f,1200.f);
    sf::RectangleShape bg(bgVector);
    bg.setFillColor(sf::Color::Blue);

    
    girl player = girl(800, 600);
    
    sf::Texture texture;
    if (!texture.loadFromFile(resourcePath() + "ammo.png")) {
        return EXIT_FAILURE;
    }
    
    std::vector<tampon> tampons;
    
    for (int i = 0; i < 10 ; i++){
        tampons.push_back(tampon(texture));
    }

    

    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile(resourcePath() + "icon.png")) {
        return EXIT_FAILURE;
    }
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());


    // Create a graphical text to display
    sf::Font font;
    if (!font.loadFromFile(resourcePath() + "sansation.ttf")) {
        return EXIT_FAILURE;
    }
    sf::Text text("Hello SFML", font, 50);
    text.setColor(sf::Color::Black);


    sf::Clock clock;
    
    sf::Clock clock2;
    
    // Start the game loop
    while (window.isOpen())
    {
        
        // Process events
        player.animate(clock);
        
        sf::Event event; //called when an event (mouse over, click, whatver) happens
        while (window.pollEvent(event))
        {
            
            
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            
            //spacebar pressed
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
                
                std::cout << "hi\n";
                if (!tampons[nextTampon].exists){
                    std::cout << "shooting a tampon " << nextTampon << "\n";

                    tampons[nextTampon].shoot();
                    if (nextTampon == tampons.size()-1){
                        std::cout << "setting nextTampon to 0\n";
                        nextTampon = 0;
                    } else {
                        nextTampon++;
                    }
                }
            }
            
            
        }

        // Clear screen
        window.clear();

        window.draw(bg);
        
        
        //draw all tampons
        for (int i = 0; i < 10; i++)
        {
            if (tampons[i].exists){
                window.draw(tampons[i].sprite);
                tampons[i].move(10,0);
                tampons[i].animate(clock2);
            }
            
        }
        
        // Draw the sprite
        window.draw(player.sprite);
        
        
        // Update the window
        window.display();
    }
    
    sol::state lua;
    lua.open_libraries( sol::lib::base );
    
    lua.script( "print('bark bark bark!')" );
    return EXIT_SUCCESS;
}

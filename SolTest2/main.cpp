
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



int main(int, char const**)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1600, 1200), "SFML window");
    
    //setting up the bg
    sf::Vector2f bgVector(1600.f,1200.f);
    sf::RectangleShape bg(bgVector);
    bg.setFillColor(sf::Color::Blue);

    
    //setting up the girl sprite
    // Load a sprite to display
//    sf::Texture texture;
//    if (!texture.loadFromFile(resourcePath() + "girlWalk.png")) {
//        return EXIT_FAILURE;
//    }
//    
//    //sf::Sprite sprite(texture);
//    sf::IntRect rectSourceSprite(0,0,59,64);
//    
//    sf::Sprite sprite(texture, rectSourceSprite);
//    //sprite.setTextureRect(sf::IntRect(0,0,59,64));
//    sprite.setPosition(800,600);
    girl player(girl(800, 600));

    

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
    
    // Start the game loop
    while (window.isOpen())
    {
        

        
        // Process events
        
        //update animation for girl
//        if (clock.getElapsedTime().asSeconds() > 0.3f){
//            if (rectSourceSprite.left == 59){
//                //std::cout << "hi ";
//                rectSourceSprite.left = 0;
//            }else{
//                rectSourceSprite.left = 59;
//                //std::cout << "bye ";
//            }
//            
//            sprite.setTextureRect(rectSourceSprite);
//            
//            clock.restart();
//        }
        
        player.animate(clock);

        
        sf::Event event; //called when an event (mouse over, tap, whatver) happens
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
            
        }

        // Clear screen
        window.clear();

        window.draw(bg);
        
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

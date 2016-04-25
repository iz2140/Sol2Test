
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
#include "ResourcePath.hpp"
#include "girl.hpp"
#include "tampon.hpp"
#include "enemy.hpp"
#include "cloud.hpp"
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include <random>
#include <chrono>

int nextTampon = 0;
int nextEnemy = 0;
int nextCloud = 0;
int score = 10;

sf::RectangleShape drawFloor();
sf::RectangleShape drawSky();


void spawnClouds(sf::Clock& clock, std::vector<cloud>& clouds);


void spawnEnemy(sf::Clock& clock, std::vector<enemy>& enemies){
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    auto die = bind(std::uniform_real_distribution<float>{1.0,50.0}, generator);
    float random_time = die();
    if (clock.getElapsedTime().asSeconds() > random_time){
        if (!enemies[nextEnemy].exists){
            std::cout << "spawning an enemy " << nextEnemy << "\n";
            
            enemies[nextEnemy].appear();
            if (nextEnemy == enemies.size()-1){
                std::cout << "setting nextEnemy to 0\n";
                nextEnemy = 0;
            } else {
                nextEnemy++;
            }
        }
        clock.restart();
    }
}


int main(int, char const**)
{
    

    
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1600, 1200), "Window");
    window.setFramerateLimit(60);
    
    sf::RectangleShape floor = drawFloor();
    sf::RectangleShape sky = drawSky();
    
    girl player = girl(400, 750);
    
    sf::Texture texture;
    if (!texture.loadFromFile(resourcePath() + "ammo.png")) {
        return EXIT_FAILURE;
    }
    
    std::vector<tampon> tampons;
    for (int i = 0; i < 10 ; i++){
        tampons.push_back(tampon(texture));
    }
    
    sf::Texture enemy_texture;
    if (!enemy_texture.loadFromFile(resourcePath() + "enemy.png")) {
        return EXIT_FAILURE;
    }
    std::vector<enemy> enemies;
    for (int i = 0; i < 10 ; i++){
        enemies.push_back(enemy(enemy_texture));
    }
    
    sf::Texture cloud_texture;
    if (!cloud_texture.loadFromFile(resourcePath() + "cloud.png")) {
        return EXIT_FAILURE;
    }
    
    std::vector<cloud> clouds;
    for (int i = 0; i < 4 ; i++){
        clouds.push_back(cloud(cloud_texture));
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
    
    sf::Clock clock3;
    
    sf::Clock cloudClock;
    
    // Start the game loop
    while (window.isOpen())
    {
        
        // Process events
        player.animate(clock);
        
        spawnEnemy(clock3, enemies);
        spawnClouds(cloudClock, clouds);

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

        window.draw(floor);
        window.draw(sky);
        
        
        //draw all tampons
        for (int i = 0; i < 10; i++)
        {
            if (tampons[i].exists){
                window.draw(tampons[i].sprite);
                for (int j = 0; j < 10; j++)
                {
                    if (enemies[j].exists){
                        tampons[i].checkCollision(enemies[j]);
                    }
                }
                tampons[i].move(10,0);
                tampons[i].animate(clock2);
            }
            
        }
        
        
        for (int i = 0; i < 10; i++)
        {
            if (enemies[i].exists){
                window.draw(enemies[i].sprite);
                //std::cout << enemies[i].speed << std::endl;
                enemies[i].move(-enemies[i].speed,0);
                enemies[i].animate(clock2);
            }
            
        }
        
        for (int i = 0; i < 4; i++)
        {
            if (clouds[i].exists){
                window.draw(clouds[i].sprite);
                //std::cout << enemies[i].speed << std::endl;
                clouds[i].move(-2,0);
                //clouds[i].animate(clock2);
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

sf::RectangleShape drawFloor(){
    //setting up the bg
    sf::Vector2f bgVector(1600.f,400.f);
    sf::RectangleShape bg(bgVector);
    bg.setFillColor(sf::Color::Black);
    bg.setPosition(0, 200);
    return bg;
}

sf::RectangleShape drawSky(){
    //setting up the bg
    sf::Vector2f bgVector(1600.f,800.f);
    sf::RectangleShape bg(bgVector);
    bg.setFillColor(sf::Color::Color(100,100,100,255));
    bg.setPosition(0, 0);
    return bg;
}

void spawnClouds(sf::Clock& clock, std::vector<cloud>& clouds){
    if (clock.getElapsedTime().asSeconds() > 6.0){
        if (!clouds[nextCloud].exists){
            //std::cout << "spawning an enemy " << nextEnemy << "\n";
            
            clouds[nextCloud].appear();
            if (nextCloud == clouds.size()-1){
                //std::cout << "setting nextEnemy to 0\n";
                nextCloud = 0;
            } else {
                nextCloud++;
            }
        }
        clock.restart();
    }
}

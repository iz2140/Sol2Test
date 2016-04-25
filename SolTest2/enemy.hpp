//
//  enemy.hpp
//  SolTest2
//
//  Created by Iris Zhang on 4/24/16.
//  Copyright © 2016 InteractAble. All rights reserved.
//

#ifndef enemy_hpp
#define enemy_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include "ResourcePath.hpp"
#include "girl.hpp"


class enemy {
    sf::Texture texture;
    sf::IntRect rectSourceSprite;
    
    
public:
    sf::Sprite sprite;
    bool exists;
    int speed;
    //constructor
    enemy(sf::Texture &texture);
    //~girl();
    
    void animate(sf::Clock &clock);
    void appear();
    void move(float a, float b);
    void destroy();
    sf::FloatRect getRect();
    
};


#endif /* enemy_hpp */

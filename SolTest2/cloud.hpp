//
//  cloud.hpp
//  SolTest2
//
//  Created by Iris Zhang on 4/25/16.
//  Copyright © 2016 InteractAble. All rights reserved.
//

#ifndef cloud_hpp
#define cloud_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include "ResourcePath.hpp"



class cloud {
    sf::Texture texture;
    sf::IntRect rectSourceSprite;
    
    
public:
    sf::Sprite sprite;
    bool exists;
    int speed;
    //constructor
    cloud(sf::Texture &texture);
    //~girl();
    
    void animate(sf::Clock &clock);
    void appear();
    void move(float a, float b);
    void destroy();
    sf::FloatRect getRect();
    
};



#endif /* cloud_hpp */

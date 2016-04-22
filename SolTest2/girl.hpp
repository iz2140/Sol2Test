//
//  girl.hpp
//  SolTest2
//
//  Created by Iris Zhang on 4/22/16.
//  Copyright © 2016 InteractAble. All rights reserved.
//

#ifndef girl_hpp
#define girl_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include "ResourcePath.hpp"

class girl {
    sf::Texture texture;
    sf::IntRect rectSourceSprite;
    
    
public:
    int tampons;
    sf::Sprite sprite;
    
    //constructor
    girl(int initialX, int initialY);
    //~girl();
    
    bool collidesWithEnemy();
    bool collidesWithBox();
    void shootTampon();
    void animate(sf::Clock &clock);
    
};

#endif /* girl_hpp */

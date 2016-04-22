//
//  girl.cpp
//  SolTest2
//
//  Created by Iris Zhang on 4/22/16.
//  Copyright © 2016 InteractAble. All rights reserved.
//

#include "girl.hpp"
#include <iostream>

//implementation of constructor
girl::girl(int initialX, int initialY){
    
    tampons = 10; //initially starting score
    
    if (!texture.loadFromFile(resourcePath() + "girlWalk.png")) {
        return EXIT_FAILURE;
    }
    
    sf::IntRect myRectSourceSprite(0,0,59,64);
    rectSourceSprite = myRectSourceSprite;
    
    sf::Sprite mySprite(texture, rectSourceSprite);
    
    sprite = mySprite;
    //sprite.setTextureRect(sf::IntRect(0,0,59,64));
    sprite.setPosition(initialX,initialY);
    std::cout << "am i being called\n";
    
}

void girl::animate(sf::Clock& clock){
    if (clock.getElapsedTime().asSeconds() > 0.3f){
        if (rectSourceSprite.left == 59){
            //std::cout << "hi ";
            rectSourceSprite.left = 0;
        }else{
            rectSourceSprite.left = 59;
            //std::cout << "bye ";
        }
        
        sprite.setTextureRect(rectSourceSprite);
        
        clock.restart();
    }

}
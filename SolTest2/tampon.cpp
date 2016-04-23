//
//  tampon.cpp
//  SolTest2
//
//  Created by Iris Zhang on 4/22/16.
//  Copyright © 2016 InteractAble. All rights reserved.
//

#include "tampon.hpp"
#include <iostream>

//implementation of constructor
tampon::tampon(sf::Texture &texture){
    
    
//    if (!texture.loadFromFile(resourcePath() + "ammo.png")) {
//        return EXIT_FAILURE;
//    }
    
    sf::IntRect myRectSourceSprite(0,0,73,16);
    rectSourceSprite = myRectSourceSprite;
    
    sf::Sprite mySprite(texture, rectSourceSprite);
    
    sprite = mySprite;
    //sprite.setTextureRect(sf::IntRect(0,0,59,64));
    sprite.setPosition(600,600);
    
    
    //std::cout << "am i getting called\n";
    
}

void tampon::animate(sf::Clock& clock){
    if (clock.getElapsedTime().asSeconds() > 0.3f){
        if (rectSourceSprite.left == 219){
            //std::cout << "hi ";
            rectSourceSprite.left = 0;
        }else{
            rectSourceSprite.left += 73;
            //std::cout << "bye ";
        }
        
        sprite.setTextureRect(rectSourceSprite);
        
        clock.restart();
    }
    
}
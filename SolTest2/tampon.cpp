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
    
    
    sf::IntRect myRectSourceSprite(0,0,73,16);
    rectSourceSprite = myRectSourceSprite;
    
    sf::Sprite mySprite(texture, rectSourceSprite);
    
    sprite = mySprite;
    
    
    exists = false;
    
    
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


void tampon::shoot(){
    sprite.setPosition(600,600);
    exists = true;
    //sprite.move(1.0, 0);
    //clock.restart();
}

void tampon::move(float a, float b){
    sprite.move(a, b);
    if (sprite.getPosition().x > 1600){
        std::cout << "destroying tampon\n";
        exists = false;
    }
}


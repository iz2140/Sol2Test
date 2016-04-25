//
//  cloud.cpp
//  SolTest2
//
//  Created by Iris Zhang on 4/25/16.
//  Copyright © 2016 InteractAble. All rights reserved.
//

#include "cloud.hpp"

//implementation of constructor
cloud::cloud(sf::Texture &texture){
    
    sf::IntRect myRectSourceSprite(0,0,55,60);
    rectSourceSprite = myRectSourceSprite;
    
    sf::Sprite mySprite(texture, rectSourceSprite);
    
    sprite = mySprite;
    exists = false;
    
}

void cloud::animate(sf::Clock& clock){
    if (clock.getElapsedTime().asSeconds() > 0.3f){
        if (rectSourceSprite.left == 55){
            //std::cout << "hi ";
            rectSourceSprite.left = 0;
        }else{
            rectSourceSprite.left = 55;
            //std::cout << "bye ";
        }
        
        sprite.setTextureRect(rectSourceSprite);
        
        clock.restart();
    }
}

sf::FloatRect cloud::getRect(){
    sf::FloatRect temp = sprite.getLocalBounds();
    temp.left = sprite.getPosition().x;
    temp.top = sprite.getPosition().y;
    return temp;
}

void cloud::appear(){
    sprite.setPosition(1600,600);
    exists = true;
}

void cloud::destroy(){
    //std::cout << "destroying cloud\n";
    exists = false;
}


void cloud::move(float a, float b){
    sprite.move(a, b);
    if (sprite.getPosition().x < 0){
        this->destroy();
    }
}

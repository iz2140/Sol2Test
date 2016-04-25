//
//  enemy.cpp
//  SolTest2
//
//  Created by Iris Zhang on 4/24/16.
//  Copyright © 2016 InteractAble. All rights reserved.
//

#include "enemy.hpp"
#include <iostream>
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include <random>
#include <chrono>

//implementation of constructor
enemy::enemy(sf::Texture &texture){
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine generator(seed);
    auto die = bind(std::uniform_int_distribution<>{5,50}, generator);
    speed = die();
    
    sf::IntRect myRectSourceSprite(0,0,55,60);
    rectSourceSprite = myRectSourceSprite;
    
    sf::Sprite mySprite(texture, rectSourceSprite);
    
    sprite = mySprite;
    exists = false;
    
}

void enemy::animate(sf::Clock& clock){
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

sf::FloatRect enemy::getRect(){
    sf::FloatRect temp = sprite.getLocalBounds();
    temp.left = sprite.getPosition().x;
    temp.top = sprite.getPosition().y;
    return temp;
}

void enemy::appear(){
    sprite.setPosition(1600,600);
    exists = true;
}

void enemy::destroy(){
    std::cout << "destroying enemy\n";
    exists = false;
}


void enemy::move(float a, float b){
    sprite.move(a, b);
    if (sprite.getPosition().x < 10){
        this->destroy();
    }
}


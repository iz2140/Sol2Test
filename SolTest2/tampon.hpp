//
//  tampon.hpp
//  SolTest2
//
//  Created by Iris Zhang on 4/22/16.
//  Copyright © 2016 InteractAble. All rights reserved.
//

#ifndef tampon_hpp
#define tampon_hpp

#include <stdio.h>
#include <SFML/Graphics.hpp>
#include "ResourcePath.hpp"
#include "girl.hpp"
#include "enemy.hpp"
#include <memory>


class tampon {
    static std::unique_ptr<sf::Texture> texture;
    sf::IntRect rectSourceSprite;
    
public:
    
    sf::Sprite sprite;
    bool exists;
    //constructor
    tampon(sf::Texture &texture);
    //tampon();
    //~girl();
    
    void animate(sf::Clock &clock);
    void shoot();
    void move(float a, float b);
    void destroy();
    bool checkCollision(enemy& enemy);
    sf::FloatRect getRect();
};

#endif /* tampon_hpp */

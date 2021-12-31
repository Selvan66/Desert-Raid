#pragma once

#include "Objects/SceneNode.hpp"

class SpriteNode : public SceneNode {
    public:
        explicit SpriteNode(const sf::Texture& texture);
        SpriteNode(const sf::Texture& texture, const sf::IntRect& textureRect);
    private:
        virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
    private:
        sf::Sprite mSprite;
};

SpriteNode::SpriteNode(const sf::Texture& texture) 
: mSprite(texture) {
}

SpriteNode::SpriteNode(const sf::Texture& texture, const sf::IntRect& textureRect)
: mSprite(texture, textureRect) {
}

void SpriteNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(mSprite, states);    
}



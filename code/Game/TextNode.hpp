#pragma once

#include "Game/SceneNode.hpp"
#include "Utils/ResourceIdentifiers.hpp"
#include "Utils/Utility.hpp"

class TextNode : public SceneNode {
    public:
        explicit TextNode(const FontHolder& fonts, const std::string& text);
        void setString(const std::string& text);
    private:
        virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
    private:
        sf::Text mText;
};

TextNode::TextNode(const FontHolder& fonts, const std::string& text) {
    mText.setFont(fonts.get(Fonts::Sansation));
    mText.setCharacterSize(20);
    setString(text);
}

void TextNode::setString(const std::string& text) {
    target.draw(mText, states);
}

void TextNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
    mText.setString(text);
    centerOrigin(mText);
}



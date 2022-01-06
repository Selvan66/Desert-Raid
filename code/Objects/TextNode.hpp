#pragma once

#include "Objects/SceneNode.hpp"
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
    mText.setString(text);
    Utility::centerOrigin(mText);
}

void TextNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(mText, states);
}



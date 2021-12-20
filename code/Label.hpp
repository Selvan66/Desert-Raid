#pragma once

#include "Container.hpp"
#include "ResourceIdentifiers.hpp"

#include <string>

namespace GUI {
class Label : public Component {
    public:
        typedef std::shared_ptr<Label> Ptr;
    public:
        Label(std::string& text, const FontHolder& fonts);
        virtual bool isSelectable() const;
        void setText(const std::string& text);
        virtual void handleEvent(const sf::Event& event);
    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    private:
        sf::Text mText;
};

Label::Label(std::string& text, const FontHolder& fonts) 
: mText(text, fonts.get(Fonts::Sansation), 16) {
}

bool Label::isSelectable() const{
    return false;
}

void Label::setText(const std::string& text) {
    mText.setString(text);
}

void Label::handleEvent(const sf::Event& event) {
}

void Label::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= Transformable::getTransform();
    target.draw(mText, states);
}

}
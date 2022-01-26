#pragma once

#include "Components/Component.hpp"
#include "Utils/ResourceIdentifiers.hpp"
#include "Utils/Utility.hpp"

#include <functional>


namespace GUI {
class Button : public Component {
    public:
        typedef std::shared_ptr<Button> Ptr;
        typedef std::function<void()> Callback;

        enum Type {
            Normal,
            Selected,
            Pressed,
            ButtonCount
        };
    public:
        Button(const FontHolder& fonts, const TextureHolder& textures);
        void setCallback(Callback callback);
        void setText(const std::string& text);
        void setToggle(bool flag);
        virtual bool isSelectable() const;
        virtual void select();
        virtual void deselect();
        virtual void activate();
        virtual void deactivate();
        virtual void handleEvent(const sf::Event& event);
    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        void changeTexture(Type buttonType);
    private:
        Callback mCallback;
        sf::Sprite mSprite;
        sf::Text mText;
        bool mIsToggle;
};

Button::Button(const FontHolder& fonts, const TextureHolder& textures) 
: mCallback(), mSprite(textures.get(Textures::Buttons)), mText("", fonts.get(Fonts::Sansation), 16), mIsToggle(false) {
    changeTexture(Normal);
    sf::FloatRect bounds = mSprite.getLocalBounds();
    mText.setPosition(bounds.width / 2.f, bounds.height / 2.f);
}

void Button::setCallback(Callback callback) {
    mCallback = std::move(callback);
}

void Button::setText(const std::string& text) {
    mText.setString(text);
    Utility::centerOrigin(mText);
}

void Button::setToggle(bool flag) {
    mIsToggle = flag;
}

bool Button::isSelectable() const {
    return true;
}

void Button::select() {
    Component::select();
    changeTexture(Selected);
}

void Button::deselect() {
    Component::deselect();
    changeTexture(Normal);
}

void Button::activate() {
    Component::activate();
    if (mIsToggle) {
        changeTexture(Pressed);
    }
    if (mCallback) {
        mCallback();
    }
    if (!mIsToggle) {
        deactivate();
    }
}

void Button::deactivate() {
    Component::deactivate();
    if (mIsToggle) {
        if (isSelected()) {
            changeTexture(Selected);
        }
        else {
            changeTexture(Normal);
        }
    }
}

void Button::handleEvent(const sf::Event& event) {    
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    states.transform *= Transformable::getTransform();
    target.draw(mSprite, states);
    target.draw(mText, states);
}

void Button::changeTexture(Type buttonType) {
    sf::IntRect textureRect(0, 50 * buttonType, 200, 50);
    mSprite.setTextureRect(textureRect);
}

}

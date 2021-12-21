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
    private:
        Callback mCallback;
        const sf::Texture& mNormalTexture;
        const sf::Texture& mSelectedTexture;
        const sf::Texture& mPressedTexture;
        sf::Sprite mSprite;
        sf::Text mText;
        bool mIsToggle;
};

Button::Button(const FontHolder& fonts, const TextureHolder& textures) 
: mCallback(), mNormalTexture(textures.get(Textures::ButtonNormal)), mSelectedTexture(textures.get(Textures::ButtonSelected)),
mPressedTexture(textures.get(Textures::ButtonPressed)), mText("", fonts.get(Fonts::Sansation), 16), mIsToggle(false) {
    mSprite.setTexture(mNormalTexture);
    sf::FloatRect bounds = mSprite.getLocalBounds();
    mText.setPosition(bounds.width / 2.f, bounds.height / 2.f);
}

void Button::setCallback(Callback callback) {
    mCallback = std::move(callback);
}

void Button::setText(const std::string& text) {
    mText.setString(text);
    centerOrigin(mText);
}

void Button::setToggle(bool flag) {
    mIsToggle = flag;
}

bool Button::isSelectable() const {
    return true;
}

void Button::select() {
    Component::select();
    mSprite.setTexture(mSelectedTexture);
}

void Button::deselect() {
    Component::deselect();
    mSprite.setTexture(mNormalTexture);
}

void Button::activate() {
    Component::activate();
    if (mIsToggle) {
        mSprite.setTexture(mPressedTexture);
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
            mSprite.setTexture(mSelectedTexture);
        }
        else {
            mSprite.setTexture(mNormalTexture);
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

}

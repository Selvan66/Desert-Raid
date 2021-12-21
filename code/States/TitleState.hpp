#pragma once

#include "States/StateStack.hpp"
#include "Utils/Utility.hpp"

class TitleState : public State {
    public:
        TitleState(StateStack& stack, Context context);
        virtual void draw();
        virtual bool update(sf::Time dt);
        virtual bool handleEvent(const sf::Event& event);
    private:
        sf::Sprite mBackgroundSprite;
        sf::Text mText;
        bool mShowText;
        sf::Time mTextEffectTime;
};

TitleState::TitleState(StateStack& stack, Context context) 
: State(stack, context), mText(), mShowText(true), mTextEffectTime(sf::Time::Zero) {
    mBackgroundSprite.setTexture(context.textures->get(Textures::TitleScreen));
    mText.setFont(context.fonts->get(Fonts::Sansation));
    mText.setString("Press any key to start");
    centerOrigin(mText);
    mText.setPosition(context.window->getView().getSize() / 2.f);
}

void TitleState::draw() {
    auto& window = *State::getContext().window;
    window.draw(mBackgroundSprite);
    if (mShowText)
        window.draw(mText);
}

bool TitleState::update(sf::Time dt) {
    mTextEffectTime += dt;
    if (mTextEffectTime >= sf::seconds(0.5f)) {
        mShowText = !mShowText;
        mTextEffectTime = sf::Time::Zero;
    }
    return true;
}

bool TitleState::handleEvent(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        requestStackPop();
        requestStackPush(States::Loading);
    }
    return true;
}



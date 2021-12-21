#pragma once

#include "States/StateStack.hpp"
#include "Utils/Utility.hpp"

class PauseState : public State {
    public:
        PauseState(StateStack& stack, Context context);
        virtual void draw();
        virtual bool update(sf::Time dt);
        virtual bool handleEvent(const sf::Event& event);
    private:
        sf::Sprite mBackgroundSprite;
        sf::Text mPauseText;
        sf::Text mInstructionText;
};

PauseState::PauseState(StateStack& stack, Context context) 
: State(stack, context), mBackgroundSprite(), mPauseText(), mInstructionText() {
    sf::Font& font = context.fonts->get(Fonts::Sansation);
    sf::Vector2f viewSize = context.window->getView().getSize();

    mPauseText.setFont(font);
    mPauseText.setString("Game Pause");
    mPauseText.setCharacterSize(70);
    centerOrigin(mPauseText);
    mPauseText.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);

    mInstructionText.setFont(font);
    mInstructionText.setString("(Press Backspace to return to the main menu)");
    centerOrigin(mInstructionText);
    mInstructionText.setPosition(0.5f * viewSize.x, 0.6f * viewSize.y);
}

void PauseState::draw() {
    sf::RenderWindow& window = *State::getContext().window;
    window.setView(window.getDefaultView());
    
    sf::RectangleShape backgroundShape;
    backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
    backgroundShape.setSize(window.getView().getSize());

    window.draw(backgroundShape);
    window.draw(mPauseText);
    window.draw(mInstructionText);
}

bool PauseState::update(sf::Time dt) {
    return false;
}

bool PauseState::handleEvent(const sf::Event& event) {
    if (event.type != sf::Event::KeyPressed)
        return false;
    if (event.key.code == sf::Keyboard::Escape) {
        requestStackPop();
    }
    if (event.key.code == sf::Keyboard::BackSpace) {
        requestStackClear();
        requestStackPush(States::Menu);
    }
    return false;
}

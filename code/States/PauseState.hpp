#pragma once

#include "States/StateStack.hpp"
#include "Utils/Utility.hpp"
#include "Components/Container.hpp"
#include "Components/Button.hpp"

class PauseState : public State {
    public:
        PauseState(StateStack& stack, Context context);
        ~PauseState();
        virtual void draw();
        virtual bool update(sf::Time dt);
        virtual bool handleEvent(const sf::Event& event);
    private:
        sf::Sprite mBackgroundSprite;
        sf::Text mPauseText;
        GUI::Container mGUIContainer;
};

PauseState::PauseState(StateStack& stack, Context context) 
: State(stack, context)
, mBackgroundSprite()
, mPauseText()
, mGUIContainer() {
    sf::Font& font = context.fonts->get(Fonts::Sansation);
    sf::Vector2f windowSize(context.window->getSize());

    mPauseText.setFont(font);
    mPauseText.setString("Game Pause");
    mPauseText.setCharacterSize(70);
    Utility::centerOrigin(mPauseText);
    mPauseText.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y);

    auto returnButton = std::make_shared<GUI::Button>(context);
    returnButton->setPosition(0.5f * windowSize.x - 100, 0.4f * windowSize.y + 75);
    returnButton->setText("Return");
    returnButton->setCallback([this] ()
    {
        State::requestStackPop();
    });

    auto backToMenuButton = std::make_shared<GUI::Button>(context);
    backToMenuButton->setPosition(0.5f * windowSize.x - 100, 0.4f * windowSize.y + 125);
    backToMenuButton->setText("Back to menu");
    backToMenuButton->setCallback([this] () {
        State::requestStackClear();
        State::requestStackPush(States::Menu);
    });
    
    mGUIContainer.pack(returnButton);
    mGUIContainer.pack(backToMenuButton);

    getContext().music->setPaused(true);
}

PauseState::~PauseState() {
    getContext().music->setPaused(false);
}

void PauseState::draw() {
    sf::RenderWindow& window = *State::getContext().window;
    window.setView(window.getDefaultView());
    
    sf::RectangleShape backgroundShape;
    backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
    backgroundShape.setSize(window.getView().getSize());

    window.draw(backgroundShape);
    window.draw(mPauseText);
    window.draw(mGUIContainer);
}

bool PauseState::update(sf::Time dt) {
    return false;
}

bool PauseState::handleEvent(const sf::Event& event) {
    mGUIContainer.handleEvent(event);
    return false;
}

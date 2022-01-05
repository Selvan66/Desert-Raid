#pragma once

#include "States/StateStack.hpp"
#include "Components/Container.hpp"
#include "Utils/Utility.hpp"

class GameOverState : public State {
    public:
        GameOverState(StateStack& stack, Context context);
        virtual void draw();
        virtual bool update(sf::Time dt);
        virtual bool handleEvent(const sf::Event& event);
    private:
        sf::Text mGameOverText;
        sf::Text mElapsedTime;
};

GameOverState::GameOverState(StateStack& stack, Context context) 
: State(stack, context), mGameOverText(), mElapsedTime(sf::Time::Zero) {
    sf::Font& font = context.fonts->get(Fonts::Sansation);
    sf::Vector2f windowSize(context.window->getSize());
    mGameOverText.setFont(font);
    if (context.player->getMissionStatus() == Player::MissionFailuer) {
        mGameOverText.setString("Mission failed!");
    }
    else {
        mGameOverText.setString("Mission successful!");
    }

    mGameOverText.setCharacterSize(70);
    centerOrigin(mGameOverText);
    mGameOverText.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y);
}

void GameOverState::draw() {
    sf::RenderWindow& window = *getContext().window;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(mGameOverText);
}

bool GameOverState::update(sf::Time dt) {
    mElapsedTime += dt;
    if (mElapsedTime > sf::seconds(3)) {
        requestStateClear();
        requestStackPush(States::Menu);
    }
    return false;
}

bool GameOverState::handleEvent(const sf::Event& event) {
    return false;
}

#pragma once

#include "States/StateStack.hpp"
#include "Objects/World.hpp"
#include "Game/Player.hpp"

class GameState : public State {
    public:
        GameState(StateStack& stack, Context context);
        virtual void draw();
        virtual bool update(sf::Time dt);
        virtual bool handleEvent(const sf::Event& event);
    private:
        World mWorld;
        Player& mPlayer;
};

GameState::GameState(StateStack& stack, Context context)
: State(stack, context), mWorld(*context.window, *context.fonts), mPlayer(*context.player) {
    mPlayer.setMissionStatus(Player::MissionRunning);
}

void GameState::draw() {
    mWorld.draw();
}

bool GameState::update(sf::Time dt) {
    mWorld.update(dt);

    if(!mWorld.hasAlivePlayer()) {
		mPlayer.setMissionStatus(Player::MissionFailure);
		requestStackPush(States::GameOver);
	}
	else if(mWorld.hasPlayerReachedEnd()) {
		mPlayer.setMissionStatus(Player::MissionSuccess);
		requestStackPush(States::GameOver);
	}

    CommandQueue& commands = mWorld.getCommandQueue();
    mPlayer.handleRealtimeInput(commands);

    return true;
}

bool GameState::handleEvent(const sf::Event& event) {
    CommandQueue& commands = mWorld.getCommandQueue();
    mPlayer.handleEvent(event, commands);
    if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        requestStackPush(States::Pause);
    return true;
}

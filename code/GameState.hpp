#pragma once

#include "StateStack.hpp"
#include "World.hpp"
#include "Player.hpp"

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
: State(stack, context), mWorld(*context.window), mPlayer(*context.player) {
}

void GameState::draw() {
    mWorld.draw();
}

bool GameState::update(sf::Time dt) {
    mWorld.update(dt);

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

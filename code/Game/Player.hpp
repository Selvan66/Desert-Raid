#pragma once

#include "Game/CommandQueue.hpp"
#include "Game/Aircraft.hpp"

#include <SFML/Window.hpp>

#include <map>

class Player {
    public:
        enum Action {
            MoveLeft,
            MoveRight,
            MoveUp,
            MoveDown,
            ActionCount
        };
    public:
        Player();
        void handleEvent(const sf::Event& event, CommandQueue& commands);
        void handleRealtimeInput(CommandQueue& commands);
        void assignKey(Action action, sf::Keyboard::Key key);
        sf::Keyboard::Key getAssignKey(Action action) const;
    private:
        void initializeActions();
        static bool isRealtimeAction(Action action);
    private:
        std::map<sf::Keyboard::Key, Action> mKeyBinding;
        std::map<Action, Command> mActionBinding;
};

struct AircraftMover {
    AircraftMover(float vx, float vy) : velocity(vx, vy) {
    }
    void operator() (Aircraft& aircraft, sf::Time) const {
        aircraft.accelerate(velocity);
    }

    sf::Vector2f velocity;
};

Player::Player() {
    mKeyBinding[sf::Keyboard::Left] = MoveLeft;
    mKeyBinding[sf::Keyboard::Right] = MoveRight;
    mKeyBinding[sf::Keyboard::Up] = MoveUp;
    mKeyBinding[sf::Keyboard::Down] = MoveDown;

    initializeActions();

    for (auto& pair : mActionBinding)
        pair.second.category = Category::PlayerAircraft;
}

void Player::handleEvent(const sf::Event& event, CommandQueue& commands) {
    if (event.type == sf::Event::KeyPressed) {
        auto found = mKeyBinding.find(event.key.code);
		if (found != mKeyBinding.end() && !isRealtimeAction(found->second))
			commands.push(mActionBinding[found->second]);
    }
}

void Player::handleRealtimeInput(CommandQueue& commands) {
    for (auto& pair : mKeyBinding) 
        if (sf::Keyboard::isKeyPressed(pair.first) && isRealtimeAction(pair.second))
            commands.push(mActionBinding[pair.second]);
}

void Player::assignKey(Action action, sf::Keyboard::Key key) {
    // erase_if()
    for (auto itr = mKeyBinding.begin(); itr != mKeyBinding.end();)
        if (itr->second == action)
            mKeyBinding.erase(itr++);
        else
            ++itr;

    mKeyBinding[key] = action;
}

sf::Keyboard::Key Player::getAssignKey(Action action) const {
    for (auto& pair : mKeyBinding) 
        if (pair.second == action)
            return pair.first;

    return sf::Keyboard::Unknown;
}

void Player::initializeActions() {
    const float playerSpeed = 200.f;

	mActionBinding[MoveLeft].action	 = derivedAction<Aircraft>(AircraftMover(-playerSpeed, 0.f));
	mActionBinding[MoveRight].action = derivedAction<Aircraft>(AircraftMover(+playerSpeed, 0.f));
	mActionBinding[MoveUp].action    = derivedAction<Aircraft>(AircraftMover(0.f, -playerSpeed));
	mActionBinding[MoveDown].action  = derivedAction<Aircraft>(AircraftMover(0.f, +playerSpeed));
}

bool Player::isRealtimeAction(Action action) {
    switch (action) {
		case MoveLeft:
		case MoveRight:
		case MoveDown:
		case MoveUp:
			return true;
		default:
			return false;
	}
}
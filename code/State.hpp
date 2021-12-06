#pragma once

class StateStack;

#include "StateStack.hpp"
#include "ResourceIdentifiers.hpp"
#include "Player.hpp"

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <memory>

class State {
    public:
        typedef std::unique_ptr<State> Ptr;
        struct Context {
            Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, Player& player);
            sf::RenderWindow* window;
            TextureHolder* textures;
            FontHolder* fonts;
            Player* player;
        };
    public:
        State(StateStack& stack, Context context);
        virtual ~State();
        virtual void draw() = 0;
        virtual bool update(sf::Time dt) = 0;
        virtual bool handleEvent(const sf::Event event) = 0;
    protected:
        void requestStackPush(States::ID stateID);
        void requestStackPop();
        void requestStackClear();
        Context getContext() const;
    private:
        StateStack* mStack;
        Context mContext;
};

State::Context::Context(sf::RenderWindow& window, TextureHolder& textures, FontHolder& fonts, Player& player) 
: window(&window), textures(&textures), fonts(&fonts), player(&player) {
}

State::State(StateStack& stack, Context context) 
: mStack(&stack), mContext(context) {
}

State::~State() {
}

void State::requestStackPush(States::ID stateID) {
    mStack->pushState(stateID);
}

void State::requestStackPop() {
    mStack->popState();
}

void State::requestStackClear() {
    mStack->clearStates();
}

Context State::getContext() const{
    return mContext;
}
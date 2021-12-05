#pragma once

class StateStack;

#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <memory>

class State {
    public:
        typedef std::unique_ptr<State> Ptr;
        struct Context {

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
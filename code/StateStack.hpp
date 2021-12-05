#pragma once

#include "State.hpp"

#include <vector>
#include <map>
#include <functional>

class StateStack : private sf::NonCopyable {
    public:
        enum Action {
            Push, 
            Pop,
            Clear
        };
    public:
        explicit StateStack(State::Context context);
        template<typename T>
        void registerState(States::ID stateID);
        void update(sf::Time dt);
        void draw();
        void handleEvent(const sf::Event& event);
        void pushState(States::ID);
        void popState();
        void clearStates();
        bool isEmpty() const;
    private:
        struct PendingChange {
            Action action;
            States::ID stateID;
        };
    private:
        std::vector<State::Ptr> mStack;
        std::vector<PendingChange> mPendingList;
        State::Context mContext;
        std::map<States::ID, std::function<State::Ptr()>> mFactories;
};
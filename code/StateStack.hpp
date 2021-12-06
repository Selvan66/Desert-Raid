#pragma once

#include "StateIdentifiers.hpp"

#include <vector>
#include <map>
#include <functional>

class State;

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
        State::Ptr createState(States::ID stateID);
        void applyPendingChanges();
    private:
        struct PendingChange {
            PendingChange(Action action, States::ID stateID = States::None);
            Action action;
            States::ID stateID;
        };
    private:
        std::vector<State::Ptr> mStack;
        std::vector<PendingChange> mPendingList;
        State::Context mContext;
        std::map<States::ID, std::function<State::Ptr()>> mFactories;
};

StateStack::StateStack(State::Context context) 
: mStack(), mPendingList(), mContext(context), mFactories() {
}

template<typename T>
void StateStack::registerState(States::ID stateID) {
    mFactories[stateID] = [this] {
        return State::Ptr(new T(*this, mContext));
    };
}

void StateStack::update(sf::Time dt) {
    for (auto stack : mStack)
        if (!stack->handleEvent(event))
            break;
    applyPendingChanges();
}

void StateStack::draw() {
    for (auto stack : mStack)
        stack->draw();
}

void StateStack::handleEvent(const sf::Event& event) {
    for (auto stack : mStack)
        if (!stack->handleEvent(event))
            break;
    applyPendingChanges();
}

void StateStack::pushState(States::ID) {
    mPendingList.push_back(PendingChange(Push, stateID));
}

void StateStack::popState() {
    mPendingList.push_back(PendingChagne(Pop));
}

void StateStack::clearStates() {
    mPendingList.push_back(PendingChange(Clear));
}

bool StateStack::isEmpty() const {
    return mStack.empty()
}

State::Ptr StateStack::createState(States::ID stateID) {
    auto found = mFactories.find(stateID);
    assert(found != mFactories.end());
    return found->second(); // Envoke function in factories which make and return new state
}

void StateStack::applyPendingChanges() {
    for (auto change : mPendingList) {
        switch(change.action) {
            case Push:
                mStack.push_back(createState(change.stateID));
                break;
            case Pop:
                mStack.pop_back();
                break;
            case Clear:
                mStack.clear();
                break;
        }
    }
    mPendingList.clear();
}

StateStack::PendingChange::PendingChange(Action action, States::ID stateID)
: action(action), stateID(stateID) {
}
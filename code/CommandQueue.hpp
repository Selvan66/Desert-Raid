#pragma once

#include "Command.hpp"

#include <queue>

class CommandQueue {
    public:
        void push(const Command& command);
        Command pop();
        bool isEmpty() const;
    private:
        std::queue<Command> mQueue;
};

void CommandQueue::push(const Command& command) {
    mQueue.push(command);
}

Command CommandQueue::pop() {
    Command command = mQueue.front();
    mQueue.pop();
    return command;
}

bool CommandQueue::isEmpty() const {
    return mQueue.empty();
}

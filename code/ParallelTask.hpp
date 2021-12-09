#pragma once

#include "SFML/System.hpp"

class ParallelTask {
    public:
        ParallelTask();
        void execute();
        bool isFinished();
        float getCompletion();
    private:
        void runTask();
    private:
        sf::Thread mThread;
        bool mFinished;
        sf::Clock mElapsedTime;
        sf::Mutex mMutex;
};

ParallelTask::ParallelTask() 
: mThread(&ParallelTask::runTask, this), mFinished(false){
}

void ParallelTask::execute() {
    mFinished = false;
    mElapsedTime.restart();
    mThread.launch();
}

bool ParallelTask::isFinished() {
    sf::Lock lock(mMutex);
    return mFinished;
}

float ParallelTask::getCompletion() {
    sf::Lock lock(mMutex);
    return mElapsedTime.getElapsedTime().asSeconds() / 10.f;
}

void ParallelTask::runTask() {
    bool ended = false;
    while (!ended) {
        sf::Lock lock(mMutex);
        if (mElapsedTime.getElapsedTime().asSeconds() >= 10.f)
            ended = true;
    }
    {
        sf::Lock lock(mMutex);
        mFinished = true;
    }
}

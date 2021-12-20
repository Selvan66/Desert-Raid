#pragma once

#include "StateStack.hpp"
#include "ParallelTask.hpp"

class LoadingState : public State {
    public:
        LoadingState(StateStack& stack, Context context);
        virtual void draw();
        virtual bool update(sf::Time dt);
        virtual bool handleEvent(const sf::Event& event);
        void setCompletion(float percent);
    private:
        sf::Text mLoadingText;
        sf::RectangleShape mProgressBarBackground;
        sf::RectangleShape mProgressBar;
        ParallelTask mLoadingTask;
};

LoadingState::LoadingState(StateStack& stack, Context context) 
: State(stack, context) {
    sf::RenderWindow& window = *State::getContext().window;
    sf::Font& font = context.fonts->get(Fonts::Sansation);
    sf::Vector2f viewSize = window.getView().getSize();

    mLoadingText.setFont(font);
    mLoadingText.setString("Loading Resources");
    centerOrigin(mLoadingText);
    mLoadingText.setPosition(viewSize.x / 2.f, viewSize.y / 2.f + 50.f);

    mProgressBarBackground.setFillColor(sf::Color::White);
    mProgressBarBackground.setSize(sf::Vector2f(viewSize.x - 20, 10));
    mProgressBarBackground.setPosition(10, mLoadingText.getPosition().y + 40);

    mProgressBar.setFillColor(sf::Color(100, 100, 100));
    mProgressBar.setSize(sf::Vector2f(200, 10));
    mProgressBar.setPosition(10, mLoadingText.getPosition().y + 40);

    setCompletion(0.f);

    mLoadingTask.execute();
}

void LoadingState::draw() {
    sf::RenderWindow& window = *State::getContext().window;

    window.setView(window.getDefaultView());
    window.draw(mLoadingText);
    window.draw(mProgressBarBackground);
    window.draw(mProgressBar);
}

bool LoadingState::update(sf::Time dt) {
    if (mLoadingTask.isFinished()) {
        requestStackPop();
        requestStackPush(States::Menu);
    }
    else {
        setCompletion(mLoadingTask.getCompletion());
    }
    return true;
}

bool LoadingState::handleEvent(const sf::Event& event) {
    return true;
}

void LoadingState::setCompletion(float percent) {
    if (percent > 1.f)
        percent = 1.f;
    mProgressBar.setSize(sf::Vector2f(mProgressBarBackground.getSize().x * percent, mProgressBar.getSize().y));
}

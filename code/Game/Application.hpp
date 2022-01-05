#pragma once

#include "States/StateStack.hpp"
#include "States/GameState.hpp"
#include "States/TitleState.hpp"
#include "States/MenuState.hpp"
#include "States/PauseState.hpp"
#include "States/LoadingState.hpp"
#include "States/SettingsState.hpp"
#include "States/GameOverState.hpp"

class Application {
    public:
        Application();
        void run();
    private:
        void processInput();
        void update(sf::Time dt);
        void render();
        void registerStates();
    private:
        sf::RenderWindow mWindow;
        TextureHolder mTextures;
        FontHolder mFonts;
        Player mPlayer;
        StateStack mStateStack;
};

Application::Application() 
: mWindow(sf::VideoMode(640, 480), "SFML - game", sf::Style::Close),
mTextures(), mFonts(), mPlayer(), mStateStack(State::Context(mWindow, mTextures, mFonts, mPlayer)) {   
    mWindow.setKeyRepeatEnabled(false);
    mFonts.load(Fonts::Sansation, "../assets/Sansation.ttf");
    mTextures.load(Textures::TitleScreen, "../assets/TitleScreen.png");
    mTextures.load(Textures::ButtonNormal, "../assets/ButtonNormal.png");
    mTextures.load(Textures::ButtonSelected, "../assets/ButtonSelected.png");
	mTextures.load(Textures::ButtonPressed,	"../assets/ButtonPressed.png");
    registerStates();
    mStateStack.pushState(States::Title);
}

void Application::run() {
    sf::Clock clock;
    const sf::Time TimePerFrame = sf::seconds(1.f/60.f);
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    while (mWindow.isOpen()) {
        sf::Time dt = clock.restart();
        timeSinceLastUpdate += dt;
        while (timeSinceLastUpdate > TimePerFrame) {
            timeSinceLastUpdate -= TimePerFrame;
            processInput();
            update(TimePerFrame);
            if (mStateStack.isEmpty())
                mWindow.close();
        }
        render();
    }
}

void Application::processInput() {
    sf::Event event;
    while (mWindow.pollEvent(event)) {
        mStateStack.handleEvent(event);
        if (event.type == sf::Event::Closed)
            mWindow.close();
    }
}

void Application::update(sf::Time dt) {
    mStateStack.update(dt);
}

void Application::render() {
    mWindow.clear();
    mStateStack.draw();
    mWindow.setView(mWindow.getDefaultView());
    mWindow.display();
}

void Application::registerStates() {
    mStateStack.registerState<TitleState>(States::Title);
    mStateStack.registerState<MenuState>(States::Menu);
    mStateStack.registerState<GameState>(States::Game);
    mStateStack.registerState<PauseState>(States::Pause);
    mStateStack.registerState<LoadingState>(States::Loading);
    mStateStack.registerState<SettingsState>(States::Settings);
    mStateStack.registerState<GameOverState>(States::GameOver);
}
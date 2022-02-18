#pragma once

#include "States/StateStack.hpp"
#include "Utils/Utility.hpp"
#include "Components/Button.hpp"
#include "Components/Container.hpp"
#include "Effects/MusicPlayer.hpp"

class MenuState : public State {
    public:
        MenuState(StateStack& stack, Context context);
        virtual void draw();
        virtual bool update(sf::Time dt);
        virtual bool handleEvent(const sf::Event& event);
    private:
        sf::Sprite mBackgroundSprite;
        GUI::Container mGUIContainer;
};

MenuState::MenuState(StateStack& stack, Context context) 
: State(stack, context), mGUIContainer() {
    sf::Texture& texture = context.textures->get(Textures::TitleScreen);
    mBackgroundSprite.setTexture(texture);

    auto playButton = std::make_shared<GUI::Button>(context);
    playButton->setPosition(100, 300);
	playButton->setText("Play");
	playButton->setCallback([this] ()
	{
		requestStackPop();
		requestStackPush(States::Loading);
	});

    auto settingsButton = std::make_shared<GUI::Button>(context);
    settingsButton->setPosition(100, 350);
    settingsButton->setText("Settings");
    settingsButton->setCallback([this](){ State::requestStackPush(States::Settings); });

    auto exitButton = std::make_shared<GUI::Button>(context);
	exitButton->setPosition(100, 400);
	exitButton->setText("Exit");
	exitButton->setCallback([this] ()
	{
		requestStackPop();
	});

    mGUIContainer.pack(playButton);
    mGUIContainer.pack(settingsButton);
    mGUIContainer.pack(exitButton);

    context.music->play(Music::MenuTheme);
}

void MenuState::draw() {
    sf::RenderWindow& window = *State::getContext().window;

    window.setView(window.getDefaultView());
    window.draw(mBackgroundSprite);
    window.draw(mGUIContainer);
}

bool MenuState::update(sf::Time dt) {
    return true;
}

bool MenuState::handleEvent(const sf::Event& event) {
    mGUIContainer.handleEvent(event);
    return false;
}



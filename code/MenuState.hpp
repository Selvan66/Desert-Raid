#pragma once

#include "StateStack.hpp"
#include "Utility.hpp"

class MenuState : public State {
    public:
        MenuState(StateStack& stack, Context context);
        virtual void draw();
        virtual bool update(sf::Time dt);
        virtual bool handleEvent(const sf::Event& event);
        void updateOptionText();
    private:
        enum OptionNames {
            Play,
            Exit
        };
    private:
        sf::Sprite mBackgroundSprite;
        std::vector<sf::Text> mOptions;
        std::size_t mOptionIndex;
};

MenuState::MenuState(StateStack& stack, Context context) 
: State(stack, context), mOptions(), mOptionIndex(0) {
    sf::Texture& texture = context.textures->get(Textures::TitleScreen);
    sf::Font& font = context.fonts->get(Fonts::Sansation);

    mBackgroundSprite.setTexture(texture);

    sf::Text playOption;
    playOption.setFont(font);
    playOption.setString("Play");
    centerOrigin(playOption);
    playOption.setPosition(context.window->getView().getSize()/2.f);
    mOptions.push_back(playOption);

    sf::Text exitOption;
    exitOption.setFont(font);
    exitOption.setString("Exit");
    centerOrigin(exitOption);
    exitOption.setPosition(playOption.getPosition() + sf::Vector2f(0.f, 30.f));
    mOptions.push_back(exitOption);

    updateOptionText();
}

void MenuState::draw() {
    sf::RenderWindow& window = *State::getContext().window;

    window.setView(window.getDefaultView());
    window.draw(mBackgroundSprite);

    for (auto& text : mOptions)
        window.draw(text);
}

bool MenuState::update(sf::Time dt) {
    return true;
}

bool MenuState::handleEvent(const sf::Event& event) {
    if (event.type != sf::Event::KeyPressed)
        return false;

    if (event.key.code == sf::Keyboard::Return) {
        if (mOptionIndex == Play) {
            requestStackPop();
            requestStackPush(States::Loading);
        }
        else {
            requestStackPop();
        }
    }
    else if (event.key.code == sf::Keyboard::Up) {
        if (mOptionIndex > 0)
            mOptionIndex--;
        else
            mOptionIndex = mOptions.size() - 1;
        updateOptionText();
    }
    else if (event.key.code == sf::Keyboard::Down) {
        if (mOptionIndex < mOptions.size() - 1)
            mOptionIndex++;
        else
            mOptionIndex = 0;
        updateOptionText();
    }
    return true;
}

void MenuState::updateOptionText() {
    if (mOptions.empty())
        return;
    for (auto& text : mOptions)
        text.setFillColor(sf::Color::White);
    mOptions[mOptionIndex].setFillColor(sf::Color::Red);
}

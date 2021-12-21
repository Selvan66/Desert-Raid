#pragma once

#include "States/StateStack.hpp"
#include "Game/Player.hpp"
#include "Components/Container.hpp"
#include "Components/Button.hpp"
#include "Components/Label.hpp"

#include <array>
#include <string>

class SettingsState : public State {
    public:
        SettingsState(StateStack& stack, Context context);
        virtual void draw();
        virtual bool update(sf::Time dt);
        virtual bool handleEvent(const sf::Event& event);
    private:
        void updateLabels();
        void addButtonLabel(Player::Action action, float y, const std::string& text, Context context);
    private:
        sf::Sprite mBackgroundSprite;
        GUI::Container mGUIContainer;
        std::array<GUI::Button::Ptr, Player::ActionCount> mBindingButtons;
        std::array<GUI::Label::Ptr, Player::ActionCount> mBindingLabels;
};

SettingsState::SettingsState(StateStack& stack, Context context) 
: State(stack, context), mGUIContainer() {
    mBackgroundSprite.setTexture(context.textures->get(Textures::TitleScreen));

    addButtonLabel(Player::MoveLeft, 150.f, "Move Left", context);
    addButtonLabel(Player::MoveRight, 200.f, "Move Right", context);
    addButtonLabel(Player::MoveUp, 250.f, "Move Up", context);
    addButtonLabel(Player::MoveDown, 300.f, "Move Down", context);
    updateLabels();

    auto backButton = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
    backButton->setPosition(80.f, 375.f);
    backButton->setText("Back");
    backButton->setCallback(std::bind(&SettingsState::requestStackPop, this));

    mGUIContainer.pack(backButton);
}

void SettingsState::draw() {
    sf::RenderWindow& window = *State::getContext().window;
    window.draw(mBackgroundSprite);
    window.draw(mGUIContainer);
}

bool SettingsState::update(sf::Time dt) {
    return true;
}

bool SettingsState::handleEvent(const sf::Event& event) {
    bool isKeyBinding = false;

    for (std::size_t action = 0; action < Player::ActionCount; ++action) {
        if (mBindingButtons[action]->isActive()) {
            isKeyBinding = true;
            if (event.type == sf::Event::KeyReleased) {
                State::getContext().player->assignKey(static_cast<Player::Action>(action), event.key.code);
                mBindingButtons[action]->deactivate();
            }
            break;
        }
    }

    if (isKeyBinding) {
        updateLabels();
    }
    else {
        mGUIContainer.handleEvent(event);
    }
    
    return false;
}

void SettingsState::updateLabels() {
    Player& player = *State::getContext().player;
    for (std::size_t i = 0; i < Player::ActionCount; ++i) {
        sf::Keyboard::Key key = player.getAssignKey(static_cast<Player::Action>(i));
        mBindingLabels[i]->setText(toString(key));
    }
}

void SettingsState::addButtonLabel(Player::Action action, float y, const std::string& text, Context context) {
    mBindingButtons[action] = std::make_shared<GUI::Button>(*context.fonts, *context.textures);
    mBindingButtons[action]->setPosition(80.f, y);
    mBindingButtons[action]->setText(text);
    mBindingButtons[action]->setToggle(true);

    mBindingLabels[action] = std::make_shared<GUI::Label>("", *context.fonts);
    mBindingLabels[action]->setPosition(300.f, y + 15.f);

    mGUIContainer.pack(mBindingButtons[action]);
    mGUIContainer.pack(mBindingLabels[action]);
}



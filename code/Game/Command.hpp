#pragma once

#include "Game/Category.hpp"

#include <SFML/System.hpp>

#include <functional>
#include <cassert>

class SceneNode;

struct Command {
    Command();
    std::function<void(SceneNode&, sf::Time)> action;
    unsigned int category;
};

Command::Command() : action(), category(Category::None) {
}

template <typename GameObject, typename Function>
std::function<void(SceneNode&, sf::Time)> derivedAction(Function fn) {
    return [=] (SceneNode& node, sf::Time dt) {
        assert(dynamic_cast<GameObject*>(&node) != nullptr);
        fn(static_cast<GameObject&>(node), dt);
    };
}
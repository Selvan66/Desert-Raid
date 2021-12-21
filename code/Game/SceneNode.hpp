#pragma once

#include "Game/Category.hpp"
#include "Game/Command.hpp"

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include <vector>
#include <memory>
#include <algorithm>
#include <cassert>

class SceneNode : public sf::Transformable, public sf::Drawable, public sf::NonCopyable {
    public:
        typedef std::unique_ptr<SceneNode> Ptr;
    public:
        SceneNode();
        void attachChild(Ptr child);
        Ptr detachChild(const SceneNode& node);
        void update(sf::Time dt);
        sf::Vector2f getWorldPosition() const;
        sf::Transform getWorldTransform() const;
        void onCommand(const Command& command, sf::Time dt);
        virtual unsigned int getCategory() const;
    private:
        virtual void updateCurrent(sf::Time dt);
        void updateChildren(sf::Time dt);
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
        void drawChildren(sf::RenderTarget& target, sf::RenderStates states) const;
    private:
        std::vector<Ptr> mChildren;
        SceneNode* mParent;
};

SceneNode::SceneNode() : mChildren(), mParent(nullptr) {
}

void SceneNode::attachChild(Ptr child) {
    child->mParent = this;
    mChildren.push_back(std::move(child));
}

SceneNode::Ptr SceneNode::detachChild(const SceneNode& node) {
    auto found = std::find_if(mChildren.begin(), mChildren.end(), [&](Ptr& p){ return p.get() == &node; });
    assert(found != mChildren.end());

    Ptr result = std::move(*found);
    result->mParent = nullptr;
    mChildren.erase(found);
    return result;    
}

void SceneNode::update(sf::Time dt) {
    updateCurrent(dt);
    updateChildren(dt);
}

sf::Vector2f SceneNode::getWorldPosition() const {
    return getWorldTransform() * sf::Vector2f();
}

sf::Transform SceneNode::getWorldTransform() const {
    sf::Transform transform = sf::Transform::Identity;

    for (const SceneNode* node = this; node != nullptr; node = node->mParent)
        transform = node->getTransform() * transform;

    return transform;
}

void SceneNode::onCommand(const Command& command, sf::Time dt) {
    if (command.category & getCategory())
        command.action(*this, dt);

    for (auto& child : mChildren)
        child->onCommand(command, dt);
}

unsigned int SceneNode::getCategory() const {
    return Category::Scene;
}

void SceneNode::updateCurrent(sf::Time dt) 
{
    // Do nothing by default
}

void SceneNode::updateChildren(sf::Time dt) {
    for (auto& child : mChildren)
        child->update(dt);
}

void SceneNode::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    drawCurrent(target, states);
    drawChildren(target, states);
}

void SceneNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
    // Do nothing by default
}

void SceneNode::drawChildren(sf::RenderTarget& target, sf::RenderStates states) const {
    for (auto& child : mChildren)
        child->draw(target, states);
}


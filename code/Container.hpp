#pragma once

#include "Component.hpp"

#include <vector>
#include <memory>

namespace GUI {
class Container : public Component {
    public:
        typedef std::shared_ptr<Container> Ptr;
    public:
        Container();
        void pack(Component::Ptr component);
        virtual bool isSelectable() const;
        virtual void handleEvent(const sf::Event& event);
    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
        bool hasSelection() const;
        void select(std::size_t index);
        void selectNext();
        void selectPrevious();
    private:
        std::vector<Component::Ptr> mChildren;
        int mSelectedChild;
};

Container::Container() 
: mChildren(), mSelectedChild(-1) {
}

void Container::pack(Component::Ptr component) {
    mChildren.push_back(component);
    if (!hasSelection() && component->isSelectable())
        select(mChildren.size() - 1);
}

bool Container::isSelectable() const {
    return false;
}

void Container::handleEvent(const sf::Event& event) {
    if (hasSelection() && mChildren[mSelectedChild]->isActive()) {
        mChildren[mSelectedChild]->handleEvent(event);
    }
    else if (event.type == sf::Event::KeyReleased) {
        if (event.key.code == sf::Keyboard::W || event.key.code == sf::Keyboard::Up) {
            selectPrevious();
        }
        else if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down) {
            selectNext();
        }
        else if (event.key.code == sf::Keyboard::Return || event.key.code == sf::Keyboard::Space) {
            if (hasSelection())
                mChildren[mSelectedChild]->activate();
        }
    }
}

void Container::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= Transformable::getTransform();
    for (const auto& child : mChildren) {
        target.draw(*child);
    }
}

bool Container::hasSelection() const{
    return mSelectedChild >= 0;
}

void Container::select(std::size_t index) {
    if (mChildren[index]->isSelectable()) {
        if (hasSelection()) {
            mChildren[mSelectedChild]->deselect();
        }
        mChildren[index]->select();
        mSelectedChild = index;
    }
}

void Container::selectNext() {
    if (!hasSeletion())
        return;
    int next = mSelectedChild;
    do {
        next = (next + 1) % mChildren.size();
    } while(!mChildren[next]->isSelectable());
    select(next);
}

void Container::selectPrevious() {
    if (!hasSeletion())
        return;
    int prev = mSelectedChild;
    do {
        prev = (prev + 1) % mChildren.size();
    } while(!mChildren[prev]->isSelectable());
    select(prev);
}



}

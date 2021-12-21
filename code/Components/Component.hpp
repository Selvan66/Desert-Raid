#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"

#include <memory>

namespace GUI {
class Component : public sf::Drawable, public sf::Transformable, private sf::NonCopyable {
    public:
        typedef std::shared_ptr<Component> Ptr;
    public:
        Component();
        virtual bool isSelectable() const = 0;
        bool isSelected() const;
        virtual void select();
        virtual void deselect();
        virtual bool isActive() const;
        virtual void activate();
        virtual void deactivate();
        virtual void handleEvent(const sf::Event& event) = 0;
    private:
        bool mIsSelected;
        bool mIsActive;
};

Component::Component() 
: mIsSelected(false), mIsActive(false) { 
}

bool Component::isSelected() const {
    return mIsSelected;
}

void Component::select() {
    mIsSelected = true;
}

void Component::deselect() {
    mIsSelected = false;
}

bool Component::isActive() const {
    return mIsActive;
}

void Component::activate() {
    mIsActive = true;
}

void Component::deactivate() {
    mIsActive = false;
}
}


#pragma once

#include "Game/SceneNode.hpp"

class Entity : public SceneNode {
    public:
        void setVelocity(sf::Vector2f velocity);
        void setVelocity(float vx, float vy);
        void accelerate(sf::Vector2f velocity);
        void accelerate(float vx, float vy);
        sf::Vector2f getVelocity() const;
    private:
        virtual void updateCurrent(sf::Time dt);
    private:
        sf::Vector2f mVelocity;
};

void Entity::setVelocity(sf::Vector2f velocity) {
    mVelocity = velocity;
}

void Entity::setVelocity(float vx, float vy) {
    mVelocity = sf::Vector2f(vx, vy);
}

void Entity::accelerate(sf::Vector2f velocity) {
    mVelocity += velocity;
}

void Entity::accelerate(float vx, float vy) {
    mVelocity.x += vx;
    mVelocity.y += vy;
}

sf::Vector2f Entity::getVelocity() const {
    return mVelocity;
}

void Entity::updateCurrent(sf::Time dt) {
    sf::Transformable::move(mVelocity * dt.asSeconds());
}

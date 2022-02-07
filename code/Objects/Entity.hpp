#pragma once

#include "Objects/SceneNode.hpp"

class Entity : public SceneNode {
    public:
        explicit Entity(int hitpoints);
        void setVelocity(sf::Vector2f velocity);
        void setVelocity(float vx, float vy);
        void accelerate(sf::Vector2f velocity);
        void accelerate(float vx, float vy);
        sf::Vector2f getVelocity() const;
        int getHitpoints() const;
        void repair(int points);
        void damage(int points);
        void destroy();
        virtual void remove();
        virtual bool isDestroyed() const;
    protected:
        virtual void updateCurrent(sf::Time dt, CommandQueue&);
    private:
        sf::Vector2f mVelocity;
        int mHitpoints;
};

Entity::Entity(int hitpoints) 
: mVelocity(), mHitpoints(hitpoints) {
}

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

int Entity::getHitpoints() const {
    return mHitpoints;
}

void Entity::repair(int points) {
    assert(points > 0);
    mHitpoints += points;
}

void Entity::damage(int points) {
    assert(points > 0);
    mHitpoints -= points;
}

void Entity::destroy() {
    mHitpoints = 0;
}

void Entity::remove() {
    destroy();
}

bool Entity::isDestroyed() const {
    return mHitpoints <= 0;
}

void Entity::updateCurrent(sf::Time dt, CommandQueue&) {
    sf::Transformable::move(mVelocity * dt.asSeconds());
}

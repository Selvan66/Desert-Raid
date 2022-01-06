#pragma once

#include "Game/Command.hpp"
#include "Utils/ResourceIdentifiers.hpp"
#include "Utils/Utility.hpp"

#include <functional>

using namespace std::placeholders;

class Aircraft;

struct PickupData {
    std::function<void(Aircraft&)> action;
    Textures::ID texture;
};

std::vector<PickupData> initializePickupData();

const std::vector<PickupData> PickupTable = initializePickupData();

class Pickup : public Entity {
    public:
        enum Type {
            HealthRefill,
            MissileRefill,
            FireSpread,
            FireRate,
            TypeCount
        };
    public:
        Pickup(Type type, const TextureHolder& textures);
        virtual unsigned int getCategory() const;
        virtual sf::FloatRect getBoundingRect() const;
        void apply(Aircraft& player) const;
    protected:
        virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
    private:
        Type mType;
        sf::Sprite mSprite;
};



Pickup::Pickup(Type type, const TextureHolder& textures) 
: Entity(1), mType(type), mSprite(textures.get(PickupTable[type].texture)) {
    Utility::centerOrigin(mSprite);
}

unsigned int Pickup::getCategory() const {
    return Category::Pickup;
}

sf::FloatRect Pickup::getBoundingRect() const {
    return SceneNode::getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

void Pickup::apply(Aircraft& player) const {
    PickupTable[mType].action(player);
}

void Pickup::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(mSprite, states);
}


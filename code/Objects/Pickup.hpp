#pragma once

#include "Objects/Aircraft.hpp"
#include "Game/Command.hpp"
#include "Utils/ResourceIdentifiers.hpp"

#include <functional>

using namespace std::placeholders;

struct PickupData {
    std::function<void(Aircraft&)> action;
    Textures::ID texture;
};

std::vector<PickupData> initializePickupData();

namespace {
    const std::vector<PickupData> Table = initializePickupData();
}

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

std::vector<PickupData> initializePickupData() {
    std::vector<PickupData> data(Pickup::TypeCount);
    
    data[Pickup::HealthRefill].texture = Textures::HealthRefill;
    data[Pickup::HealthRefill].action = [] (Aircraft& a) { a.repair(25); };

    data[Pickup::MissileRefill].texture = Textures::MissileRefill;
    data[Pickup::MissileRefill].action = std::bind(&Aircraft::collectMissiles, _1, 3);

    data[Pickup::FireSpread].texture = Textures::FireSpread;
    data[Pickup::FireSpread].action = std::bind(&Aircraft::increaseSpread, _1);

    data[Pickup::FireRate].texture = Textures::FireRate;
    data[Pickup::FireRate].action = std::bind(&Aircraft::increaseFireRate, _1);

    return data;
}

Pickup::Pickup(Type type, const TextureHolder& textures) 
: Entity(1), mType(type), mSprite(textures.get(Table[type].texture)) {
    centerOrigin(mSprite);
}

unsigned int Pickup::getCategory() const {
    return Category::Pickup;
}

sf::FloatRect Pickup::getBoundingRect() const {
    return SceneNode::getWorldTranform().tranformRect(mSprite.getGlobalBounds());
}

void Pickup::apply(Aircraft& player) const {
    Table[mType].action(player);
}

void Pickup::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(mSprite, states);
}


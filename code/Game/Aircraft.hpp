#pragma once

#include "Game/Entity.hpp"
#include "Utils/ResourceIdentifiers.hpp"

#include <vector>
#include <functional>

struct Direction {
    Direction(float angle, float distance)
    : angle(angle), distance(distance) {
    }

    float angle;
    float distance;
};

struct AircraftData {
    int hitpoints;
    float speed;
    Textures::ID texture;
    sf::Time fireInterval;
    std::vector<Direction> directions;
};

std::vector<AircraftData> initializeAircraftData();
class Aircraft : public Entity {
    public:
        enum Type {
            Eagle,
            Raptor,
            Avenger,
            TypeCount,
        };
    public:
        Aircraft(Type type, const TextureHolder& textures);
        virtual unsigned int getCategory() const;
        virtual sf::FloatRect getBoundingRect() const;
        virtual bool isMarkedForRemoval() const;
        bool isAllied() const;
        float getMaxSpeed() const;
        void increaseFireRate();
        void increaseSpreed();
        void collectMissiles(unsigned int count);
        void fire();
        void launchMissile();
    private:
        virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
        void updateMovementPattern(sf::Time dt);
        void checkPickupDrop(CommandQueue& commands);
        void checkProjectileLauch(sf::Time dt, CommandQueue& commands);
        void createBullets(SceneNode& node, const TextureHolder& textures) const;
        void createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const;
        void createPickup(SceneNode& node, const TextureHolder& textures) const;
        void updateTexts();
    private:
        Type mType;
        sf::Sprite mSprite;
        Command mFireCommand;
        Command mMissileCommand;
        sf::Time mFireCountdown;
        bool mIsFiring;
        bool mIsLauchingMissile;
        bool mIsMarkedForRemoval;
        int mFireRateLevel;
        int mSpreadLevel;
        int mMissileAmmo;
        Command mDropPickupCommand;
        float mTravelledDistance;
        std::size_t mDirectionIndex;
        TextNode* mHealthDisplay;
        TextNode* mMissileDisplay;
};

Textures::ID toTextureID(Aircraft::Type type) {
    switch (type) {
        case Aircraft::Eagle:
            return Textures::Eagle;
        case Aircraft::Raptor:
            return Textures::Raptor;
    }
    return Textures::Eagle;
}

std::vector<AircraftData> initializeAircraftData() {
    std::vector<AircraftData> data(Aircraft::TypeCount);

    data[Aircraft::Eagle].hitpoints = 100;
    data[Aircraft::Eagle].speed = 200.f;
    data[Aircraft::Eagle].fireInterval = sf::seconds(1);
    data[Aircraft::Eagle].texture = Textures::Eagle;

    data[Aircraft::Raptor].hitpoints = 20;
    data[Aircraft::Raptor].speed = 80.f;
    data[Aircraft::Raptor].texture = Textures::Raptor;
    data[Aircraft::Raptor].directions.emplace_back(45.f, 80.f);
    data[Aircraft::Raptor].directions.emplace_back(-45.f, 160.f);
    data[Aircraft::Raptor].directions.emplace_back(45.f, 80.f);
    data[Aircraft::Raptor].fireInterval = sf::Time::Zero;

    data[Aircraft::Avenger].hitpoints = 40;
    data[Aircraft::Avenger].speed = 50.f;
    data[Aircraft::Avenger].texture = Textures::Avenger;
    data[Aircraft::Avenger].direction.emplace_back(45.f, 50.f);
    data[Aircraft::Avenger].direction.emplace_back(0.f, 50.f);
    data[Aircraft::Avenger].direction.emplace_back(-45.f, 100.f);
    data[Aircraft::Avenger].direction.emplace_back(0, 50.f);
    data[Aircraft::Avenger].direction.emplace_back(45.f, 50.f);
    data[Aircraft::Avenger].fireInterval = sf::seconds(2);

    return data;
}

Aircraft::Aircraft(Type type, const TextureHolder& textures) 
: mType(type), mSprite(textures.get(toTextureID(type))) {
    sf::FloatRect bounds = mSprite.getLocalBounds();
    mSprite.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
}

unsigned int Aircraft::getCategory() const {
    switch (mType) {
		case Eagle:
			return Category::PlayerAircraft;
		default:
			return Category::EnemyAircraft;
	}
}

void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(mSprite, states);
}

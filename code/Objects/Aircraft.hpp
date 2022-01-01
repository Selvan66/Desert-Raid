#pragma once

#include "Objects/Entity.hpp"
#include "Objects/TextNode.hpp"
#include "Utils/ResourceIdentifiers.hpp"
#include "Utils/Utility.hpp"

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

namespace {
    const std::vector<AircraftData> Table = initializeAircraftData();
}

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

Aircraft::Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts)
: Entity(Table[type].hitpoints), mType(type), mSprite(textures.get(Table[type].texture)),
mFireCommand(), mMissileCommand(), mFireCountdown(sf::Time::Zero), mIsFiring(false), mIsLaunchingMissile(false),
mIsMarkedForRemoval(false), mFireRateLevel(1), mSpreadLevel(1), mMissileAmmo(2), mDropPickipCommand(), 
mTravelledDistance(0.f), mDirectionIndex(0), mHealthDisplay(nullptr), mMissileDisplay(nullptr) {
    centerOrigin(mSprite);
    mFireCommand.category = Category::SceneAirLayer;
    mFireCommand.action = [this, &texures] (SceneNode& node, sf::Time) {
        createBullets(node, textures);
    };

    mMissileCommand.category = Category::SceneAirLayer;
    mMissileCommand.action = [this, &textures] (SceneNode& node, sf::Time) {
        createProjectile(node, Projectile::Missile, 0.f, 0.5f, textures);
    };

    mDropPickupCommand.category = Category::SceneAirLayer;
    mDropPickupCommand.action = [this, &textures] (SceneNode& node, sf::Time) {
        createPickup(node, textures);
    };

    std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
    mHealthDisplay = healthDisplay.get();
    SceneNode::attachChild(std::move(healthDisplay));

    if (getCategory() == Category::PlayerAircraft) {
        std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
        missileDisplay->setPosition(0, 70);
        mMissileDisplay = missileDisplay.get();
        SceneNode::attachChild(std::move(missileDisplay));
    }

    updateText();
}

void Aircraft::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(mSprite, states);
}

void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands) {
    if (isDestroyed()) {
        checkPickupDrop(commands);
        mIsMarkedForRemoval = true;
        return;
    }
    checkProjectileLaunch(dt, commands);
    updateMovementPattern(dt);
    Enitity::updateCurrent(dt, commands);
    updateText();
}

unsigned int Aircraft::getCategory() const {
    if (isAllied()) {
        return Category::PlayerAircraft;
    }
    else {
        return Category::EnemyAircraft;
    }
}

sf::FloatRect Aircraft::getBoundingRect() const {
    return SceneNode::getWorldTransform().tranformRect(mSprite.getGlobalBounds());
}

bool Aircraft::isMarkedForRemoval() const {
    return mIsMarkedForRemoval;
}

bool Aircraft::isAllied() const {
    return mType == Eagle;
}

float Aircraft::getMaxSpeed() const {
    return Table[mType].speed;
}

void Aircraft:increaseFireRate() {
    if (mFireRateLevel < 10) {
        ++mFireRateLevel;
    }
}

void Aircraft::increaseSpread() {
    if (mSpreadLevel < 3) {
        ++SpreadLevel;
    }
}

void Aircraft::collectMissiles(unsigned int count) {
    mMissileAmmo += count;
}

void Aircraft::fire() {
    if (Table[mType].fireInterval != sf::Time::Zero) {
        mIsFiring = true;
    }
}

void Aircraft::launchMissile() {
    if (mMissileAmmo > 0) {
        mIsLauchingMissile = true;
        --mMissileAmmo;
    }
}

void Aircraft::updateMovementPattern(sf::Time dt) {
    const std::vector<Direction>& directions = Table[mType].directions;

    if (!directions.empty()) {
        if (mTravelledDistance > directions[mDirectionIndex].distance) {
            mDirectionIndex = (mDirectionIndex + 1) % directions.size();
            mTravelledDistance = 0.f;
        }

        float radians = toRadian(directions[mDirectionIndex].angle + 90.f);
        float vx = getMaxSpeed() * std::cos(radians);
        float vy = getMaxSpeed() * std::sin(radians);

        Entity::setVelocity(vx, vy);
        mTravelledDistance += getMaxSpeed() * dt.asSeconds();
    }
}

void Aircraft::checkPickupDrop(CommandQueue& commands) {
    if ((!isAllied()) && (randomInt(3) == 0)) {
        command.push(mDropPickupCommand);
    }
}

void Aircraft::checkProjectileLaunch(sf::Time dt, CommandQueue& commands) {
    if (!isAllied()) {
        fire();
    }
    
    if (mIsFiring && (mFireCountdown <= sf::Time::Zero)) {
        commands.push(mFireCommand);
        mFireCountdown += Table[mType].fireInterval / (mFireRateLevel + 1.f);
        mIsFiring = false;
    }
    else if (mFireCountdown > sf::Time::Zero) {
        mFireCountdown -= dt;
        mIsFiring = false;
    }

    if (mIsLaunchingMissile) {
        commands.push(mMissileCommand);
        mIsLaunchingMissile = false;
    }
}

void Aircraft::createBullets(SceneNode& node, const TextureHolder& textures) const {
    Projectile::Type type = isAllied() ? Projectile::AlliedBullet : Projectile::EnemyBullet;

    switch (mSpreadLevel) {
        case 1:
            createProjectile(node, type, 0.0f, 0.5f, textures);
            break;
        case 2:
            createProjectile(node, type, -0.33, 0.33f, textures);
            createProjectile(node, type, 0.33, 0.33f, textures);
            break;
        case 3:
            createProjectile(node, type, -0.5f, 0.33f, textures);
            createProjectile(node, type, 0.0f, 0.5f, textures);
            createProjectile(node, type, 0.5f, 0.33, textures);
            break;
    }
}

void Aircraft::createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const {
    std::unique_ptr<Projectile> projectile(new Projectile(type, textures));

    sf::Vector2f offset(xOffset * mSprite.getGlobalBounds().width, yOffset * mSprite.getGlobalBounds().height);
    sf::Vector2f velocity(0, projectile->getMaxSpeed());

    float sign = isAllied() ? -1.f : 1.f;
    projectile->setPosition(SceneNode::getWorldPosition() + offset * sign);
    projectile->setVelocity(velocity * sign);
    node.attachChild(std::move(projectile));
}

void Aircraft::createPickup(SceneNode& node, const TextureHolder& textures) const {
    auto type = static_cast<Pickup::Type>(randomInt(Pickup::TypeCount));

    std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
    pickup->setPosition(SceneNode::getWorldPosition());
    pickup->setVelocity(0.f, 1.f);
    node.attachChild(std::move(pickup));
}

void Aircraft::updateTexts() {
    mHealthDisplay->setString(std::to_string(Entity::getHitpoints()) + " HP");
    mHealthDisplay->setPosition(0.f, 50.f);
    mHealthDisplay->setRotation(-Tranformable::getRotation());
    if (mMissileDisplay) {
        if (mMissileAmmo == 0) {
            mMissileDisplay->setString("");
        }
        else {
            mMissileDisplay->setString("M: " + std::to_string(mMissileAmmo));
        }
    }
}
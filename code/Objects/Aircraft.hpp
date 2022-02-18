#pragma once

#include "Objects/Entity.hpp"
#include "Objects/TextNode.hpp"
#include "Objects/Projectile.hpp"
#include "Objects/Pickup.hpp"
#include "Objects/SoundNode.hpp"
#include "Effects/Animation.hpp"
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
    sf::IntRect textureRect;
    sf::Time fireInterval;
    std::vector<Direction> directions;
    bool hasRollAnimation;
};

std::vector<AircraftData> initializeAircraftData();

const std::vector<AircraftData> AircraftTable = initializeAircraftData();

class Aircraft : public Entity {
    public:
        enum Type {
            Eagle,
            Raptor,
            Avenger,
            TypeCount,
        };
    public:
        Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts);
        virtual unsigned int getCategory() const;
        virtual sf::FloatRect getBoundingRect() const;
        virtual void remove();
        virtual bool isMarkedForRemoval() const;
        bool isAllied() const;
        float getMaxSpeed() const;
        void increaseFireRate();
        void increaseSpread();
        void collectMissiles(unsigned int count);
        void fire();
        void launchMissile();
        void playLocalSound(CommandQueue& commands, SoundEffect::ID effect);
    private:
        virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
        virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
        void updateMovementPattern(sf::Time dt);
        void checkPickupDrop(CommandQueue& commands);
        void checkProjectileLaunch(sf::Time dt, CommandQueue& commands);
        void createBullets(SceneNode& node, const TextureHolder& textures) const;
        void createProjectile(SceneNode& node, Projectile::Type type, float xOffset, float yOffset, const TextureHolder& textures) const;
        void createPickup(SceneNode& node, const TextureHolder& textures) const;
        void updateText();
        void updateRollAnimation();
    private:
        Type mType;
        sf::Sprite mSprite;
        Animation mExplosion;
        Command mFireCommand;
        Command mMissileCommand;
        sf::Time mFireCountdown;
        bool mIsFiring;
        bool mIsLaunchingMissile;
        bool mShowExplosion;
        bool mPlayedExplosionSound;
        bool mSpawnedPickup;
        int mFireRateLevel;
        int mSpreadLevel;
        int mMissileAmmo;
        Command mDropPickupCommand;
        float mTravelledDistance;
        std::size_t mDirectionIndex;
        TextNode* mHealthDisplay;
        TextNode* mMissileDisplay;
};

std::vector<AircraftData> initializeAircraftData() {
    std::vector<AircraftData> data(Aircraft::TypeCount);

    data[Aircraft::Eagle].hitpoints = 100;
    data[Aircraft::Eagle].speed = 200.f;
    data[Aircraft::Eagle].fireInterval = sf::seconds(1);
    data[Aircraft::Eagle].texture = Textures::Entities;
    data[Aircraft::Eagle].textureRect = sf::IntRect(0, 0, 48, 64);
    data[Aircraft::Eagle].hasRollAnimation = true;

    data[Aircraft::Raptor].hitpoints = 20;
    data[Aircraft::Raptor].speed = 80.f;
    data[Aircraft::Raptor].texture = Textures::Entities;
    data[Aircraft::Raptor].textureRect = sf::IntRect(144, 0, 84, 64);
    data[Aircraft::Raptor].directions.emplace_back(45.f, 80.f);
    data[Aircraft::Raptor].directions.emplace_back(-45.f, 160.f);
    data[Aircraft::Raptor].directions.emplace_back(45.f, 80.f);
    data[Aircraft::Raptor].fireInterval = sf::Time::Zero;
    data[Aircraft::Raptor].hasRollAnimation = false;

    data[Aircraft::Avenger].hitpoints = 40;
    data[Aircraft::Avenger].speed = 50.f;
    data[Aircraft::Avenger].texture = Textures::Entities;
    data[Aircraft::Avenger].textureRect = sf::IntRect(228, 0, 60, 59);
    data[Aircraft::Avenger].directions.emplace_back(45.f, 50.f);
    data[Aircraft::Avenger].directions.emplace_back(0.f, 50.f);
    data[Aircraft::Avenger].directions.emplace_back(-45.f, 100.f);
    data[Aircraft::Avenger].directions.emplace_back(0.f, 50.f);
    data[Aircraft::Avenger].directions.emplace_back(45.f, 50.f);
    data[Aircraft::Avenger].fireInterval = sf::seconds(2);
    data[Aircraft::Avenger].hasRollAnimation = false;

    return data;
}

// Must be here
std::vector<PickupData> initializePickupData() {
    std::vector<PickupData> data(Pickup::TypeCount);
    
    data[Pickup::HealthRefill].texture = Textures::Entities;
    data[Pickup::HealthRefill].textureRect = sf::IntRect(0, 64, 40, 40);
    data[Pickup::HealthRefill].action = [] (Aircraft& a) { a.repair(25); };

    data[Pickup::MissileRefill].texture = Textures::Entities;
    data[Pickup::MissileRefill].textureRect = sf::IntRect(40, 64, 40, 40);
    data[Pickup::MissileRefill].action = std::bind(&Aircraft::collectMissiles, _1, 3);

    data[Pickup::FireSpread].texture = Textures::Entities;
    data[Pickup::FireSpread].textureRect = sf::IntRect(80, 64, 40, 40);
    data[Pickup::FireSpread].action = std::bind(&Aircraft::increaseSpread, _1);

    data[Pickup::FireRate].texture = Textures::Entities;
    data[Pickup::FireRate].textureRect = sf::IntRect(120, 64, 40, 40);
    data[Pickup::FireRate].action = std::bind(&Aircraft::increaseFireRate, _1);

    return data;
}

Aircraft::Aircraft(Type type, const TextureHolder& textures, const FontHolder& fonts)
: Entity(AircraftTable[type].hitpoints), 
mType(type), 
mSprite(textures.get(AircraftTable[type].texture), AircraftTable[type].textureRect),
mExplosion(textures.get(Textures::Explosion)),
mFireCommand(), 
mMissileCommand(), 
mFireCountdown(sf::Time::Zero), 
mIsFiring(false), 
mIsLaunchingMissile(false),
mShowExplosion(true),
mPlayedExplosionSound(false),
mSpawnedPickup(false), 
mFireRateLevel(1), 
mSpreadLevel(1), 
mMissileAmmo(2), 
mDropPickupCommand(), 
mTravelledDistance(0.f), 
mDirectionIndex(0), 
mHealthDisplay(nullptr), 
mMissileDisplay(nullptr) {
    mExplosion.setFrameSize(sf::Vector2i(256, 256));
    mExplosion.setNumFrames(16);
    mExplosion.setDuration(sf::seconds(1));
    Utility::centerOrigin(mExplosion);

    Utility::centerOrigin(mSprite);
    mFireCommand.category = Category::SceneAirLayer;
    mFireCommand.action = [this, &textures] (SceneNode& node, sf::Time) {
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
    if (Entity::isDestroyed() && mShowExplosion) {
        target.draw(mExplosion, states);
    }
    else {
        target.draw(mSprite, states);
    }
}

void Aircraft::updateCurrent(sf::Time dt, CommandQueue& commands) {
    updateText();
    updateRollAnimation();

    if (isDestroyed()) {
        checkPickupDrop(commands);
        mExplosion.update(dt);

        if (!mPlayedExplosionSound) {
            SoundEffect::ID soundEffect = (Utility::randomInt(2) == 2) ? SoundEffect::Explosion1 : SoundEffect::Explosion2;
            playLocalSound(commands, soundEffect);

            mPlayedExplosionSound = true;
        }

        return;
    }

    checkProjectileLaunch(dt, commands);
    updateMovementPattern(dt);
    Entity::updateCurrent(dt, commands);
    
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
    return SceneNode::getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

bool Aircraft::isMarkedForRemoval() const {
    return isDestroyed() && (mExplosion.isFinished() || !mShowExplosion);
}

void Aircraft::remove() {
    Entity::remove();
    mShowExplosion = false;
}

bool Aircraft::isAllied() const {
    return mType == Eagle;
}

float Aircraft::getMaxSpeed() const {
    return AircraftTable[mType].speed;
}

void Aircraft::increaseFireRate() {
    if (mFireRateLevel < 10) {
        ++mFireRateLevel;
    }
}

void Aircraft::increaseSpread() {
    if (mSpreadLevel < 3) {
        ++mSpreadLevel;
    }
}

void Aircraft::collectMissiles(unsigned int count) {
    mMissileAmmo += count;
}

void Aircraft::fire() {
    if (AircraftTable[mType].fireInterval != sf::Time::Zero) {
        mIsFiring = true;
    }
}

void Aircraft::launchMissile() {
    if (mMissileAmmo > 0) {
        mIsLaunchingMissile = true;
        --mMissileAmmo;
    }
}
void Aircraft::playLocalSound(CommandQueue& commands, SoundEffect::ID effect) {
    sf::Vector2f worldPosition = SceneNode::getWorldPosition();

    Command command;
    command.category = Category::SoundEffect;
    command.action = derivedAction<SoundNode>(
        [effect, worldPosition] (SoundNode& node, sf::Time)
        {
            node.playSound(effect, worldPosition);
        });
    commands.push(command);
}

void Aircraft::updateMovementPattern(sf::Time dt) {
    const std::vector<Direction>& directions = AircraftTable[mType].directions;

    if (!directions.empty()) {
        if (mTravelledDistance > directions[mDirectionIndex].distance) {
            mDirectionIndex = (mDirectionIndex + 1) % directions.size();
            mTravelledDistance = 0.f;
        }

        float radians = Utility::toRadian(directions[mDirectionIndex].angle + 90.f);
        float vx = getMaxSpeed() * std::cos(radians);
        float vy = getMaxSpeed() * std::sin(radians);

        Entity::setVelocity(vx, vy);
        mTravelledDistance += getMaxSpeed() * dt.asSeconds();
    }
}

void Aircraft::checkPickupDrop(CommandQueue& commands) {
    if ((!isAllied()) && (Utility::randomInt(3) == 0) && !mSpawnedPickup) {
        commands.push(mDropPickupCommand);
    }

    mSpawnedPickup = true;
}

void Aircraft::checkProjectileLaunch(sf::Time dt, CommandQueue& commands) {
    if (!isAllied()) {
        fire();
    }
    
    if (mIsFiring && (mFireCountdown <= sf::Time::Zero)) {
        commands.push(mFireCommand);
        playLocalSound(commands, isAllied() ? SoundEffect::AlliedGunfire : SoundEffect::EnemyGunfire);

        mFireCountdown += AircraftTable[mType].fireInterval / (mFireRateLevel + 1.f);
        mIsFiring = false;
    }
    else if (mFireCountdown > sf::Time::Zero) {
        mFireCountdown -= dt;
        mIsFiring = false;
    }

    if (mIsLaunchingMissile) {
        commands.push(mMissileCommand);
        playLocalSound(commands, SoundEffect::LaunchMissile);
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
            createProjectile(node, type, -0.33f, 0.33f, textures);
            createProjectile(node, type, 0.33f, 0.33f, textures);
            break;
        case 3:
            createProjectile(node, type, -0.5f, 0.33f, textures);
            createProjectile(node, type, 0.0f, 0.5f, textures);
            createProjectile(node, type, 0.5f, 0.33f, textures);
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
    auto type = static_cast<Pickup::Type>(Utility::randomInt(Pickup::TypeCount));

    std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
    pickup->setPosition(SceneNode::getWorldPosition());
    pickup->setVelocity(0.f, 1.f);
    node.attachChild(std::move(pickup));
}

void Aircraft::updateText() {
    if (Entity::isDestroyed()) {
        mHealthDisplay->setString("");
    }
    else {
        mHealthDisplay->setString(std::to_string(Entity::getHitpoints()) + " HP");
    }
    mHealthDisplay->setPosition(0.f, 50.f);
    mHealthDisplay->setRotation(-sf::Transformable::getRotation());
    if (mMissileDisplay) {
        if (mMissileAmmo == 0 || Entity::isDestroyed()) {
            mMissileDisplay->setString("");
        }
        else {
            mMissileDisplay->setString("M: " + std::to_string(mMissileAmmo));
        }
    }
}

void Aircraft::updateRollAnimation() {
    if (AircraftTable[mType].hasRollAnimation) {
        sf::IntRect textureRect = AircraftTable[mType].textureRect;

        if (Entity::getVelocity().x < 0.f) {
            textureRect.left += textureRect.width;
        }
        else if (Entity::getVelocity().x > 0.f) {
            textureRect.left += 2 * textureRect.width;
        }
        mSprite.setTextureRect(textureRect);
    }
}
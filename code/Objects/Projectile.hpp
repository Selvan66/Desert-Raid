#pragma once

#include "Objects/Entity.hpp"
#include "Utils/ResourceIdentifiers.hpp"
#include "Utils/Utility.hpp"

#include <vector>

struct ProjectileData {
    int damage;
    float speed;
    Textures::ID texture;
    sf::IntRect textureRect;
};

std::vector<ProjectileData> initializeProjectileData();

const std::vector<ProjectileData> ProjectileTable = initializeProjectileData();

class Projectile : public Entity {
    public:
        enum Type {
            AlliedBullet,
            EnemyBullet,
            Missile,
            TypeCount
        };
    public:
        Projectile(Type type, const TextureHolder& textures);
        void guideTowards(sf::Vector2f position);
        bool isGuided() const;
        virtual unsigned int getCategory() const;
        virtual sf::FloatRect getBoundingRect() const;
        float getMaxSpeed() const;
        int getDamage() const;
    private:
        virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
        virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
    private:
        Type mType;
        sf::Sprite mSprite;
        sf::Vector2f mTargetDirection;
};

std::vector<ProjectileData> initializeProjectileData() {
    std::vector<ProjectileData> data(Projectile::TypeCount);

    data[Projectile::AlliedBullet].damage = 10;
    data[Projectile::AlliedBullet].speed = 300.f;
    data[Projectile::AlliedBullet].texture = Textures::Entities;
    data[Projectile::AlliedBullet].textureRect = sf::IntRect(175, 64, 3, 14);

    data[Projectile::EnemyBullet].damage = 10;
    data[Projectile::EnemyBullet].speed = 300.f;
    data[Projectile::EnemyBullet].texture = Textures::Entities;
    data[Projectile::EnemyBullet].textureRect = sf::IntRect(178, 64, 3, 14);

    data[Projectile::Missile].damage = 200;
    data[Projectile::Missile].speed = 150.f;
    data[Projectile::Missile].texture = Textures::Entities;
    data[Projectile::Missile].textureRect = sf::IntRect(160, 64, 15, 32);

    return data;
}

Projectile::Projectile(Type type, const TextureHolder& textures) 
: Entity(1), mType(type), mSprite(textures.get(ProjectileTable[type].texture), ProjectileTable[type].textureRect), mTargetDirection() {
    Utility::centerOrigin(mSprite);
}

void Projectile::guideTowards(sf::Vector2f position) {
    assert(isGuided());
    mTargetDirection = Utility::unitVector(position - SceneNode::getWorldPosition());
}

bool Projectile::isGuided() const {
    return mType == Missile;
}

unsigned int Projectile::getCategory() const {
    if (mType == EnemyBullet) {
        return Category::EnemyProjectile;
    }
    else {
        return Category::AlliedProjectile;
    }
}

sf::FloatRect Projectile::getBoundingRect() const {
    return SceneNode::getWorldTransform().transformRect(mSprite.getGlobalBounds());
}

float Projectile::getMaxSpeed() const {
    return ProjectileTable[mType].speed;
}

int Projectile::getDamage() const {
    return ProjectileTable[mType].damage;
}

void Projectile::updateCurrent(sf::Time dt, CommandQueue& commands) {
    if (isGuided()) {
        const float approachRate = 200.f;

        sf::Vector2f newVelocity = Utility::unitVector(approachRate * dt.asSeconds() * mTargetDirection + Entity::getVelocity());
        newVelocity *= getMaxSpeed();
        float angle = std::atan2(newVelocity.y, newVelocity.x);

        Transformable::setRotation(Utility::toDegree(angle) + 90.f);
        Entity::setVelocity(newVelocity);
    }

    Entity::updateCurrent(dt, commands);
}

void Projectile::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
    target.draw(mSprite, states);
}
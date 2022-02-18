#pragma once

#include "Utils/ResourceIdentifiers.hpp"
#include "Objects/SpriteNode.hpp"
#include "Objects/Aircraft.hpp"
#include "Objects/ParticleNode.hpp"
#include "Game/CommandQueue.hpp"
#include "Effects/BloomEffect.hpp"

#include <array>
#include <cmath>
#include <set>
#include <algorithm>
#include <limits>

class World : private sf::NonCopyable {
    public:
        explicit World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds);
        void update(sf::Time dt);
        void draw();
        CommandQueue& getCommandQueue();
        bool hasAlivePlayer() const;
        bool hasPlayerReachedEnd() const;
    private:
        void loadTextures();
        void adaptPlayerPosition();
        void adaptPlayerVelocity();
        void handleCollisions();
        void updateSounds();
        void buildScene();
        void addEnemies();
        void addEnemy(Aircraft::Type type, float relX, float relY);
        void spawnEnemies();
        void destroyEntitiesOutsideView();
        void guideMissiles();
        sf::FloatRect getViewBounds() const;
        sf::FloatRect getBattlefieldBounds() const;

    private:
        enum Layer {
            Background,
            LowerAir,
            UpperAir,
            LayerCount
        };

        struct SpawnPoint {
            SpawnPoint(Aircraft::Type type, float x, float y)
            : type(type), x(x), y(y) {
            }

            Aircraft::Type type;
            float x, y;
        };
    private:
        sf::RenderTarget& mTarget;
        sf::RenderTexture mSceneTexture;
        sf::View mWorldView;
        TextureHolder mTextures;
        FontHolder& mFonts;
        SoundPlayer& mSounds;
        SceneNode mSceneGraph;
        std::array<SceneNode*, LayerCount> mSceneLayers;
        CommandQueue mCommandQueue;
        sf::FloatRect mWorldBounds;
        sf::Vector2f mSpawnPosition;
        float mScrollSpeed;
        Aircraft* mPlayerAircraft;
        std::vector<SpawnPoint> mEnemySpawnPoints;
        std::vector<Aircraft*> mActiveEnemies;
        BloomEffect mBloomEffect;
};

World::World(sf::RenderTarget& outputTarget, FontHolder& fonts, SoundPlayer& sounds) 
: mTarget(outputTarget), 
mSceneTexture(),
mWorldView(outputTarget.getDefaultView()), 
mTextures(), 
mFonts(fonts),
mSounds(sounds),
mSceneGraph(), 
mSceneLayers(),
mWorldBounds(0.f, 0.f, mWorldView.getSize().x, 5000.f), 
mSpawnPosition(mWorldView.getSize().x / 2.f, mWorldBounds.height - mWorldView.getSize().y / 2.f),
mScrollSpeed(-50.f), 
mPlayerAircraft(nullptr), 
mEnemySpawnPoints(), 
mActiveEnemies(),
mBloomEffect() {
    mSceneTexture.create(mTarget.getSize().x, mTarget.getSize().y);
    loadTextures();
    buildScene();
    mWorldView.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt) {
    mWorldView.move(0.f, mScrollSpeed * dt.asSeconds());
    mPlayerAircraft->setVelocity(0.f, 0.f);

    destroyEntitiesOutsideView();
    guideMissiles();

    while (!mCommandQueue.isEmpty())
        mSceneGraph.onCommand(mCommandQueue.pop(), dt);
    adaptPlayerVelocity();
    handleCollisions();
    mSceneGraph.removeWrecks();
    spawnEnemies();
    mSceneGraph.update(dt, mCommandQueue);
    adaptPlayerPosition();

    updateSounds();
}

void World::draw() {
    if (PostEffect::isSupported()) {
        mSceneTexture.clear();
        mSceneTexture.setView(mWorldView);
        mSceneTexture.draw(mSceneGraph);
        mSceneTexture.display();
        mBloomEffect.apply(mSceneTexture, mTarget);
    }
    else {
        mTarget.setView(mWorldView);
        mTarget.draw(mSceneGraph);
    }
}

CommandQueue& World::getCommandQueue() {
    return mCommandQueue;
}

bool World::hasAlivePlayer() const {
    return !mPlayerAircraft->isMarkedForRemoval();
}

bool World::hasPlayerReachedEnd() const {
    return !mWorldBounds.contains(mPlayerAircraft->getPosition());
}

void World::loadTextures() {
    mTextures.load(Textures::Entities, "../assets/Textures/Entities.png");
    mTextures.load(Textures::Jungle, "../assets/Textures/Jungle.png");
    mTextures.load(Textures::Explosion, "../assets/Textures/Explosion.png");
    mTextures.load(Textures::Particle, "../assets/Textures/Particle.png");
    mTextures.load(Textures::FinishLine, "../assets/Textures/FinishLine.png");
}

void World::adaptPlayerPosition() {
    sf::FloatRect viewBounds = getViewBounds();
    const float borderDistance = 40.f;
    sf::Vector2f position = mPlayerAircraft->getPosition();
    position.x = std::max(position.x, viewBounds.left + borderDistance);
    position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
    position.y = std::max(position.y, viewBounds.top + borderDistance);
    position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
    mPlayerAircraft->setPosition(position);
}

void World::adaptPlayerVelocity() {
    sf::Vector2f velocity = mPlayerAircraft->getVelocity();

	if (velocity.x != 0.f && velocity.y != 0.f)
		mPlayerAircraft->setVelocity(velocity / std::sqrt(2.f));

	mPlayerAircraft->accelerate(0.f, mScrollSpeed);
}

bool matchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2) {
    auto category1 = colliders.first->getCategory();
    auto category2 = colliders.second->getCategory();

    if ((type1 & category1) && (type2 & category2)) {
        return true;
    }
    else if ((type1 & category2) && (type2 & category1)) {
        std::swap(colliders.first, colliders.second);
        return true;
    }
    else {
        return false;
    }
}

void World::handleCollisions() {
    std::set<SceneNode::Pair> collisionPairs;
    mSceneGraph.checkSceneCollision(mSceneGraph, collisionPairs);

    for (auto pair : collisionPairs) {
        if (matchesCategories(pair, Category::PlayerAircraft, Category::EnemyAircraft)) {
            auto& player = static_cast<Aircraft&>(*pair.first);
            auto& enemy = static_cast<Aircraft&>(*pair.second);

            player.damage(enemy.getHitpoints());
            enemy.destroy();
        }
        else if (matchesCategories(pair, Category::PlayerAircraft, Category::Pickup)) {
            auto& player = static_cast<Aircraft&>(*pair.first);
            auto& pickup = static_cast<Pickup&>(*pair.second);

            pickup.apply(player);
            pickup.destroy();
            player.playLocalSound(mCommandQueue, SoundEffect::CollectPickup);
        }
        else if (matchesCategories(pair, Category::EnemyAircraft, Category::AlliedProjectile)
			  || matchesCategories(pair, Category::PlayerAircraft, Category::EnemyProjectile)) {
            auto& aircraft = static_cast<Aircraft&>(*pair.first);
            auto& projectile = static_cast<Projectile&>(*pair.second);

            aircraft.damage(projectile.getDamage());
            projectile.destroy();
        }
    }
}

void World::updateSounds() {
    mSounds.setListenerPosition(mPlayerAircraft->getWorldPosition());
    mSounds.removeStoppedSounds();
}

void World::buildScene() {
    for (std::size_t i = 0; i < LayerCount; ++i) {
		Category::Type category = (i == LowerAir) ? Category::SceneAirLayer : Category::None;

        SceneNode::Ptr layer(new SceneNode(category));
        mSceneLayers[i] = layer.get();

        mSceneGraph.attachChild(std::move(layer));
	}
	sf::Texture& texture = mTextures.get(Textures::Jungle);
	sf::IntRect textureRect(mWorldBounds);
	texture.setRepeated(true);

    float viewHeight = mWorldView.getSize().y;
    textureRect.height += static_cast<int>(viewHeight);

	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, textureRect));
	backgroundSprite->setPosition(mWorldBounds.left, mWorldBounds.top - viewHeight);
	mSceneLayers[Background]->attachChild(std::move(backgroundSprite));

    sf::Texture& finishTexture = mTextures.get(Textures::FinishLine);
    std::unique_ptr<SpriteNode> finishSprite(new SpriteNode(finishTexture));
    finishSprite->setPosition(0.f, -76.f);
    mSceneLayers[Background]->attachChild(std::move(finishSprite));

    std::unique_ptr<ParticleNode> smokeNode(new ParticleNode(Particle::Smoke, mTextures));
    mSceneLayers[LowerAir]->attachChild(std::move(smokeNode));

    std::unique_ptr<ParticleNode> propellantNode(new ParticleNode(Particle::Propellant, mTextures));
    mSceneLayers[LowerAir]->attachChild(std::move(propellantNode));

    std::unique_ptr<SoundNode> soundNode(new SoundNode(mSounds));
    mSceneGraph.attachChild(std::move(soundNode));

	std::unique_ptr<Aircraft> player(new Aircraft(Aircraft::Eagle, mTextures, mFonts));
	mPlayerAircraft = player.get();
	mPlayerAircraft->setPosition(mSpawnPosition);
	mSceneLayers[UpperAir]->attachChild(std::move(player));

    addEnemies();
}

void World::addEnemies() {
    addEnemy(Aircraft::Raptor,    0.f,  500.f);
	addEnemy(Aircraft::Raptor,    0.f, 1000.f);
	addEnemy(Aircraft::Raptor, +100.f, 1150.f);
	addEnemy(Aircraft::Raptor, -100.f, 1150.f);
	addEnemy(Aircraft::Avenger,  70.f, 1500.f);
	addEnemy(Aircraft::Avenger, -70.f, 1500.f);
	addEnemy(Aircraft::Avenger, -70.f, 1710.f);
	addEnemy(Aircraft::Avenger,  70.f, 1700.f);
	addEnemy(Aircraft::Avenger,  30.f, 1850.f);
	addEnemy(Aircraft::Raptor,  300.f, 2200.f);
	addEnemy(Aircraft::Raptor, -300.f, 2200.f);
	addEnemy(Aircraft::Raptor,    0.f, 2200.f);
	addEnemy(Aircraft::Raptor,    0.f, 2500.f);
	addEnemy(Aircraft::Avenger,-300.f, 2700.f);
	addEnemy(Aircraft::Avenger,-300.f, 2700.f);
	addEnemy(Aircraft::Raptor,    0.f, 3000.f);
	addEnemy(Aircraft::Raptor,  250.f, 3250.f);
	addEnemy(Aircraft::Raptor, -250.f, 3250.f);
	addEnemy(Aircraft::Avenger,   0.f, 3500.f);
	addEnemy(Aircraft::Avenger,   0.f, 3700.f);
	addEnemy(Aircraft::Raptor,    0.f, 3800.f);
	addEnemy(Aircraft::Avenger,   0.f, 4000.f);
	addEnemy(Aircraft::Avenger,-200.f, 4200.f);
	addEnemy(Aircraft::Raptor,  200.f, 4200.f);
	addEnemy(Aircraft::Raptor,    0.f, 4400.f);

    std::sort(mEnemySpawnPoints.begin(), mEnemySpawnPoints.end(), [](SpawnPoint lhs, SpawnPoint rhs) {
        return lhs.y < rhs.y;
    });
}

void World::addEnemy(Aircraft::Type type, float relX, float relY) {
    SpawnPoint spawn(type, mSpawnPosition.x + relX, mSpawnPosition.y - relY);
    mEnemySpawnPoints.push_back(spawn);
}

void World::spawnEnemies() {
    while(!mEnemySpawnPoints.empty() && mEnemySpawnPoints.back().y > getBattlefieldBounds().top) {
        SpawnPoint spawn = mEnemySpawnPoints.back();
        std::unique_ptr<Aircraft> enemy(new Aircraft(spawn.type, mTextures, mFonts));
        enemy->setPosition(spawn.x, spawn.y);
        enemy->setRotation(180.f);

        mSceneLayers[UpperAir]->attachChild(std::move(enemy));
        mEnemySpawnPoints.pop_back();
    }
}

void World::destroyEntitiesOutsideView() {
    Command command;
    command.category = Category::Projectile | Category::EnemyAircraft;
    command.action = derivedAction<Entity>([this] (Entity& e, sf::Time) {
        if (!getBattlefieldBounds().intersects(e.getBoundingRect())) {
            e.destroy();
        }
    });

    mCommandQueue.push(command);
}

void World::guideMissiles() {
    Command enemyCollector;
    enemyCollector.category = Category::EnemyAircraft;
    enemyCollector.action = derivedAction<Aircraft>([this] (Aircraft& enemy, sf::Time) {
        if (!enemy.isDestroyed())
            mActiveEnemies.push_back(&enemy);
    });

    Command missileGuider;
    missileGuider.category = Category::AlliedProjectile;
    missileGuider.action = derivedAction<Projectile>([this] (Projectile& missile, sf::Time) {
        if (!missile.isGuided())
            return;
        
        float minDistance = std::numeric_limits<float>::max();
        Aircraft* closestEnemy = nullptr;

        for (Aircraft* enemy : mActiveEnemies) {
            float enemyDistance = distance(missile, *enemy);

            if (enemyDistance < minDistance) {
                closestEnemy = enemy;
                minDistance = enemyDistance;
            }
        }

        if (closestEnemy)
            missile.guideTowards(closestEnemy->getWorldPosition());
    });

    mCommandQueue.push(enemyCollector);
    mCommandQueue.push(missileGuider);
    mActiveEnemies.clear();
}

sf::FloatRect World::getViewBounds() const {
    return sf::FloatRect(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
}

sf::FloatRect World::getBattlefieldBounds() const {
    sf::FloatRect bounds = getViewBounds();
    bounds.top -= 100.f;
    bounds.height += 100.f;

    return bounds;
}
#pragma once

#include "Objects/SceneNode.hpp"
#include "Objects/Particle.hpp"
#include "Utils/ResourceIdentifiers.hpp"

#include <deque>

const std::vector<ParticleData> ParticleTable = initializeParticleData();

class ParticleNode : public SceneNode {
    public:
        ParticleNode(Particle::Type type, const TextureHolder& textures);
        void addParticle(sf::Vector2f position);
        Particle::Type getParticleType() const;
        virtual unsigned int getCategory() const;
    private:
        virtual void updateCurrent(sf::Time dt, CommandQueue& commands);
        virtual void drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
        void addVertex(float worldX, float worldY, float texCoordX, float texCoordY, const sf::Color& color) const;
        void computeVertices() const;
    private:
        std::deque<Particle> mParticles;
        const sf::Texture& mTexture;
        Particle::Type mType;
        mutable sf::VertexArray mVertexArray;
        mutable bool mNeedsVertexUpdate;
};

ParticleNode::ParticleNode(Particle::Type type, const TextureHolder& textures) 
: SceneNode(), mParticles(), mTexture(textures.get(Textures::Particle)), mType(type), mVertexArray(sf::Quads), mNeedsVertexUpdate(true) {

}

void ParticleNode::addParticle(sf::Vector2f position) {
    Particle particle;
    particle.position = position;
    particle.color = ParticleTable[mType].color;
    particle.lifetime = ParticleTable[mType].lifetime;
    mParticles.push_back(particle);
}

Particle::Type ParticleNode::getParticleType() const {
    return mType;
}

void ParticleNode::getCategory() const {
    return Category::ParticleSystem;
}

void ParticleNode::updateCurrent(sf::Time dt, CommandQueue& commands) {
    while (!mParticle.empty() && mParticle.front().lifetime <= sf::Time::Zero)
        mParticle.pop_front();
    for (auto& particle : mParticles)
        particle.lifetime -= dt;
    mNeedsVertexUpdate = true;
}

void ParticleNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const {
    if (mNeedsVertexUpdate) {
        computeVerticles();
        mNeedsVertexUpdate = false;
    }

    states.texture = &mTexture;
    target.draw(mVertexArray, states);
}

void ParticleNode::addVertex(float worldX, float worldY, float texCoordX, float texCoordY, const sf::Color& color) const {
    sf::Vertex vertex;
    vertex.position = sf::Vector2f(worldX, worldY);
    vertex.texCoords = sf::Vector2f(texCoordX, texCoordY);
    vertex.color = color;

    mVertexArray.append(vertex);
}

void ParticleNode::computeVertices() const {
    sf::Vector2f size(mTexture.getSize());
    sf::Vector2f half = size / 2.f;

    mVertexArray.clear();
    for (const auto& particle : mParticles) {
        sf::Vector2f pos = particle.position;
        sf::Color color = particle.color;

        float ratio = particle.lifetime.asSeconds() / ParticleTable[mType].lifetime.asSeconds();
        color.a = static_cast<sf::Uint8>(255 * std::max(ratio, 0.f));

        addVertex(pos.x - half.x, pos.y - half.y, 0.f, 0.f, color);
        addVertex(pos.x - half.x, pos.y - half.y, size.x, 0.f, color);
        addVertex(pos.x - half.x, pos.y - half.y, size.x, size.y, color);
        addVertex(pos.x - half.x, pos.y - half.y, 0.f, size.y, color);
    }
}


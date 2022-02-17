#pragma once

#include "Objects/SceneNode.hpp"
#include "Utils/ResourceIdentifiers.hpp"
#include "Effects/SoundEffect.hpp"

class SoundNode : public SceneNode {
    public:
        explicit SoundNode(SoundPlayer& player);
        void playSound(SoundEffect::ID sound, sf::Vector2f position);
        virtual unsigned int getCategory() const;
    private:
        SoundPlayer& mSounds;
};

SoundNode::SoundNode(SoundPlayer& player)
: SceneNode()
, mSounds(player)
{
}

void SoundNode::playSound(SoundEffect::ID sound, sf::Vector2f position) {
    mSounds.play(sound, position);
}

unsigned int SoundNode::getCategory() const {
    return Categoty::SoundEffect;
}

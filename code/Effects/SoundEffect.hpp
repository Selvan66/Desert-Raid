#pragma once

#include "Utils/ResourceIdentifiers.hpp"

#include <SFML/System.hpp>

#include <list>

class SoundPlayer : private sf::NonCopyable {
    public:
        SoundPlayer();
        void play(SoundEffect::ID effect);
        void play(SoundEffect::ID, sf::Vector2f position);
        void removeStoppedSounds();
        void setListenerPosition(sf::Vector2f position);
        sf::Vector2f getListenerPosition() const;
    private:
        SoundBufferHolder mSoundBuffers;
        std::list<sf::Sound> mSounds;
};
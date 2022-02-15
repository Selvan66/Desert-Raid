#pragma once

#include "Utils/ResourceIdentifiers.hpp"

#include <SFML/System.hpp>

#include <list>
#include <cmath>

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

        static const float ListenerZ = 300.f;
        static const float Attenuation = 8.f;
        static const float MinDistance2D = 200.f;
        static const float MinDistance3D = std::sqrt(MinDistance2D*MinDistance2D + ListenerZ*ListenerZ);
};

SoundPlayer::SoundPlayer()
: mSoundBuffers()
, mSounds() {
    mSoundBuffers.load(SoundEffect::AlliedGunfire, "../assets/Sound/AlliedGunfire.wav");
    mSoundBuffers.load(SoundEffect::EnemyGunfire, "../assets/Sound/EnemyGunfire.wav");
    mSoundBuffers.load(SoundEffect::Explosion1, "../assets/Sound/Explosion1.wav");
    mSoundBuffers.load(SoundEffect::Explosion2, "../assets/Sound/Explosion2.wav");
    mSoundBuffers.load(SoundEffect::LaunchMissile, "../assets/Sound/LaunchMissile.wav");
    mSoundBuffers.load(SoundEffect::CollectPickup, "../assets/Sound/CollectPickup.wav");
    mSoundBuffers.load(SoundEffect::Button, "../assets/Sound/Button.wav");

    sf::Listener::setDirection(0.f, 0.f, -1.f);
}

void SoundPlayer::play(SoundEffect::ID effect) {
    play(effect, getListenerPosition());
}

void SoundPlayer::play(SoundEffect::ID, sf::Vector2f position) {
    mSounds.push_back(sf::Sound());
    sf::Sound& sound = mSounds.back();

    sound.setBuffer(mSoundBuffers.get(effect));
    sound.setPosition(position.x, -position.y, 0.0);
    sound.setAttenuation(Attenuation);
    sound.setMinDistance(MinDistance3D);

    sound.play();
}

void SoundPlayer::removeStoppedSounds() {
    mSounds.remove_if([] (const sf::Sound& s) {return s.getStatus() == sf::Sound::Stopped; });
}

void SoundPlayer::setListenerPosition(sf::Vector2f position) {
    sf::Listener::setPosition(position.x, -position.y, ListenerZ);
}

sf::Vector2f SoundPlayer::getListenerPosition() const {
    sf::Vector3f position = sf::Listener::getPosition();
    return sf::Vector2f(position.x, -position.y);
}
#pragma once

#include "Utils/ResourceIdentifiers.hpp"

#include "SFML/System.hpp"
#include "SFML/Audio.hpp"

#include <map>
#include <string>

class MusicPlayer : private sf::NonCopyable {
    public:
        MusicPlayer();
        void play(Music::ID theme);
        void stop();
        void setPaused(bool paused);
        void setVolume(float volume);
    private:
        sf::Music mMusic;
        std::map<Music::ID, std::string> mFilenames;
        float mVolume;
};

MusicPlayer::MusicPlayer() 
: mMusic()
, mFilenames()
, mVolume(100.f) {
    mFilenames[Music::MenuTheme] = "../assets/Music/MenuTheme.ogg";
    mFilenames[Music::MissionTheme] = "../assets/Music/MissionTheme.ogg";
}

void MusicPlayer::play(Music::ID theme) {
    std::string filename = mFilenames[theme];

    if (!mMusic.openFromFile(filename)) {
        throw std::runtime_error("Music " + filename + " could not be loaded");
    }

    mMusic.setVolume(mVolume);
    mMusic.setLoop(true);
    mMusic.play();
}

void MusicPlayer::stop() {
    mMusic.stop();
}

void MusicPlayer::setPaused(bool paused) {
    if (paused) {
        mMusic.pause();
    } 
    else {
        mMusic.play();
    }
}

void MusicPlayer::setVolume(float volume) {
    mVolume = volume;
}
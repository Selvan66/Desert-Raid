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
        std::map<Music::ID, std::string> mFilename;
        float mVolume;
};
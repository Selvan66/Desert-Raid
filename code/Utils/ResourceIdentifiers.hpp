#pragma once

#include "Utils/ResourceHolder.hpp"

#include <SFML/Graphics.hpp>

namespace Textures {
    enum ID {
        Entities,
        Jungle,
        TitleScreen,
        Buttons,
        Explosion,
        Particle,
        FinishLine
    };
}

namespace Fonts {
    enum ID {
        Sansation
    };
}

template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
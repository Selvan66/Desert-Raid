#pragma once

#include "ResourceHolder.hpp"

#include <SFML/Graphics.hpp>

namespace Textures {
    enum ID {
        Eagle,
        Raptor,
        Desert,
    };
}

template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
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

namespace Shader {
    enum ID {
        BrightnessPass,
        DownSamplePass,
        GaussianBlurPass,
        AddPass,
    };
}
namespace Fonts {
    enum ID {
        Sansation
    };
}

namespace Music {
    enum ID {
        MenuTheme,
        MissionTheme,
    };
}

template <typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures::ID> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts::ID> FontHolder;
typedef ResourceHolder<sf::Shader, Shader::ID> ShaderHolder;
#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

struct Particle {
    enum Type {
        Propellant,
        Smoke,
        ParticleCount
    };

    sf::Vector2f position;
    sf::Color color;
    sf::Time lifetime;
};
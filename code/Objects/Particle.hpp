#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include <vector>

struct ParticleData {
    sf::Color color;
    sf::Time lifetime;
};

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

std::vector<ParticleData> initializeParticleData() {
    std::vector<ParticleData> data(Particle::ParticleCount);
    data[Particle::Propellant].color = sf::Color(255, 255, 50);
    data[Particle::Propellant].lifetime = sf::seconds(0.6f);

    data[Particle::Smoke].color = sf::Color(50, 50, 50);
    data[Particle::Smoke].lifetime = sf::seconds(4.f);

    return data;
}
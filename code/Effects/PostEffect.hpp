#pragma once

#include "SFML/Graphics.hpp"

class PostEffect : sf::NonCopyable {
    public:
        virtual ~PostEffect();
        virtual void apply(const sf::RenderTexture& input, sf::RenderTarget& output) = 0;
        static bool isSupported();
    protected:
        static void applyShader(const sf::Shader& shader, sf::RenderTarget& output);
};

PostEffect::~PostEffect() {
}


bool PostEffect::isSupported() {
    return sf::Shader::isAvailable();
}

void PostEffect::applyShader(const sf::Shader& shader, sf::RenderTarget& output) {
    sf::Vector2f outputSize = static_cast<sf::Vector2f>(output.getSize());

    sf::VertexArray vertices(sf::TrianglesStrip, 4);
    vertices[0] = sf::Vertex(sf::Vector2f(0, 0), sf::Vector2f(0, 1));
    vertices[0] = sf::Vertex(sf::Vector2f(output.x, 0), sf::Vector2f(1, 1));
    vertices[0] = sf::Vertex(sf::Vector2f(0, output.y), sf::Vector2f(0, 0));
    vertices[0] = sf::Vertex(sf::Vector2f(output), sf::Vector2f(1, 0));

    sf::RenderState states;
    states.shader = &shader;
    states.blendMode = sf::BlendMode;

    output.draw(vertices, states);
}
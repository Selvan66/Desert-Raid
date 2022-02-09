#pragma once

#include "Effects/PostEffect.hpp"
#include "Utils/ResourceIdentifiers.hpp"

#include <array>

class BloomEffect : public PostEffect {
    public:
        BloomEffect();
        virtual void apply(const sf::RenderTexture& input, sf::RenderTarget& output);
    private:
        typedef std::array<sf::RenderTexture, 2> RenderTextureArray;
    private:
        void prepareTextures(sf::Vector2u size);
        void filterBright(const sf::RenderTexture& input, sf::RenderTexture& output);
        void blurMultipass(RenderTextureArray& renderTextures);
        void blur(const sf::RenderTexture& input, sf::RenderTexture& output, sf::Vector2f offsetFactor);
        void downsample(const sf::RenderTexture& input, sf::RenderTexture& output);
        void add(const sf::RenderTexture& source, const sf::RenderTexture& bloom, sf::RenderTarget& target);
    private:
        ShaderHolder mShaders;
        sf::RenderTexture mBrightnessTexture;
        RenderTextureArray mFirstPassTextures;
        RenderTextureArray mSecondPassTextures;
};

BloomEffect::BloomEffect() 
: mShaders(), mBrightnessTexture(), mFirstPassTextures(), mSecondPassTextures() {
    mShaders.load(Shaders::BrightnessPass, "../assets/Shaders/Fullpass.vert", "../assets/Shaders/Brightness.frag");
    mShaders.load(Shaders::DownSamplePass, "../assets/Shaders/Fullpass.vert", "../assets/Shaders/DownSample.frag");
    mShaders.load(Shaders::GaussianBlurPass, "../assets/Shaders/Fullpass.vert", "../assets/Shaders/GuassianBlur.frag");
    mShaders.load(Shaders::AddPass, "../assets/Shaders/Fullpass.vert", "../assets/Shaders/Add.frag");
}

void BloomEffect::apply(const sf::RenderTarget& input, sf::RenderTarget& output) {
    prepareTextures(input.getSize());
    filterBright(input, mBrightnessTexture);
    
    downsample(mBrightnessTexture, mFirstPassTextures[0]);
    blurMultipass(mFirstPassTextures);

    downsample(mFirstPassTextures[0], mSecondPassTextures[0]);
    blurMultipass(mSecondPassTextures);

    add(mFirstPassTextures[0], mSecondPassTextures[0], mFirstPassTextures[1]);
    mFirstPassTextures[1].display();
    add(input, mFirstPassTextures[1], output);
}

void BloomEffect::prepareTextures(sf::Vector2u size) {
    if (mBrightnessTexture.getSize() != size) {
        mBrightnessTexture.create(size.x, size.y);
        mBrightnessTexture.setSmooth(true);

        mFirstPassTextures[0].create(size.x / 2, size.y / 2);
        mFirstPassTextures[1].create(size.x / 2, size.y / 2);
        mFirstPassTextures[0].setSmooth(true);
        mFirstPassTextures[1].setSmooth(true);

        mSecondPassTextures[0].create(size.x / 2, size.y / 2);
        mSecondPassTextures[1].create(size.x / 2, size.y / 2);
        mSecondPassTextures[0].setSmooth(true);
        mSecondPassTextures[1].setSmooth(true);
    }
}

void BloomEffect::filterBright(const sf::RenderTexture& input, sf::RenderTexture& output) {
    sf::Shader& brightness = mShader.get(Shaders::BrightnessPass);

    brightness.setParameter("source", input.getTexture());
    PostEffect::applyShader(brightness, output);
    output.display();
}

void BloomEffect::blurMultipass(RenderTextureArray& renderTextures) {
    sf::Vector2u textureSize = renderTextures[0].getSize();

    for (std::size_t count = 0; count < 2; ++count) {
        blur(renderTextures[0], renderTextures[1], sf::Vector2f(0.f, 1.f / textureSize.y));
        blur(renderTextures[1], renderTextures[0], sf::Vector2f(1.f / textureSize.x, 0.f));
    }
}

void BloomEffect::blur(const sf::RenderTexture& input, sf::RenderTexture& output, sf::Vector2f offsetFactor) {
    sf::Shader& gaussianBlur = mShaders.get(Shaders::GaussianBlurPass);
    gaussianBlur.setParameter("source", input.getTexture());
    gaussianBlur.setParameter("offsetFactor", offsetFactor);
    PostEffect::applyShader(gaussianBlur, output);
    output.display();
}

void BloomEffect::downsample(const sf::RenderTexture& input, sf::RenderTexture& output) {
    sf::Shader& downSample = mShaders.get(Shaders::DownSamplePass);
    downSample.setParameter("source", input.getTexture());
    downSample.setParameter("sourceSize", sf::Vector2f(input.getSize()));
    PostEffect::applyShader(downSample, output);
    output.display();
}

void BloomEffect::add(const sf::RenderTexture& source, const sf::RenderTexture& bloom, sf::RenderTarget& target) {
    sf::Shader& adder = mShaders.get(Shaders::AddPass);

    adder.setParameter("source", source.getTexture());
    adder.setParameter("bloom", bloom.getTexture());
    PostEffect::applyShader(adder, output);
}

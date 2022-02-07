#pragma once

#include "SFML/Graphics.hpp"
#include "SFML/System.hpp"

class Animation : public sf::Drawable, public sf::Transformable {
    public:
        Animation();
        explicit Animation(const sf::Texture& texture);
        void setTexture(const sf::Texture& texture);
        const sf::Texture* getTexture() const;
        void setFrameSize(sf::Vector2i frameSize);
        sf::Vector2i getFrameSize() const;
        void setNumFrames(std::size_t numFrames);
        std::size_t getNumFrames() const;
        void setDuration(sf::Time duration);
        sf::Time getDuration() const;
        void setRepeating(bool flag);
        bool isRepeating() const;
        void restart();
        bool isFinished() const;
        sf::FloatRect getLocalBounds() const;
        sf::FloatRect getGlobalBounds() const;
        void update(sf::Time dt);
    private:
        void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    private:
        sf::Sprite mSprite;
        sf::Vector2i mFrameSize;
        std::size_t mNumFrames;
        std::size_t mCurrentFrame;
        sf::Time mDuration;
        sf::Time mElapsedTime;
        bool mRepeat;
};

Animation::Animation() 
: mSprite(), mFrameSize(), mNumFrames(), mCurrentFrame(0),
mDuration(sf::Time::Zero), mElapsedTime(sf::Time::Zero), mRepeat(false) {
}

Animation::Animation(const sf::Texture& texture) 
: mSprite(texture), mFrameSize(), mNumFrames(0), mCurrentFrame(0), 
mDuration(sf::Time::Zero), mElapsedTime(sf::Time::Zero), mRepeat(false) {
}

void Animation::setTexture(const sf::Texture& texture) {
    mSprite.setTexture(texture);
}

const sf::Texture* Animation::getTexture() const {
    return mSprite.getTexture();
}

void Animation::setFrameSize(sf::Vector2i frameSize) {
    mFrameSize = frameSize;
}

sf::Vector2i Animation::getFrameSize() const {
    return mFrameSize;
}

void Animation::setNumFrames(std::size_t numFrames) {
    mNumFrames = numFrames;
}

std::size_t Animation::getNumFrames() const {
    return mNumFrames;
}

void Animation::setDuration(sf::Time duration) {
    mDuration = duration;
}

sf::Time Animation::getDuration() const {
    return mDuration;
}

void Animation::setRepeating(bool flag) {
    mRepeat = flag;
}

bool Animation::isRepeating() const {
    return mRepeat;
}

void Animation::restart() {
    mCurrentFrame = 0;
}

bool Animation::isFinished() const {
    return mCurrentFrame >= mNumFrames;
}

sf::FloatRect Animation::getLocalBounds() const {
    return sf::FloatRect(sf::Transformable::getOrigin(), static_cast<sf::Vector2f>(getFrameSize()));
}

sf::FloatRect Animation::getGlobalBounds() const {
    return sf::Transformable::getTransform().transformRect(getLocalBounds());
}

void Animation::update(sf::Time dt) {
    sf::Time timePerFrame = mDuration / static_cast<float>(mNumFrames);
    mElapsedTime += dt;

    sf::Vector2i textureBounds(mSprite.getTexture()->getSize());
    sf::IntRect textureRect = mSprite.getTextureRect();

    if (mCurrentFrame == 0) {
        textureRect = sf::IntRect(0, 0, mFrameSize.x, mFrameSize.y);
    }

    while (mElapsedTime >= timePerFrame && (mCurrentFrame <= mNumFrames || mRepeat)) {
        textureRect.left += textureRect.width;

        if (textureRect.left + textureRect.width > textureBounds.x) {
            textureRect.left = 0;
            textureRect.top += textureRect.height;
        }

        mElapsedTime -= timePerFrame;

        if (mRepeat) {
            mCurrentFrame = (mCurrentFrame + 1) % mNumFrames;
            if (mCurrentFrame == 0) {
                textureRect = sf::IntRect(0, 0, mFrameSize.x, mFrameSize.y);
            }
        }
        else {
            mCurrentFrame++;
        }
    }
    mSprite.setTextureRect(textureRect);
}

void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= sf::Transformable::getTransform();
    target.draw(mSprite, states);
}


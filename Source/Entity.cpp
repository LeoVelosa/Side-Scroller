#include "Entity.hpp"


void Entity::setVelocity(sf::Vector2f velocity)
{
	mVelocity = velocity;
}

void Entity::setVelocity(float vx, float vy)
{
	mVelocity.x = vx;
	mVelocity.y = vy;
}

void Entity::setRotation(float r)
{
	mRotation = r;
}

sf::Vector2f Entity::getVelocity() const
{
	return mVelocity;
}

void Entity::accelerate(sf::Vector2f velocity)
{
	mVelocity += velocity;
}

void Entity::accelerate(float vx, float vy)
{
	mVelocity.x += vx;
	mVelocity.y += vy;
}

void Entity::rotation(float r)
{
	mRotation += r;
}

void Entity::updateCurrent(sf::Time dt)
{	
	move(mVelocity * dt.asSeconds());
	//rotate(mRotation * dt.asSeconds());
}

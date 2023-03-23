#pragma once

#include "SceneNode.hpp"


class Entity : public SceneNode
{
	public:
		void				setVelocity(sf::Vector2f velocity);
		void				setVelocity(float vx, float vy);
		void 				setRotation(float r);
		void				accelerate(sf::Vector2f velocity);
		void				accelerate(float vx, float vy);
		void 				rotation(float r);
		sf::Vector2f		getVelocity() const;


	private:
		virtual void		updateCurrent(sf::Time dt);


	private:
		sf::Vector2f		mVelocity;
		float				mRotation;
};

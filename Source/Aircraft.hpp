#pragma once

#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/Sprite.hpp>


class Aircraft : public Entity
{
	public:
		enum Type
		{
			Character,
			Asteroid,
			Spike,
		};


	public:
								Aircraft(Type type, const TextureHolder& textures);
		virtual unsigned int	getCategory() const;
		sf::FloatRect 			getBounds();

	private:
		virtual void			drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;


	private:
		Type					mType;
		sf::Sprite				mSprite;
		sf::FloatRect			mBounds;
};

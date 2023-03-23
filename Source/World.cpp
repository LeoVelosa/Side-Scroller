#include "World.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include "StringHelpers.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <ctime>

World::World(sf::RenderWindow& window)
: mWindow(window)
, mWorldView(window.getDefaultView())
, mTextures() 
, mSceneGraph()
, mSceneLayers()
, mWorldBounds(0.f, 0.f, -4000.f, mWorldView.getSize().x)
, mSpawnPosition(mWorldView.getSize().x / 2.f, mWorldBounds.height - (mWorldView.getSize().y / 2.f))
, mScrollSpeed(-50.f)
, mPlayerAircraft(nullptr)
, mAsteroid(nullptr)
, mSpike(nullptr)
, mHealth()
, mAsteroidTime()
, mSpikeTime()
, mTracker()
{
	loadTextures();
	buildScene();
	sf::Time elapsedTime = sf::Time::Zero;
	mHealth = 10;
	mAsteroidTime = sf::Time::Zero;
	mSpikeTime = sf::Time::Zero;
	// Prepare the view
	mWorldView.setCenter(mSpawnPosition);
}

void World::update(sf::Time dt)
{
	sf::Clock clock;
	// Scroll the world, reset player velocity
	mWorldView.move(-(mScrollSpeed * dt.asSeconds()), 0.f);	
	mPlayerAircraft->setVelocity(50.f, 200.f);

	// Forward commands to scene graph, adapt velocity (scrolling, diagonal correction)
	while (!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop(), dt);
	adaptPlayerVelocity();

	// Regular update step, adapt position (correct if outside view)
	mSceneGraph.update(dt);
	updateHealth();
	spawnObstacles(dt, clock);
	adaptPlayerPosition();
}

void World::draw()
{
	mWindow.setView(mWorldView);
	mWindow.draw(mSceneGraph);
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

void World::loadTextures()
{
	mTextures.load(Textures::Character, "../Media/Textures/sidechar.png");
	mTextures.load(Textures::Background, "../Media/Textures/background.jpg");
	mTextures.load(Textures::Asteroid, "../Media/Textures/Asteroid.png");
	mTextures.load(Textures::Spike, "../Media/Textures/rotatingspike.png");
}

void World::buildScene()
{	

	// Initialize the different layers
	for (std::size_t i = 0; i < LayerCount; ++i)
	{
		SceneNode::Ptr layer(new SceneNode());
		mSceneLayers[i] = layer.get();

		mSceneGraph.attachChild(std::move(layer));
	}

	// Prepare the tiled background
	sf::Texture& texture = mTextures.get(Textures::Background);
	sf::IntRect textureRect(mWorldBounds);
	texture.setRepeated(true);

	// Add the background sprite to the scene
	std::unique_ptr<SpriteNode> backgroundSprite(new SpriteNode(texture, textureRect));
	backgroundSprite->setPosition(mWorldBounds.left, mWorldBounds.top);
	mSceneLayers[Background]->attachChild(std::move(backgroundSprite));

	// Add player's aircraft
	std::unique_ptr<Aircraft> leader(new Aircraft(Aircraft::Character, mTextures));
	mPlayerAircraft = leader.get();
	mPlayerAircraft->setPosition(mSpawnPosition.x, mSpawnPosition.y+250.f);
	mSceneLayers[Air]->attachChild(std::move(leader));

	// Asteroid
	std::unique_ptr<Aircraft> asteroid(new Aircraft(Aircraft::Asteroid, mTextures));
	mAsteroid = asteroid.get();
	mAsteroid->setPosition(320,1500);
	mAsteroid->setVelocity(50.f,50.f);
	mSceneLayers[Air]->attachChild(std::move(asteroid));

	// Spike
	std::unique_ptr<Aircraft> spike(new Aircraft(Aircraft::Spike, mTextures));
	mSpike = spike.get();
	mSpike->setPosition(800,100);
	mSpike->setVelocity(-50.f,0.f);
	mSceneLayers[Air]->attachChild(std::move(spike));

}

void World::adaptPlayerPosition()
{
	// Keep player's position inside the screen bounds, at least borderDistance units from the border
	sf::FloatRect viewBounds(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
	const float borderDistance = 75.f;

	sf::Vector2f position = mPlayerAircraft->getPosition();
	position.x = std::max(position.x, viewBounds.left + borderDistance);
	position.x = std::min(position.x, viewBounds.left + viewBounds.width - borderDistance);
	position.y = std::max(position.y, viewBounds.top + borderDistance);
	position.y = std::min(position.y, viewBounds.top + viewBounds.height - borderDistance);
	mPlayerAircraft->setPosition(position);
}

void World::adaptPlayerVelocity()
{
	sf::Vector2f velocity = mPlayerAircraft->getVelocity();

	// If moving diagonally, reduce velocity (to have always same velocity)
	if (velocity.x != 0.f && velocity.y != 0.f)
		mPlayerAircraft->setVelocity(velocity / std::sqrt(2.f));

	// Add scrolling velocity
	mPlayerAircraft->accelerate(mScrollSpeed, 0.f);
}

void World::updateHealth()
{
	sf::FloatRect viewBounds(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
	if(mPlayerAircraft->getBounds().intersects(mAsteroid->getBounds()))
	{
		mAsteroid->setPosition(mWorldView.getCenter().x + random(-100.f, 100.f), viewBounds.top);
		mAsteroid->setVelocity(random(-50.f, 50.f), 60.f);
		mHealth -= 1;
		mAsteroidTime = sf::Time::Zero;
	}
	if(mPlayerAircraft->getBounds().intersects(mSpike->getBounds()))
	{
		mSpike->setPosition(mWorldView.getCenter().x + 400.f,mWorldView.getCenter().y + random(-100.f, 100.f));
		mSpike->setVelocity(-50.f, 0.f);
		mHealth -= 1;
		mSpikeTime = sf::Time::Zero;
	}
}

int World::getHealth()
{
	return mHealth;
}

void World::spawnObstacles(sf::Time elapsedTime, sf::Clock clock)
{
	sf::FloatRect viewBounds(mWorldView.getCenter() - mWorldView.getSize() / 2.f, mWorldView.getSize());
	mAsteroidTime += elapsedTime;
	mSpikeTime += elapsedTime;
	if(mAsteroidTime > sf::seconds(7.0f)) {
		mTracker += 500;
		mAsteroid->setPosition(mWorldView.getCenter().x + random(-100.f, 100.f), viewBounds.top);
		mAsteroid->setVelocity(random(-50.f, 50.f), 60.f);
		mAsteroidTime = sf::Time::Zero;
	}
	if(mSpikeTime > sf::seconds(10.f)) {
		mSpike->setPosition(mWorldView.getCenter().x + 400.f,mWorldView.getCenter().y + random(-100.f, 100.f));
		mSpike->setVelocity(-50.f, 0.f);
		mSpikeTime = sf::Time::Zero;
	}
}

float World::random(float min, float max)
{
	srand((unsigned int)time(NULL));
	float random = ((float) rand()) / (float) RAND_MAX;
	float diff = max - min;
	float r = random * diff;
	return min + r;
}

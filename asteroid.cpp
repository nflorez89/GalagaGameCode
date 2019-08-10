#include "asteroid.h"

float Asteroid::lastXLocation = 0.0;

//=============================================================================
// constructor
//=============================================================================
Asteroid::Asteroid() : Entity()
{
	startFrame = asteroidNS::ASTEROID1_START_FRAME;     // first frame of ship animation
    endFrame     = asteroidNS::ASTEROID1_END_FRAME;     // last frame of ship animation
	currentFrame = startFrame;
	radius = asteroidNS::WIDTH/2.0;
	collisionType = entityNS::CIRCLE;
	spriteData.x = asteroidNS::X;                   // location on screen
    spriteData.y = asteroidNS::Y;
    rotation = 0.0f;
    velocity.x = 0;
    velocity.y = 0;
}

//=============================================================================
// Initialize the Entity.
// Pre: *gamePtr = pointer to Game object
//      width = width of Image in pixels  (0 = use full texture width)
//      height = height of Image in pixels (0 = use full texture height)
//      ncols = number of columns in texture (1 to n) (0 same as 1)
//      *textureM = pointer to TextureManager object
// Post: returns true if successful, false if failed
//=============================================================================
bool Asteroid::initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM)
{
	float upperBound, lowerBound;

	std::default_random_engine generator(static_cast<unsigned int>(time(0)));
	std::uniform_int_distribution<int> distributionSize(1,3);
	std::uniform_int_distribution<int> distributionRotationSpeed(1,10);
	std::uniform_int_distribution<int> distributionRotationDir(0,1);
	

	// decide the range depending on where the last asteroid was spawned
	if(lastXLocation < (GAME_WIDTH/2.0))
	{
		lowerBound = lastXLocation;
		upperBound = GAME_WIDTH - asteroidNS::WIDTH;
	}
	else
	{
		lowerBound = 0;
		upperBound = lastXLocation - asteroidNS::WIDTH;
	}

	// create distribution object for range between lower and uper bound
	std::uniform_int_distribution<float> distributionXLoc(lowerBound, upperBound);

	std::uniform_int_distribution<float> distributionVertSpeed(10,20);

	int size = distributionSize(generator); // get a number between 1 and 3
	int rotationSpeed = distributionRotationSpeed(generator); // get a number between 1 and 10
	int rotDir = distributionRotationDir(generator); // get a number between 0 and 1
	float xLoc = distributionXLoc(generator); // get a number between the lower and upper bounds determined by the previous asteroid's spawn location
	float yVel = distributionVertSpeed(generator); // get number between 1 and 10


	spriteData.x = float(xLoc);

	// set the last x location for use by the next generated asteroid
	lastXLocation = float(xLoc);

	switch(size)
	{
	case 1:
		spriteData.scale = 0.5;
		rotationRate = rotationSpeed * asteroidNS::SMALL_ROTATION_FACTOR;
		velocity.y = yVel * asteroidNS::SMALL_SPEED;
		mass = 600.0;
		break;
	case 2:
		spriteData.scale = 1.0;
		rotationRate = rotationSpeed * asteroidNS::MEDIUM_ROTATION_FACTOR;
		velocity.y = yVel * asteroidNS::MEDIUM_SPEED;
		mass = 1200.0;
		break;
	case 3:
		spriteData.scale = 1.5;
		rotationRate = rotationSpeed * asteroidNS::LARGE_ROTATION_FACTOR;
		velocity.y = yVel * asteroidNS::LARGE_SPEED;
		mass = 1800.0;
		break;
	default:
		spriteData.scale = 1.0;
		rotationRate = rotationSpeed * asteroidNS::MEDIUM_ROTATION_FACTOR;
		velocity.y = yVel * asteroidNS::MEDIUM_SPEED;
		mass = 1200.0;
	}

	if (rotDir == 0)
		rotationDirection = LEFT;
	else
		rotationDirection = RIGHT;

	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void Asteroid::update(float frameTime)
{

	spriteData.x += frameTime * velocity.x;     // move ship along X 
    spriteData.y += frameTime * velocity.y;     // move ship along Y

    Entity::update(frameTime);

	rotation = frameTime * rotationRate * rotationDirection;

    spriteData.angle += frameTime * rotation;   // apply rotation

	
}
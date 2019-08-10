#include "gravitationalBody.h"

//=============================================================================
// constructor
//=============================================================================
GravitationalBody::GravitationalBody() : Entity()
{
	startFrame = gravitationalBodyNS::PLANET_START_FRAME;     // planet has no animation
    endFrame     = gravitationalBodyNS::PLANET_END_FRAME;     // planet has no animation
	currentFrame = startFrame;
	radius = gravitationalBodyNS::WIDTH/2.0;
	collisionType = entityNS::CIRCLE;
	spriteData.x = gravitationalBodyNS::X;                   // location on screen
    spriteData.y = gravitationalBodyNS::Y;
    velocity.x = 0;
    velocity.y = 0;
	active = false;
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
bool GravitationalBody::initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM)
{
	setActive(true);

	std::default_random_engine generator(static_cast<unsigned int>(time(0)));
	std::uniform_int_distribution<int> distributionSize(1,3);

	// create distribution object for range between lower and uper bound
	std::uniform_int_distribution<float> distributionXLoc(-((gravitationalBodyNS::WIDTH/2.0f) * spriteData.scale), GAME_WIDTH - ((gravitationalBodyNS::WIDTH/2.0f) * spriteData.scale));

	int size = distributionSize(generator); // get a number between 1 and 3
	float xLoc = distributionXLoc(generator); // get a number between the lower and upper bounds determined by the previous asteroid's spawn location

	spriteData.x = float(xLoc);

	switch(size)
	{
	case 1:
		spriteData.scale = 0.5;
		mass = 1.0e14f;
		break;
	case 2:
		spriteData.scale = 1.0;
		mass = 1.0e14f;
		break;
	case 3:
		spriteData.scale = 1.5;
		mass = 1.0e14f;
		break;
	default:
		spriteData.scale = 1.0;
		mass = 1.0e14f;
	}

	spriteData.y = -(spriteData.scale * gravitationalBodyNS::HEIGHT);

	velocity.y = gravitationalBodyNS::VERTICAL_SPEED;

	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void GravitationalBody::update(float frameTime)
{
	if(!active)
	{
		return;
	}

	if(spriteData.y > GAME_HEIGHT)
	{
		setActive(false);
	}
	else
	{
		spriteData.x += frameTime * velocity.x;     // move ship along X 
		spriteData.y += frameTime * velocity.y;     // move ship along Y

		Entity::update(frameTime);

		if(spriteData.y > GAME_HEIGHT)
		{
			active = false;
		}
	}
	
}
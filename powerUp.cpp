#include "powerUp.h"

//=============================================================================
// constructor
//=============================================================================
PowerUp::PowerUp() : Entity()
{
	//startFrame = powerUpNS::START_FRAME;     // first frame of ship animation
    //endFrame     = powerUpNS::END_FRAME;     // last frame of ship animation
	//currentFrame = startFrame;
	radius = powerUpNS::WIDTH/2.0;
	collisionType = entityNS::CIRCLE;
    velocity.x = 0;
    velocity.y = 0;
	active = false;
	visible = false;
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
bool PowerUp::initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM)
{
	float xLoc;
	short powerType;

	active = true;
	visible = true;

	std::default_random_engine generator(static_cast<unsigned int>(time(0)));

	// randomly generate the x coord of where the power up will spawn as well as what power up it is
	std::uniform_int_distribution<float> distributionXLoc(0, GAME_WIDTH - powerUpNS::WIDTH * spriteData.scale);
	std::uniform_int_distribution<int> distributionPowerType(0, 1);

	xLoc = distributionXLoc(generator);
	powerType = distributionPowerType(generator);

	if (powerType == 0)
	{
		powerUpType = powerUpNS::INVINCIBILITY;
		startFrame = powerUpNS::START_FRAME_INVINCIBILITY;
		endFrame = powerUpNS::END_FRAME_INVINCIBILITY;
	}
	else
	{
		powerUpType = powerUpNS::BLACK_HOLE;
		startFrame = powerUpNS::START_FRAME_BLACK_HOLE;
		endFrame = powerUpNS::END_FRAME_BLACK_HOLE;
	}

	setFrames(startFrame, endFrame);
	setCurrentFrame(startFrame);

	// power up does not move in the x direction
	velocity.x = 0.0f;

	spriteData.x = float(xLoc);
	spriteData.y = -spriteData.scale * height;

	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void PowerUp::update(float frameTime)
{
	if(!active)
		return;
	velocity.y = powerUpNS::SPEED * frameTime;

	spriteData.x += frameTime * velocity.x;     // move power up along X 
    spriteData.y += frameTime * velocity.y;     // move power up along Y

	// the power up went off screen. Deactivate it.
	if (spriteData.y > GAME_HEIGHT)
		active = false;

    Entity::update(frameTime);
	
}
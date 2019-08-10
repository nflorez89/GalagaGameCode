#include "blackHole.h"

//=============================================================================
// constructor
//=============================================================================
BlackHole::BlackHole() : Entity()
{
	startFrame = blackHoleNS::BLACK_HOLE_START_FRAME;     // planet has no animation
    endFrame     = blackHoleNS::BLACK_HOLE_END_FRAME;     // planet has no animation
	currentFrame = startFrame;
	radius = blackHoleNS::WIDTH/2.0;
	collisionType = entityNS::CIRCLE;
	spriteData.x = blackHoleNS::X;                   // location on screen
    spriteData.y = blackHoleNS::Y;
	spriteData.scale = 1.0f;
    velocity.x = 0;
    velocity.y = 0;
	frameDelay = 0.1f;
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
bool BlackHole::initialize(Game *gamePtr, int width, int height, int ncols,
                            TextureManager *textureM, float duration)
{

	totalStayTime = duration;

	stayTime = totalStayTime;

	//std::default_random_engine generator(static_cast<unsigned int>(time(0)));

	// create distribution object for range between lower and uper bound
	//std::uniform_int_distribution<float> distributionXLoc(0, GAME_WIDTH - (blackHoleNS::WIDTH * spriteData.scale));
	//std::uniform_int_distribution<float> distributionYLoc(0, GAME_HEIGHT - (blackHoleNS::HEIGHT * spriteData.scale));

	//float xLoc = distributionXLoc(generator);
	//float yLoc = distributionYLoc(generator); 

	// todo: put this back
	//spriteData.x = xLoc;
	//spriteData.y = yLoc;
	spriteData.x = GAME_WIDTH/2.0f;
	spriteData.y = GAME_HEIGHT/2.0f;

	return(Entity::initialize(gamePtr, width, height, ncols, textureM));
}

//=============================================================================
// update
// typically called once per frame
// frameTime is used to regulate the speed of movement and animation
//=============================================================================
void BlackHole::update(float frameTime)
{
	if(!active)
	{
		return;
	}

	stayTime -= frameTime;                     // time remaining until black hole disappears

	if(stayTime < 0)                           // if black hole has stayed the entire duration
    {
        visible = false;                        // black hole off
        active = false;
    }

	Entity::update(frameTime);

}